#include "gh3x_demo.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void User_GH3300_init(void){
    GH3X_DemoInit();
}
void User_GH3300_StartSamping(int GH3300_FUNCTION){
    GH3X_DemoStartSampling(GH3300_FUNCTION);
}

void FRT_GH3300_Task(){

    while(1){
        GH3X_DemoStartSampling(GH3X_FUNCTION_HR);
        GH3X_DemoInterruptProcess();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}