#include "uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#include "esp_log.h"
#include "global_config.h"

// 接收回调示例
static void rx_callback(uart_packet_t *packet) {
    // 处理接收到的数据（示例）
    if(packet->len > 0) {
        ESP_LOGI("UART_RX", "Received empty packet");
        // goto cleanup;
        // char *str = malloc(packet->len + 1);
        // if(str) {
        //     memcpy(str, packet->data, packet->len);
        //     str[packet->len] = '\0';
        //     ESP_LOGI("UART_RX", "Received: %s", str);
        //     free(str);
        // }
    }
    ESP_LOGI("UART_RX", "Received %d bytes of binary data:", packet->len);
    ESP_LOG_BUFFER_HEXDUMP("UART_RX", packet->data, packet->len, ESP_LOG_INFO);
    // cleanup:
    // 释放内存（安全处理空指针）
    if(packet->data) free(packet->data);
    free(packet);
    // 释放内存
    // free(packet->data);
    // free(packet);
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
    // static uart_driver_config_t uart_cfg = {
    //     .uart_num = UART_NUM_1,
    //     .tx_pin = 47,  // 自定义TX引脚
    //     .rx_pin = 48,  // 自定义RX引脚
    //     .baud_rate = 256000,
    //     .rx_queue = NULL, // 在驱动中创建
    //     .tx_queue = NULL  // 在驱动中创建
    // };

    static uart_driver_config_t uart_cfg = {

        #if (USE_UART0)
        .uart_num = UART_NUM_0,
        .tx_pin = 43,  // 自定义TX引脚
        .rx_pin = 44,  // 自定义RX引脚
        .baud_rate = 1152000,
        .rx_queue = NULL, // 在驱动中创建
        .tx_queue = NULL  // 在驱动中创建
        #endif
        // .uart_num = UART_NUM_2,
        // .tx_pin = 5,  // 自定义TX引脚
        // .rx_pin = 4,  // 自定义RX引脚
        // .baud_rate = 9600,
        // .rx_queue = NULL, // 在驱动中创建
        // .tx_queue = NULL  // 在驱动中创建
        #if (USE_UART1)
        .uart_num = UART_NUM_1,
        .tx_pin = 47,  // 自定义TX引脚
        .rx_pin = 48,  // 自定义RX引脚
        .baud_rate = 1152000,
        .rx_queue = NULL, // 在驱动中创建
        .tx_queue = NULL  // 在驱动中创建
        #endif

        #if (USE_UART2)
        .uart_num = UART_NUM_2,
        .tx_pin = 4,  // 自定义TX引脚
        .rx_pin = 5,  // 自定义RX引脚
        .baud_rate = 1152000,
        .rx_queue = NULL, // 在驱动中创建
        .tx_queue = NULL  // 在驱动中创建
        #endif

    };
    
    // uart_config_t uart_config = {
    //     .baud_rate = 115200,                       // 设置波特率
    //     .data_bits = UART_DATA_8_BITS,             // 设置数据位数
    //     .parity = UART_PARITY_DISABLE,             // 设置奇偶校验
    //     .stop_bits = UART_STOP_BITS_1,             // 设置停止位
    //     .flow_ctrl = UART_HW_FLOWCTRL_DISABLE      // 硬件流控制
    // };
    
    // uart_param_config(UART_NUM_1, &uart_config);

    // int tx_pin = 47;      
    // int rx_pin = 48;

    // uart_set_pin(UART_NUM_1, tx_pin, rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    // uart_set_mode(UART_NUM_1, UART_MODE_UART);

    // uart_driver_install(UART_NUM_1, 200, 200, 0, NULL, 0);
    // 初始化UART驱动
    if(uart_init_driver(&uart_cfg) != ESP_OK) {
        ESP_LOGE("UART_INIT", "Failed to initialize UART");
        return;
    }
    
    // 注册接收回调
    // uart_register_rx_callback(rx_callback);
    
    // 创建传输和接收任务
    xTaskCreate(uart_transmit_task, "uart_tx", 4096, NULL, 5, NULL);
    // xTaskCreate(uart_receive_task, "uart_rx", 8192, NULL, 5, NULL);
    
    // 创建周期发送任务
    // xTaskCreate(uart_periodic_task, "uart_periodic", 2048, NULL, 4, NULL);
}