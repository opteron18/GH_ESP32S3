#include "uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#include "esp_log.h"

// 接收回调示例
static void rx_callback(uart_packet_t *packet) {
    // 处理接收到的数据（示例）
    if(packet->len > 0) {
        char *str = malloc(packet->len + 1);
        if(str) {
            memcpy(str, packet->data, packet->len);
            str[packet->len] = '\0';
            ESP_LOGI("UART_RX", "Received: %s", str);
            free(str);
        }
    }
    
    // 释放内存
    free(packet->data);
    free(packet);
}

// 周期发送任务的函数
static void uart_periodic_task(void *arg) {
    const char *msg = "Hello UART!\n";
    size_t len = strlen(msg);
    
    while(1) {
        esp_err_t err = uart_send_data((const uint8_t *)msg, len, portMAX_DELAY);
        if(err != ESP_OK) {
            ESP_LOGE("UART_SEND", "Send failed: %d", err);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void create_uart_tasks(void) {
    // 使用新类型名
    static uart_driver_config_t uart_cfg = {
        .uart_num = UART_NUM_1,
        .tx_pin = 47,  // 自定义TX引脚
        .rx_pin = 48,  // 自定义RX引脚
        .baud_rate = 256000,
        .rx_queue = NULL, // 在驱动中创建
        .tx_queue = NULL  // 在驱动中创建
    };
    
    // 初始化UART驱动
    if(uart_init_driver(&uart_cfg) != ESP_OK) {
        ESP_LOGE("UART_INIT", "Failed to initialize UART");
        return;
    }
    
    // 注册接收回调
    // uart_register_rx_callback(rx_callback);
    
    // 创建传输和接收任务
    // xTaskCreate(uart_transmit_task, "uart_tx", 4096, NULL, 5, NULL);
    // xTaskCreate(uart_receive_task, "uart_rx", 4096, NULL, 5, NULL);
    
    // 创建周期发送任务
    // xTaskCreate(uart_periodic_task, "uart_periodic", 2048, NULL, 4, NULL);
}