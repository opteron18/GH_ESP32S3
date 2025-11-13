#include "lcd_init.h"
#include "esp_rom_sys.h"

static inline void delay_ms(uint32_t ms){ esp_rom_delay_us(ms*1000); }

void LCD_Address_Set(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye)
{
    LCD_WR_REG(0x2A);
    LCD_WR_DATA8(xs >> 8);
    LCD_WR_DATA8(xs & 0xFF);
    LCD_WR_DATA8(xe >> 8);
    LCD_WR_DATA8(xe & 0xFF);
    LCD_WR_REG(0x2B);
    LCD_WR_DATA8(ys >> 8);
    LCD_WR_DATA8(ys & 0xFF);
    LCD_WR_DATA8(ye >> 8);
    LCD_WR_DATA8(ye & 0xFF);
    LCD_WR_REG(0x2C);
    delay_ms(5);
    LCD_WR_REG(0x2C);
}

void LCD_Fill(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye, uint16_t color)
{
    LCD_Address_Set(xs, ys, xe - 1, ye - 1);
    for (uint16_t j = ys; j < ye; j++){
        for (uint16_t i = xs; i < xe; i++){
            LCD_WR_DATA(color);
        }
    }
}

void LCD_Init(void)
{
    LCD_GPIOInit();
    // Reset sequence
    // Using public helpers in spi_bitbang via macros
    // Toggle RES
    gpio_set_level(LCD_PIN_RES, 1);
    delay_ms(100);
    gpio_set_level(LCD_PIN_RES, 0);
    delay_ms(250);
    gpio_set_level(LCD_PIN_RES, 1);
    delay_ms(250);

    // Use shortened, essential init only to bring panel up (full table can be added if needed)
    LCD_WR_REG(0x11);
    delay_ms(120);
    LCD_WR_REG(0x3A);
    LCD_WR_DATA8(0x55);

    LCD_WR_REG(0x2A);
    LCD_WR_DATA8(0x00); LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x00); LCD_WR_DATA8(0x77);

    LCD_WR_REG(0x2B);
    LCD_WR_DATA8(0x00); LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x00); LCD_WR_DATA8(0xEF);

    LCD_WR_REG(0x36);
#if USE_HORIZONTAL==0
    LCD_WR_DATA8(0x00);
#else
    LCD_WR_DATA8(0xC0);
#endif

    delay_ms(25);
    LCD_WR_REG(0x29);
}


