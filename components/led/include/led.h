#ifndef _LED_H_
#define _LED_H_

#include "esp_system.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"

#define BLINK_GPIO 48
static const char *LED_Thread = "LED_Thread";
void blink_led(void);
void configure_led(void);
void blink_task(void *arg);
void ws2812(void);
#endif 