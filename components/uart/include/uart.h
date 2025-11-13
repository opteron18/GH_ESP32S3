#ifndef UART_DRIVER_H
#define UART_DRIVER_H

#include <stddef.h>
#include <stdint.h>
#include "driver/uart.h"
#include "freertos/queue.h"
#include "freertos/task.h"

// 消息类型
typedef enum {
    UART_RAW_DATA,
    UART_CMD_FLUSH
} uart_msg_type_t;

// 数据包结构
typedef struct {
    uart_msg_type_t type;
    size_t len;
    uint8_t *data;
} uart_packet_t;

// 配置结构体
typedef struct {
    int tx_pin;
    int rx_pin;
    int baud_rate;
    uart_port_t uart_num;
    QueueHandle_t rx_queue;   // 接收数据队列
    QueueHandle_t tx_queue;   // 发送数据队列
    QueueHandle_t event_queue; // 事件队列（由驱动创建）
} uart_driver_config_t;

// 初始化UART驱动
esp_err_t uart_init_driver(uart_driver_config_t *config);

// 注册接收回调
typedef void (*uart_rx_cb_t)(uart_packet_t *packet);
void uart_register_rx_callback(uart_rx_cb_t callback);

// 发送数据（队列方式）
esp_err_t uart_send_data(const uint8_t *data, size_t len, TickType_t timeout);

// 队列发送接口（供其他任务调用）
esp_err_t uart_send_data_queue(const uint8_t *data, size_t len, TickType_t timeout);

// 任务函数
void uart_transmit_task(void *arg);
void uart_receive_task(void *arg);

#endif // UART_DRIVER_H