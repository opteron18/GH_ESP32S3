#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "led.h"
#include "uart.h"
// #include "led_strip.h"
#include "gh3x_demo.h"
#include "freertos/semphr.h"

#include "pwm.h"
#include "gh3x_demo.h"
#include "ads1x9x.h"
#include "lcd_init.h"
#include "esp_blufi_api.h"
#include "blufi_example.h"

// #include "lvgl.h"
// #include "lv_port_disp.h"
// #include "lv_tick.h"
// #include "lvgl/src/core/lv_obj.h"

extern esp_err_t gh_ESP_LOG(char *string);
extern esp_err_t gh_spi_send(uint8_t *tx_data, size_t length);
extern esp_err_t spi_driver_init(void);
extern void User_GH3300_init(void);
extern void FRT_GH3300_TaskCreate();
extern void FRT_LED_TaskCreate(void);
extern void create_uart_tasks(void);
extern void FRT_adc_buttom_TaskCreate(void);
extern void create_EMG_tasks(void);
extern void FRT_UI_TaskCreate(void);
extern void sdcard_test(void);
extern void https_ota(void);
extern void ble_spp_server_task(void);
// extern void lv_port_disp_init(void);
// extern void lv_init(void);

void app_main(void)
{
    //启动延迟
    vTaskDelay(pdMS_TO_TICKS(20));

    // lv_init();          
    // lv_port_disp_init();

    // blufi_start();

    ble_spp_server_task();

    


    // https_ota();

    // create_uart_tasks();//串口初始化线程

    // FRT_adc_buttom_TaskCreate();//ADC复用按钮线程

    // create_EMG_tasks();//1292R EMG肌电线程

    // sdcard_test();//sd卡线程

    // FRT_GH3300_TaskCreate();//GH3300线程

    // FRT_LED_TaskCreate();//单脚LED线程非2812
}

