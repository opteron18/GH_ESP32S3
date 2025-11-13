#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "esp_log.h"

uint32_t a;
uint32_t b;
void FRT_LED_Task(){
    vTaskDelay(pdMS_TO_TICKS(10));
    configure_led();
    const TickType_t xfrequency = pdMS_TO_TICKS(1000);
    TickType_t xlastwaketime = xTaskGetTickCount();
    while(1){
        
        gpio_set_level(BLINK_GPIO, 1);
        vTaskDelayUntil(&xlastwaketime,xfrequency);
        // ESP_LOGI(LED_Thread, "LED ON at tick: %lu", (unsigned long)xlastwaketime);
        /* Toggle the LED state */
        // vTaskDelay(1000 / portTICK_PERIOD_MS);
        gpio_set_level(BLINK_GPIO, 0);
        vTaskDelayUntil(&xlastwaketime,xfrequency);
        // ESP_LOGI(LED_Thread, "LED OFF at tick: %lu", (unsigned long)xlastwaketime);
        // vTaskDelay(1000 / portTICK_PERIOD_MS);

    }
}

void FRT_LED_TaskCreate(){
    xTaskCreate(
            FRT_LED_Task,                   // 任务函数
            "FRT_GH3300_Task",                 // 任务名称
            1024,                       // 堆栈大小（字节）
            NULL,                       // 参数传递
            configMAX_PRIORITIES - 1,   // 优先级（高）
            NULL                        // 任务句柄
        );
}