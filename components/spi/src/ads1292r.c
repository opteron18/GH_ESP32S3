#include "ads1292r.h"
#include "freertos/task.h"
#include <string.h>
#include "global_config.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "esp_timer.h"
#include "uart.h"
static const char *TAG = "ADS1292R";

static void spi_readwrite(ADS1292R_t *dev, uint8_t *tx, uint8_t *rx, size_t len) {
    spi_transaction_t t = {
        .length = len * 8,
        .tx_buffer = tx,
        .rx_buffer = rx,
    };
    // ESP_LOGI("SPI","gpio_set_level"); 
    gpio_set_level(dev->cs_pin, 0);
    spi_device_transmit(dev->spi, &t);
    gpio_set_level(dev->cs_pin, 1);
}

void ads1292r_send_cmd(ADS1292R_t *dev, uint8_t cmd) {
    uint8_t tx = cmd, rx = 0;
    spi_readwrite(dev, &tx, &rx, 1);
}

void ads1292r_write_regs(ADS1292R_t *dev, uint8_t addr, uint8_t *data, uint8_t len) {
    // uint8_t cmd[2] = { ADS1292R_WREG | addr, len - 1 };
    // spi_readwrite(dev, cmd, NULL, 2);
    // spi_readwrite(dev, data, NULL, len);
    uint8_t tx_buf[2 + len];

    tx_buf[0] = ADS1292R_WREG | addr;  // WREG command + register address
    tx_buf[1] = len - 1;               // number of registers - 1

    memcpy(&tx_buf[2], data, len);     // append data to be written

    spi_readwrite(dev, tx_buf, NULL, 2 + len);  // One single SPI transaction
}

void ads1292r_read_regs(ADS1292R_t *dev, uint8_t addr, uint8_t *data, uint8_t len) {
    // uint8_t cmd[2] = { ADS1292R_RREG | addr, len - 1 };
    // uint8_t dummy[12] = {0};
    // spi_readwrite(dev, cmd, NULL, 2);
    // spi_readwrite(dev, dummy, data, len);

    uint8_t tx_buf[2 + len];
    uint8_t rx_buf[2 + len];

    tx_buf[0] = ADS1292R_RREG | addr;  // RREG command + register address
    tx_buf[1] = len - 1;               // number of registers - 1

    memset(&tx_buf[2], 0x00, len);     // fill dummy bytes

    spi_readwrite(dev, tx_buf, rx_buf, 2 + len); // One single SPI transaction

    memcpy(data, &rx_buf[2], len);    // Extract only the register data
    
}

void ads1292r_reset(ADS1292R_t *dev) {
    gpio_set_level(dev->cs_pin, 1);
    gpio_set_level(dev->start_pin, 0);
    gpio_set_level(dev->pwdn_pin, 0);
    vTaskDelay(pdMS_TO_TICKS(50));
    gpio_set_level(dev->pwdn_pin, 1);
    vTaskDelay(pdMS_TO_TICKS(10));
    ads1292r_send_cmd(dev, ADS1292R_SDATAC);
    ads1292r_send_cmd(dev, ADS1292R_STOP);
}

esp_err_t ads1292r_init(ADS1292R_t *dev, spi_host_device_t host) {
    // 初始化GPIO
    // gpio_config_t io_conf = {
    //     .mode = GPIO_MODE_OUTPUT,
    //     .pin_bit_mask = (1ULL << dev->cs_pin) | (1ULL << dev->start_pin) | (1ULL << dev->pwdn_pin),
    // };
    // gpio_config(&io_conf);
    // gpio_set_direction(dev->drdy_pin, GPIO_MODE_INPUT);

    gpio_reset_pin(dev->drdy_pin);   //四个单独gpio控制
    gpio_reset_pin(dev->start_pin);
    gpio_reset_pin(dev->pwdn_pin);
    gpio_reset_pin(dev->cs_pin);
    gpio_set_direction(dev->drdy_pin, GPIO_MODE_INPUT);    //数据准备接口使用输入
    gpio_set_direction(dev->start_pin, GPIO_MODE_OUTPUT);
    gpio_set_direction(dev->pwdn_pin, GPIO_MODE_OUTPUT);
    gpio_set_direction(dev->cs_pin, GPIO_MODE_OUTPUT);

    // 初始化SPI
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 500 * 1000,
        .mode = 1,
        .spics_io_num = -1,
        .queue_size = 10,
    };
    spi_bus_add_device(host, &devcfg, &dev->spi);

    ads1292r_reset(dev);

    #if (USE_EVT_ADS1192)
    ads1292r_write_regs(dev, 0x01, ads1192_default_config, 11);
    #endif
    #if (USE_ADS1292R)
    ads1292r_write_regs(dev, 0x01, ads1292r_default_config, 11);
    #endif
    #if (USE_DVT_ADS1292R)
    ads1292r_write_regs(dev, 0x01, ads1292r_default_config, 11);
    #endif

    vTaskDelay(pdMS_TO_TICKS(10));
    
    ads1292r_read_regs(dev, 0x00, dev->reg, 12);

    for (int i = 0; i < 12; ++i) {
        ESP_LOGI(TAG, "Reg[%02d] = 0x%02X", i, dev->reg[i]);
    }

    return ESP_OK;
}

void ads1292r_start(ADS1292R_t *dev) {
    ads1292r_send_cmd(dev, ADS1292R_RDATAC);
    gpio_set_level(dev->start_pin, 1);
}

char data[80];

void ads1292r_stop(ADS1292R_t *dev) {
    gpio_set_level(dev->start_pin, 0);
}

esp_err_t  ads1292r_read_data(ADS1292R_t *dev, float *ch_val) {

    #if (USE_ADS1292R)
    uint8_t buf[9] = {0};
    uint8_t dummy[9] = {0};
    spi_readwrite(dev, dummy, buf, 9);

    int32_t raw_ch1 = (buf[3] << 16) | (buf[4] << 8) | buf[5];
    int32_t raw_ch2 = (buf[6] << 16) | (buf[7] << 8) | buf[8];
    #endif

    #if (USE_ADS1292R_TWO_CORE)
    uint8_t buf[9] = {0};
    uint8_t dummy[9] = {0};
    spi_readwrite(dev, dummy, buf, 9);

    int32_t raw_ch1 = (buf[3] << 16) | (buf[4] << 8) | buf[5];
    int32_t raw_ch2 = (buf[6] << 16) | (buf[7] << 8) | buf[8];
    #endif

    #if (USE_EVT_ADS1192)
    uint8_t buf[7];
    uint8_t dummy[7] = {0};
    spi_readwrite(dev, dummy, buf, 7);

    int16_t raw_ch1 = (buf[3] << 8) | buf[4];
    int16_t raw_ch2 = (buf[5] << 8) | buf[6];
    #endif

    // #if (USE_ADS1292R)
    // if (raw_ch1 & 0x800000) raw_ch1 |= 0xFF000000;
    // if (raw_ch2 & 0x800000) raw_ch2 |= 0xFF000000;
    // #endif

    #if (USE_ADS1292R_TWO_CORE)
    if (raw_ch1 & 0x800000) raw_ch1 |= 0xFF000000;
    if (raw_ch2 & 0x800000) raw_ch2 |= 0xFF000000;
    #endif

    #if (USE_EVT_ADS1192)
    if (raw_ch1 & 0x8000) raw_ch1 |= 0xFFFF0000;
    if (raw_ch2 & 0x8000) raw_ch2 |= 0xFFFF0000;
    #endif

    #if (USE_DVT_ADS1292R)
    
    uint8_t buf[9] = {0};
    uint8_t dummy[9] = {0};
    spi_readwrite(dev, dummy, buf, 9);

    // int32_t raw_ch1 = (buf[3] << 24) | (buf[4] << 16) | buf[5] << 8;
    // int32_t raw_ch2 = (buf[6] << 24) | (buf[7] << 16) | buf[8] << 8;

    int32_t raw_ch1 = (buf[3] << 16) | (buf[4] << 8) | buf[5];
    int32_t raw_ch2 = (buf[6] << 16) | (buf[7] << 8) | buf[8];

    // if (raw_ch1 & 0x8000) raw_ch1 |= 0xFF000000;
    // if (raw_ch2 & 0x8000) raw_ch2 |= 0xFF000000;
    #endif

    // ch_val[0] = ((float)raw_ch1);
    // ch_val[1] = ((float)raw_ch2);

    // ch_val[0] = ((float)raw_ch1 / 256.0f) * 0.288486f/12;
    // ch_val[1] = ((float)raw_ch2 / 256.0f) * 0.288486f/12;

    ch_val[0] = (float)raw_ch1 * 0.288486f/6.f;
    ch_val[1] = (float)raw_ch2 * 0.288486f/6.f;


    // uint8_t buf[4];
    // uint8_t dummy[4] = {0};
    // spi_readwrite(dev, dummy, buf, 7);

    // int16_t raw_ch1 = (buf[0] << 8) | buf[1];
    // int16_t raw_ch2 = (buf[2] << 8) | buf[2];
    // ESP_LOGI(TAG,"ch1: %ld",raw_ch1);
    // ESP_LOGI(TAG,"ch2: %ld",raw_ch2);

    // float test_data[2];
    // test_data[0] = 1000.f;
    // test_data[1] = 5000.f;

    // char data[50];
    // int len = snprintf(data, sizeof(data), "data:%.3f,%.3f\n", ch_val[0], ch_val[1]);
    // printf("Sending data: %s", data);
    static int64_t start_us = -1;
    static int64_t last_n = -1;
    const int64_t sample_period_us = 2000; // 500 Hz -> 2 ms per sample
    // int64_t now_us = esp_timer_get_time();
    // if (start_us < 0) {
    //     start_us = now_us;
    //     last_n = -1;
    // }
    // int64_t elapsed_us = now_us - start_us;
    // Round to nearest 2 ms step to align exactly to 500 Hz grid
    // int64_t n = (elapsed_us + sample_period_us / 2) / sample_period_us;
    // if (n <= last_n) {
    //     n = last_n + 1; // enforce monotonic increment in case of jitter
    // }
    // last_n = n;
    // double t_sec = (double)n * 0.002; // aligned timestamp in seconds

    // int len = snprintf(data, sizeof(data), "data:%.6f,%.6f,%.6f\n", ch_val[0], ch_val[1], t_sec);
    int len = snprintf(data, sizeof(data), "data:%.6f,%.6f\n", ch_val[0], ch_val[1]);

    uart_send_data((const uint8_t *)data, len, portMAX_DELAY);

    // uart_write_bytes(UART_NUM_0, data, len);

    // data = ["data:",ch_val[0],ch_val[1]];

    // ESP_LOGI(TAG,"ch2: %ld",raw_ch2);
    // float lsb_uV = 2.42 / 32767.0f;  // LSB 电压权重（VREF = 2.4V）

    // ch_val[0] = (float)raw_ch1 * lsb_uV;   // 通道1电压（单位：V）
    // ch_val[1] = (float)raw_ch2 * lsb_uV;

    // ch_val[0] = ch_val[0] ;   // 如果用了增益为6
    // ch_val[1] = ch_val[1] ;
    // ch_val[0] = ((float)raw_ch1 ) / 6;
    // ch_val[1] = ((float)raw_ch2 )  / 6;

    return ESP_OK;
}
