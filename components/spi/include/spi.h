#ifndef SPI_DRIVER_H
#define SPI_DRIVER_H

#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_err.h"

esp_err_t spi_driver_init(void);
esp_err_t spi_send_command(uint8_t *tx_data, size_t length);
esp_err_t spi_send_receive(uint8_t *tx_data, uint8_t *rx_data, size_t length);

#endif // SPI_DRIVER_H
