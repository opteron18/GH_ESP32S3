#include "gh3x_demo.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"
#include "esp_log.h"
#include  "heart_net_org_export.h"

void User_GH3300_init(void){
    GH3X_DemoInit();
}
void User_GH3300_StartSamping(int GH3300_FUNCTION){
    GH3X_DemoStartSampling(GH3300_FUNCTION);
}

void FRT_GH3300_Task(){
    vTaskDelay(pdMS_TO_TICKS(10));
    User_GH3300_init();
    User_GH3300_StartSamping(GH3X_FUNCTION_HRV);
    // GH3X_DemoStopSampling(GH3X_FUNCTION_HRV);
    while(1){
        // esp_task_wdt_reset();
        GH3X_DemoInterruptProcess();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
void FRT_GH3300_TaskCreate(){
    xTaskCreate(
            FRT_GH3300_Task,                   // 任务函数
            "FRT_GH3300_Task",                 // 任务名称
            8192,                       // 堆栈大小（字节）
            NULL,                       // 参数传递
            configMAX_PRIORITIES - 1,   // 优先级（高）
            NULL                        // 任务句柄
        );
}