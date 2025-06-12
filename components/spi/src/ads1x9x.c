
#include "ads1x9x.h"
#include "spi.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TAG "ADS1192_DRIVER"

static void delay_ms(uint32_t ms) {
    vTaskDelay(pdMS_TO_TICKS(ms));
}

// === GPIO + SPI Resource Setup ===
esp_err_t ads1192_init(void) {
    spi_driver_init();

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

    ads1192_reset();
    delay_ms(10);
    ads1192_stop();
    delay_ms(10);
    ads1192_start();

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
