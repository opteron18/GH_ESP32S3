#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"

void FRT_LED_Task(){
    vTaskDelay(pdMS_TO_TICKS(10));
    configure_led();
    while(1){
        // ESP_LOGI(TAG, "Turning the LED %s!", s_led_state == true ? "ON" : "OFF");
        gpio_set_level(BLINK_GPIO, 1);
        /* Toggle the LED state */
        vTaskDelay(100 / portTICK_PERIOD_MS);
        gpio_set_level(BLINK_GPIO, 0);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void FRT_LED_TaskCreate(){
    xTaskCreate(
            FRT_LED_Task,                   // 任务函数
            "FRT_GH3300_Task",                 // 任务名称
            4096,                       // 堆栈大小（字节）
            NULL,                       // 参数传递
            configMAX_PRIORITIES - 1,   // 优先级（高）
            NULL                        // 任务句柄
        );
}