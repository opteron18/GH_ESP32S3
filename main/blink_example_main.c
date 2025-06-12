#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "led.h"
#include "uart.h"
#include "led_strip.h"
#include "gh3x_demo.h"
#include "freertos/Semphr.h"

extern esp_err_t gh_ESP_LOG(char *string);
extern esp_err_t gh_spi_send(uint8_t *tx_data, size_t length);
extern esp_err_t spi_driver_init(void);
void app_main(void)
{

    // GhCommInit();
    configure_led();
    uart_init();
    gh_ESP_LOG("Hello world!\n");
    // spi_driver_init();
    // const char *message = "helloworld";
    // gh_spi_send((uint8_t *)message, strlen(message));  // 转换为uint8_t*并传入字符串长度
    // const char *message2 = "helloworldhelloworldhelloworldhelloworldhelloworld";
    // gh_spi_send((uint8_t *)message2, strlen(message2));  // 转换为uint8_t*并传入字符串长度

    GH3X_DemoInit();
    GH3X_DemoStartSampling(GH3X_FUNCTION_HR);
    
    // spi_driver_init();


    // gh3x_bsp_delay_us(1000);
    // xTaskCreate(rx_task, "uart_rx_task", 1024 * 2, NULL, configMAX_PRIORITIES - 1, NULL);
    // xTaskCreate(tx_task, "uart_tx_task", 1024 * 2, NULL, configMAX_PRIORITIES - 2, NULL);

    // xTaskCreate(blink_task, "blink_task", 1024 * 2, NULL, configMAX_PRIORITIES - 3, NULL);

    // ws2812();

}

