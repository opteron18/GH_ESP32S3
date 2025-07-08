#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "led.h"
#include "uart.h"
#include "led_strip.h"
#include "gh3x_demo.h"
#include "freertos/semphr.h"

#include "pwm.h"
#include "gh3x_demo.h"
#include "ads1x9x.h"

extern esp_err_t gh_ESP_LOG(char *string);
extern esp_err_t gh_spi_send(uint8_t *tx_data, size_t length);
extern esp_err_t spi_driver_init(void);
extern void User_GH3300_init(void);
extern void FRT_GH3300_TaskCreate();
extern void FRT_LED_TaskCreate(void);
extern void create_uart_tasks(void);
extern void FRT_adc_buttom_TaskCreate(void);
extern void create_EMG_tasks(void);
void app_main(void)
{
    create_uart_tasks();
    GhCommInit();
    // configure_led();
    // uart_init();
    // while(1){
    //     gh_ESP_LOG("System initialization complete\n");
    //     vTaskDelay(1);
    // }
    vTaskDelay(pdMS_TO_TICKS(10));
    gh_ESP_LOG("System initialization complete\n");
    // const char * name = "hellothere";
    // sendData(name,name);
    // ads1192_init();
    // for(;;){
    //     vTaskDelay(pdMS_TO_TICKS(10));
    // }

    // spi_driver_init();
    // const char *message = "helloworld";
    // gh_spi_send((uint8_t *)message, strlen(message));  // 转换为uint8_t*并传入字符串长度
    // const char *message2 = "helloworldhelloworldhelloworldhelloworldhelloworld";
    // gh_spi_send((uint8_t *)message2, strlen(message2));  // 转换为uint8_t*并传入字符串长度
    create_EMG_tasks();
    // FRT_GH3300_TaskCreate();
    // FRT_LED_TaskCreate();


    // FRT_adc_buttom_TaskCreate();


    // create_uart_tasks();
    // GH3X_DemoStartSampling(GH3X_FUNCTION_HR);
    // while(1){
    //     GH3X_DemoStartSampling(GH3X_FUNCTION_HR);
    //     GH3X_DemoInterruptProcess();
    //     GH3X_DelayUs(1000);
    // }
    //  pwm_halfbridge_init(10000);
    //  pwm_halfbridge_start();
    //  pwm_halfbridge_stop();
    //  pwm_single_init(10000, 0.5f, GPIO_NUM_5);
    //  pwm_single_start();
    //  pwm_single_stop();

    // pwm_halfbridge_init(10000);
    // pwm_single_output(10000, 0.5f, GPIO_NUM_5);
    // pump_pwm_init(0);  // 初始化为 50% 占空比
    // pump_pwm_set_duty(PWM_DUTY_MAX / 2);  // 初始化为 50% 占空比
    // while(1){
    // pump_pwm_set_duty(PWM_DUTY_MAX / 2);  // 初始化为 50% 占空比
    // vTaskDelay(pdMS_TO_TICKS(1000));
    // pump_pwm_set_duty(PWM_DUTY_MAX / 2);
    // vTaskDelay(pdMS_TO_TICKS(1000));
    // }
    // spi_driver_init();


    // gh3x_bsp_delay_us(1000);
    // xTaskCreate(rx_task, "uart_rx_task", 1024 * 2, NULL, configMAX_PRIORITIES - 1, NULL);
    // xTaskCreate(tx_task, "uart_tx_task", 1024 * 2, NULL, configMAX_PRIORITIES - 2, NULL);

    // xTaskCreate(blink_task, "blink_task", 1024 * 2, NULL, configMAX_PRIORITIES - 3, NULL);

    // ws2812();

}

