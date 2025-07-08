#include "ads1x9x.h"
#include "spi.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TAG "ADS1192_DRIVER"

#define ADS1x9x_PIN_NUM_MISO 12  // Changed from 6
#define ADS1x9x_PIN_NUM_MOSI 13  // Changed from 7
#define ADS1x9x_PIN_NUM_CLK  14  // Changed from 8
#define ADS1x9x_PIN_NUM_CS   15  // Changed from 9
#define ADS1x9x_PIN_NUM_RSN  18  // Keep as is

    // #define GH3X_INT_GPIO      4
    // #define GH3X_INT_NAME      "GH3X_INT"

    static spi_device_handle_t ads1x9x_spi;

    static void delay_ms(uint32_t ms) {
        vTaskDelay(pdMS_TO_TICKS(ms));
    }

    esp_err_t ads1x9x_spi_driver_init(void)
    {
        spi_bus_config_t ads1x9x_spi_cfg = {
            .miso_io_num = ADS1x9x_PIN_NUM_MISO,
            .mosi_io_num = ADS1x9x_PIN_NUM_MOSI,
            .sclk_io_num = ADS1x9x_PIN_NUM_CLK,
            .quadwp_io_num = -1,
            .quadhd_io_num = -1,
            .max_transfer_sz = 64
        };

        spi_device_interface_config_t ads1x9x_spi_interface_cfg = {
            .clock_speed_hz = 8 * 1000 * 1000, // 1 MHz
            // .duty_cycle_pos = 64,     //Duty cycle of positive clock, in 1/256th increments (128 = 50%/50% duty). Setting this to 0 (=not setting it) is equivalent to setting this to 128.
            .mode = 0,                          // CPOL = 0, CPHA = 1
            .spics_io_num = ADS1x9x_PIN_NUM_CS,
            .queue_size = 3,
            // .spics_io_num = -1,
        };
        
        esp_err_t ret;

        ret = spi_bus_initialize(SPI3_HOST, &ads1x9x_spi_cfg, SPI_DMA_CH_AUTO);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "ads1x9x SPI bus init failed");
            return ret;
        }

        ret = spi_bus_add_device(SPI3_HOST, &ads1x9x_spi_interface_cfg, &ads1x9x_spi);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "ads1x9x SPI device add failed");
            return ret;
        }

        // gpio_config_t io_conf = {
        // .pin_bit_mask = (1ULL << PIN_NUM_CS),
        // .mode = GPIO_MODE_OUTPUT,
        // .pull_up_en = GPIO_PULLUP_DISABLE,
        // .pull_down_en = GPIO_PULLDOWN_DISABLE,
        // .intr_type = GPIO_INTR_DISABLE
        // };
        // gpio_config(&io_conf);
        // gpio_set_level(PIN_NUM_CS, 1); // CS 初始为高电平

        ESP_LOGI(TAG, "ads1x9x_SPI bus/device init complete");
        return ESP_OK;
    }

// === GPIO + SPI Resource Setup ===
esp_err_t ads1192_init(void) {
    ads1x9x_spi_driver_init();

    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << ADS_RESET_GPIO) |
                        (1ULL << ADS_START_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);

    gpio_set_level(ADS_RESET_GPIO, 1);
    gpio_set_level(ADS_START_GPIO, 0);

    // ads1192_reset();
    // delay_ms(10);
    // ads1192_stop();
    // delay_ms(10);
    // ads1192_start();

    return ESP_OK;
}

esp_err_t ads1192_reset(void) {
    gpio_set_level(ADS_RESET_GPIO, 1);
    delay_ms(1);
    gpio_set_level(ADS_RESET_GPIO, 0);
    delay_ms(1);
    gpio_set_level(ADS_RESET_GPIO, 1);
    delay_ms(10);
    return ESP_OK;
}

esp_err_t ads1192_soft_reset(void) {
    uint8_t cmd = CMD_RESET;
    return spi_send_command(&cmd, 1);
}

esp_err_t ads1192_start(void) {
    gpio_set_level(ADS_START_GPIO, 1);
    return ESP_OK;
}

esp_err_t ads1192_stop(void) {
    gpio_set_level(ADS_START_GPIO, 0);
    return ESP_OK;
}

// === Command Send ===
static esp_err_t ads1192_send_command(uint8_t cmd) {
    return spi_send_command(&cmd, 1);
}

// === Register Read/Write ===
esp_err_t ads1192_write_register(uint8_t reg_addr, uint8_t value) {
    uint8_t tx[3] = { CMD_WREG | reg_addr, 0x00, value };
    return spi_send_command(tx, 3);
}

esp_err_t ads1192_read_register(uint8_t reg_addr, uint8_t *value) {
    uint8_t tx[3] = { CMD_RREG | reg_addr, 0x00, 0x00 };
    uint8_t rx[3] = {0};
    esp_err_t ret = spi_send_receive(tx, rx, 3);
    if (ret == ESP_OK) *value = rx[2];
    return ret;
}

esp_err_t ads1192_read_all_registers(uint8_t *reg_buf) {
    for (uint8_t i = 0; i <= REG_GPIO; i++) {
        if (ads1192_read_register(i, &reg_buf[i]) != ESP_OK)
            return ESP_FAIL;
    }
    return ESP_OK;
}

// === Continuous Read Mode ===
esp_err_t ads1192_start_continuous_read(void) {
    uint8_t cmd = CMD_RDATAC;
    return spi_send_command(&cmd, 1);
}

esp_err_t ads1192_stop_continuous_read(void) {
    uint8_t cmd = CMD_SDATAC;
    return spi_send_command(&cmd, 1);
}

esp_err_t ads1192_read_data(uint8_t *data_buf, size_t length) {
    uint8_t cmd = CMD_RDATA;
    spi_send_command(&cmd, 1);
    return spi_send_receive(NULL, data_buf, length);
}

// === Additional Control Commands ===
esp_err_t ads1192_wakeup(void) {
    uint8_t cmd = CMD_WAKEUP;
    return spi_send_command(&cmd, 1);
}

esp_err_t ads1192_standby(void) {
    uint8_t cmd = CMD_STANDBY;
    return spi_send_command(&cmd, 1);
}

esp_err_t ads1192_start_conversion(void) {
    uint8_t cmd = CMD_START;
    return spi_send_command(&cmd, 1);
}

esp_err_t ads1192_stop_conversion(void) {
    uint8_t cmd = CMD_STOP;
    return spi_send_command(&cmd, 1);
}
