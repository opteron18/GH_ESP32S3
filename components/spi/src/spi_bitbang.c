#include "spi_bitbang.h"
#include "esp_rom_sys.h"

static inline void sck_low(void){ gpio_set_level(LCD_PIN_SCK, 0); }
static inline void sck_high(void){ gpio_set_level(LCD_PIN_SCK, 1); }
static inline void mosi_low(void){ gpio_set_level(LCD_PIN_MOSI, 0); }
static inline void mosi_high(void){ gpio_set_level(LCD_PIN_MOSI, 1); }
static inline void res_low(void){ gpio_set_level(LCD_PIN_RES, 0); }
static inline void res_high(void){ gpio_set_level(LCD_PIN_RES, 1); }
static inline void dc_low(void){ gpio_set_level(LCD_PIN_DC, 0); }
static inline void dc_high(void){ gpio_set_level(LCD_PIN_DC, 1); }
static inline void cs_low(void){ gpio_set_level(LCD_PIN_CS, 0); }
static inline void cs_high(void){ gpio_set_level(LCD_PIN_CS, 1); }

void LCD_GPIOInit(void)
{
    const gpio_config_t cfg = {
        .pin_bit_mask = (1ULL<<LCD_PIN_SCK) | (1ULL<<LCD_PIN_MOSI) | (1ULL<<LCD_PIN_RES) | (1ULL<<LCD_PIN_DC) | (1ULL<<LCD_PIN_CS),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config((gpio_config_t*)&cfg);
    sck_low(); mosi_low(); dc_high(); cs_high(); res_high();
}

void LCD_WR_Bus(uint8_t dat)
{
    cs_low();
    for (int i = 0; i < 8; i++) {
        sck_low();
        if (dat & 0x80) mosi_high(); else mosi_low();
        esp_rom_delay_us(1);
        sck_high();
        esp_rom_delay_us(1);
        dat <<= 1;
    }
    cs_high();
}

void LCD_WR_REG(uint8_t reg)
{
    dc_low();
    LCD_WR_Bus(reg);
    dc_high();
}

void LCD_WR_DATA8(uint8_t dat)
{
    dc_high();
    LCD_WR_Bus(dat);
    dc_high();
}

void LCD_WR_DATA(uint16_t dat)
{
    dc_high();
    LCD_WR_Bus((uint8_t)(dat >> 8));
    LCD_WR_Bus((uint8_t)(dat & 0xFF));
    dc_high();
}


