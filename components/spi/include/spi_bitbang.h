#pragma once
#include <stdint.h>
#include "driver/gpio.h"

#ifndef LCD_PIN_SCK
#define LCD_PIN_SCK  GPIO_NUM_9
#endif
#ifndef LCD_PIN_MOSI
#define LCD_PIN_MOSI GPIO_NUM_8
#endif
#ifndef LCD_PIN_RES
#define LCD_PIN_RES  GPIO_NUM_10
#endif
#ifndef LCD_PIN_DC
#define LCD_PIN_DC   GPIO_NUM_6
#endif
#ifndef LCD_PIN_CS
#define LCD_PIN_CS   GPIO_NUM_7
#endif

void LCD_GPIOInit(void);
void LCD_WR_Bus(uint8_t dat);
void LCD_WR_REG(uint8_t reg);
void LCD_WR_DATA8(uint8_t dat);
void LCD_WR_DATA(uint16_t dat);


