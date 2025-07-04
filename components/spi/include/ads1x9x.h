#ifndef ADS1192_H
#define ADS1192_H

#include "esp_err.h"
#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

// ========= 初始化 & 控制 =========
esp_err_t ads1192_init(void);
esp_err_t ads1192_start(void);
esp_err_t ads1192_stop(void);
esp_err_t ads1192_reset(void);
esp_err_t ads1192_soft_reset(void);
esp_err_t ads1192_read_register(uint8_t reg_addr, uint8_t *value);
esp_err_t ads1192_write_register(uint8_t reg_addr, uint8_t value);
esp_err_t ads1192_read_all_registers(uint8_t *reg_buf);
esp_err_t ads1192_start_continuous_read(void);
esp_err_t ads1192_stop_continuous_read(void);
esp_err_t ads1192_read_data(uint8_t *data_buf, size_t length);

// ========= 命令码 =========
#define CMD_WAKEUP    0x02
#define CMD_STANDBY   0x04
#define CMD_RESET     0x06
#define CMD_START     0x08
#define CMD_STOP      0x0A
#define CMD_RDATAC    0x10
#define CMD_SDATAC    0x11
#define CMD_RDATA     0x12
#define CMD_RREG      0x20
#define CMD_WREG      0x40

// ========= 寄存器地址 + 注释 =========
#define REG_ID          0x00
/*
 * ID Register (Read-Only)
 * Bits[1:0]: Device Type
 *   00 = ADS1191
 *   01 = ADS1192
 *   10 = ADS1291
 *   11 = ADS1292
 * Bits[7:5]: Revision ID (Factory Programmed)
 */

#define REG_CONFIG1     0x01
/*
 * CONFIG1 Register
 * ------------------------------------------------------------------
 * | BIT 7       | BIT 6-3 Reserved (0) | BIT 2 | BIT 1 | BIT 0     |
 * |-------------|----------------------|--------|--------|----------|
 * | SINGLE_SHOT |         0s           |  OSR2  |  OSR1  |  OSR0    |
 * ------------------------------------------------------------------
 * SINGLE_SHOT: 0 = Continuous conversion mode (default), 1 = Single-shot
 * OSR: Oversampling ratio selection
 *   000 = fMOD / 1024 = 125 SPS
 *   001 = fMOD / 512  = 250 SPS
 *   010 = fMOD / 256  = 500 SPS
 *   011 = fMOD / 128  = 1000 SPS
 *   100 = fMOD / 64   = 2000 SPS
 *   101-111 = Reserved (do not use)
 */

#define REG_CONFIG2     0x02
/*
 * CONFIG2 Register
 * -----------------------------------------------------------------------------------
 * | BIT 7 | BIT 6 | BIT 5 | BIT 4 | BIT 3 | BIT 2 | BIT 1     | BIT 0               |
 * |-------|--------|--------|--------|--------|--------|------------|----------------|
 * |   1   | PDB_LOFF_COMP | PDB_REFBUF | VREF_4V | CLK_EN | 0 | INT_TEST | TEST_FREQ |
 * -----------------------------------------------------------------------------------
 * PDB_LOFF_COMP: 1 = Enable lead-off comparator
 * PDB_REFBUF:    1 = Enable internal reference buffer
 * VREF_4V:       1 = Use 4.0V reference; 0 = Use 2.4V
 * CLK_EN:        1 = Output system clock to CLK pin
 * INT_TEST:      1 = Enable internal test signal
 * TEST_FREQ:     1 = Test signal is 1Hz square wave; 0 = DC
 */

#define REG_LOFF        0x03
/*
 * LOFF Register (Lead-Off Control)
 * ------------------------------------------------------------------------------------------------
 * | BIT 7-5: COMP_TH[2:0] | BIT 4 = 1 | ILEAD_OFF[1:0] | 0 | FLEAD_OFF |
 * ------------------------------------------------------------------------------------------------
 * COMP_TH: Comparator threshold (% of full scale)
 *   000 = 95% / 5%, 111 = 70% / 30%
 * ILEAD_OFF: Lead-off current
 *   00 = 6 nA, 01 = 24 nA, 10 = 6 uA, 11 = 24 uA
 * FLEAD_OFF: 1 = AC mode; 0 = DC mode
 */

#define REG_CH1SET      0x04
#define REG_CH2SET      0x05
/*
 * CHnSET Registers (Channel Settings)
 * --------------------------------------------------------------------------------
 * | BIT 7 | BIT 6-4: GAIN[2:0] | 0 | MUX[3:0]                                      |
 * --------------------------------------------------------------------------------
 * PDn: 1 = Power down channel
 * GAIN: 000 = 6 (default), 001 = 1, 010 = 2, ..., 110 = 12
 * MUX: 0000 = Normal electrode input
 *      0001 = Input shorted
 *      0010 = RLD_MEASURE
 *      0011 = VDD/2
 *      0100 = Temperature sensor
 *      0101 = Cal signal
 *      0110 = RLD_DRP
 *      0111 = RLD_DRM
 */

#define REG_RLD_SENS    0x06
/*
 * RLD_SENS: Right Leg Drive Sense
 * Enable channel inputs to contribute to RLD
 * Bits include CHOP, PD_RLD, and RLDP/nX for each channel
 */

#define REG_LOFF_SENS   0x07
/*
 * LOFF_SENS: Lead-Off Sense Select
 * Enable IN1P/N, IN2P/N for lead-off detection
 * Bits 7-6 reserved (0), bit 5-0 for FLIP and LOFFx settings
 */

#define REG_LOFF_STAT   0x08
/*
 * LOFF_STAT: Lead-Off Status
 * Read-only indication of lead-off status per channel and RLD
 * '1' means disconnected; '0' connected
 */

#define REG_RESP1       0x09
#define REG_RESP2       0x0A
/*
 * RESP Registers
 * Enable/disable respiration detection and demodulation
 * Phase, amplitude, frequency, calibration on/off, etc.
 */

#define REG_GPIO        0x0C
/*
 * GPIO: General Purpose Input/Output Register
 * Bits 3-2: GPIO Control (0 = output, 1 = input)
 * Bits 1-0: GPIO Data
 */

// ========= 设备类型 ID =========
#define DEV_ID_ADS1191  0x00
#define DEV_ID_ADS1192  0x01
#define DEV_ID_ADS1291  0x02
#define DEV_ID_ADS1292  0x03

// ========= 默认引脚定义 =========
#define ADS1x9x_PIN_NUM_MISO 16  // Changed from 6
#define ADS1x9x_PIN_NUM_MOSI 18  // Changed from 7
#define ADS1x9x_PIN_NUM_CLK  17  // Changed from 8
#define ADS1x9x_PIN_NUM_CS   12  // Changed from 9

#define ADS_RESET_GPIO  14
#define ADS_START_GPIO  13
#define ADS_DRDY_GPIO   15

esp_err_t ads1192_init(void);

#ifdef __cplusplus
}
#endif

#endif // ADS1192_H
