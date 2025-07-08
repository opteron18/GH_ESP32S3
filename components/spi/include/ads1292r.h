#ifndef _ADS1292R_H_
#define _ADS1292R_H_

#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define ADS1292R_REG_NUM 12

// ADS1292R命令定义
#define ADS1292R_WAKEUP     0x02
#define ADS1292R_STANDBY    0x04
#define ADS1292R_ADSRESET   0x06
#define ADS1292R_START      0x08
#define ADS1292R_STOP       0x0A
#define ADS1292R_OFFSETCAL  0x1A
#define ADS1292R_RDATAC     0x10
#define ADS1292R_SDATAC     0x11
#define ADS1292R_RDATA      0x12
#define ADS1292R_RREG       0x20
#define ADS1292R_WREG       0x40

typedef struct {
    spi_device_handle_t spi;
    gpio_num_t cs_pin;
    gpio_num_t pwdn_pin;
    gpio_num_t start_pin;
    gpio_num_t drdy_pin;
    uint8_t reg[ADS1292R_REG_NUM];
} ADS1292R_t;

esp_err_t ads1292r_init(ADS1292R_t *dev, spi_host_device_t host);
void ads1292r_reset(ADS1292R_t *dev);
void ads1292r_send_cmd(ADS1292R_t *dev, uint8_t cmd);
void ads1292r_write_regs(ADS1292R_t *dev, uint8_t addr, uint8_t *data, uint8_t len);
void ads1292r_read_regs(ADS1292R_t *dev, uint8_t addr, uint8_t *data, uint8_t len);
void ads1292r_start(ADS1292R_t *dev);
void ads1292r_stop(ADS1292R_t *dev);
void ads1292r_read_data(ADS1292R_t *dev, float *ch_val);

#endif
