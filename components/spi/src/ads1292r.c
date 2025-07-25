#include "ads1292r.h"
#include "freertos/task.h"
#include <string.h>
#include "global_config.h"
static const char *TAG = "ADS1292R";

static void spi_readwrite(ADS1292R_t *dev, uint8_t *tx, uint8_t *rx, size_t len) {
    spi_transaction_t t = {
        .length = len * 8,
        .tx_buffer = tx,
        .rx_buffer = rx,
    };
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
    gpio_config_t io_conf = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << dev->cs_pin) | (1ULL << dev->start_pin) | (1ULL << dev->pwdn_pin),
    };
    gpio_config(&io_conf);
    gpio_set_direction(dev->drdy_pin, GPIO_MODE_INPUT);

    // 初始化SPI
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 500 * 1000,
        .mode = 1,
        .spics_io_num = -1,
        .queue_size = 3,
    };
    spi_bus_add_device(host, &devcfg, &dev->spi);

    ads1292r_reset(dev);

    #if (USE_EVT_ADS1192)
    ads1292r_write_regs(dev, 0x01, ads1192_default_config, 11);
    #endif

    #if (USE_ADS1292R)
    ads1292r_write_regs(dev, 0x01, ads1292r_default_config, 11);
    #endif
    ads1292r_read_regs(dev, 0x00, dev->reg, 12);

    // uint8_t id_val = 0;
    // ads1292r_read_regs(dev, 0x00, &id_val, 1);
    // ESP_LOGE(TAG, "DEVICE ID: 0x%02X", id_val);

    // if ((id_val & 0xF0) != 0x60) {
    //     ESP_LOGE(TAG, "Unexpected ADS1192 ID: 0x%02X", id_val);
    //     return ESP_FAIL;
    // }

    for (int i = 0; i < 12; ++i) {
        ESP_LOGI(TAG, "Reg[%02d] = 0x%02X", i, dev->reg[i]);
    }

    // ESP_LOGI(TAG,"READ SECOND TIME");
    // ads1292r_read_regs(dev, 0x00, dev->reg, 12);
    // for (int i = 0; i < 12; ++i) {
    //     if(dev->reg[i] == 0x51){
    //         ESP_LOGI(TAG,"THIS REG IS 0x51");
    //         ESP_LOGI(TAG,"THIS ADDR IS %d",i);
    //     }
    //     ESP_LOGI(TAG, "Reg[%02d] = 0x%02X", i, dev->reg[i]);
    // }

    return ESP_OK;
}

void ads1292r_start(ADS1292R_t *dev) {
    ads1292r_send_cmd(dev, ADS1292R_RDATAC);
    gpio_set_level(dev->start_pin, 1);
}

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

    #if (USE_EVT_ADS1192)
    uint8_t buf[7];
    uint8_t dummy[7] = {0};
    spi_readwrite(dev, dummy, buf, 7);

    int16_t raw_ch1 = (buf[3] << 8) | buf[4];
    int16_t raw_ch2 = (buf[5] << 8) | buf[6];
    #endif

    // #if (USE_ADS1292R)
    // // if (raw_ch1 & 0x800000) raw_ch1 |= 0xFF000000;
    // // if (raw_ch2 & 0x800000) raw_ch2 |= 0xFF000000;
    // #endif

    // #if (USE_EVT_ADS1192)
    // if (raw_ch1 & 0x8000) raw_ch1 |= 0xFFFF0000;
    // if (raw_ch2 & 0x8000) raw_ch2 |= 0xFFFF0000;
    // #endif

    ch_val[0] = ((float)raw_ch1 / 256.0f) * 0.288486f / 12;
    ch_val[1] = ((float)raw_ch2 / 256.0f) * 0.288486f / 12;

    return ESP_OK;
}
