#include "uart.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"

static const char *TAG = "UART_DRIVER";
static uart_driver_config_t *uart_cfg = NULL;
static uart_rx_cb_t rx_callback = NULL;

// UART事件处理任务
static void uart_event_task(void *arg)
{
    uart_event_t event;
    
    while (1) {
        // 等待UART事件
        if (xQueueReceive(uart_cfg->event_queue, &event, portMAX_DELAY)) {
            switch (event.type) {
                case UART_DATA:
                {
                    size_t len = 0;
                    uart_get_buffered_data_len(uart_cfg->uart_num, &len);
                    
                    if (len > 0) {
                        uint8_t *data = malloc(len);
                        if (data) {
                            int read_len = uart_read_bytes(uart_cfg->uart_num, data, len, 0);
                            if (read_len > 0) {
                                // 创建数据包
                                uart_packet_t *packet = malloc(sizeof(uart_packet_t));
                                if (packet) {
                                    packet->type = UART_RAW_DATA;
                                    packet->len = read_len;
                                    packet->data = data;
                                    
                                    // 推送到接收队列
                                    xQueueSend(uart_cfg->rx_queue, &packet, 0);
                                } else {
                                    free(data);
                                }
                            } else {
                                free(data);
                            }
                        }
                    }
                    break;
                }
                
                case UART_FIFO_OVF:
                case UART_BUFFER_FULL:
                    // 缓冲区溢出，需要刷新
                    uart_flush_input(uart_cfg->uart_num);
                    break;
                
                default:
                    break;
            }
        }
    }
}

// 初始化UART驱动
esp_err_t uart_init_driver(uart_driver_config_t *config) {
    if(uart_cfg != NULL) {
        ESP_LOGE(TAG, "UART driver already initialized");
        return ESP_FAIL;
    }
    
    uart_cfg = config;
    
    // 创建队列（如果未提供）
    if(config->rx_queue == NULL) {
        config->rx_queue = xQueueCreate(10, sizeof(uart_packet_t *));
    }
    if(config->tx_queue == NULL) {
        config->tx_queue = xQueueCreate(10, sizeof(uart_packet_t *));
    }
    // 创建事件队列
    config->event_queue = xQueueCreate(10, sizeof(uart_event_t));
    
    // IDF标准的UART配置
    uart_config_t uart_config = {
        .baud_rate = config->baud_rate,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    
    // 安装UART驱动
    esp_err_t err = uart_driver_install(
        config->uart_num, 
        2048,   // RX缓冲区
        2048,   // TX缓冲区
        10,     // 事件队列大小
        &config->event_queue,   // 事件队列
        0       // 中断标志
    );
    
    if(err != ESP_OK) {
        ESP_LOGE(TAG, "UART driver install failed: %d", err);
        return err;
    }
    
    // 配置参数和引脚
    err = uart_param_config(config->uart_num, &uart_config);
    if(err != ESP_OK) return err;
    
    err = uart_set_pin(
        config->uart_num,
        config->tx_pin,
        config->rx_pin,
        UART_PIN_NO_CHANGE,
        UART_PIN_NO_CHANGE
    );
    
    if(err != ESP_OK) return err;
    
    // 创建事件处理任务
    xTaskCreate(uart_event_task, "uart_event_task", 4096, NULL, 10, NULL);
    
    ESP_LOGI(TAG, "UART%d initialized: TX=%d, RX=%d, Baud=%d",
             config->uart_num, config->tx_pin, config->rx_pin, config->baud_rate);
             
    return ESP_OK;
}

// 注册接收回调
void uart_register_rx_callback(uart_rx_cb_t callback) {
    rx_callback = callback;
}

// 发送数据（队列方式）
esp_err_t uart_send_data(const uint8_t *data, size_t len, TickType_t timeout) {
    if(uart_cfg == NULL) return ESP_ERR_INVALID_STATE;
    if(len == 0) return ESP_OK;
    
    // 分配数据包
    uart_packet_t *packet = malloc(sizeof(uart_packet_t));
    if(packet == NULL) return ESP_ERR_NO_MEM;
    
    packet->type = UART_RAW_DATA;
    packet->len = len;
    packet->data = malloc(len);
    if(packet->data == NULL) {
        free(packet);
        return ESP_ERR_NO_MEM;
    }
    memcpy(packet->data, data, len);
    
    // 发送到TX队列
    if(xQueueSend(uart_cfg->tx_queue, &packet, timeout) != pdPASS) {
        free(packet->data);
        free(packet);
        return ESP_FAIL;
    }
    
    return ESP_OK;
}

// 传输任务
void uart_transmit_task(void *arg) {
    uart_packet_t *packet;
    
    while(1) {
        if(xQueueReceive(uart_cfg->tx_queue, &packet, portMAX_DELAY) == pdPASS) {
            // 实际发送数据
            uart_write_bytes(uart_cfg->uart_num, (const char*)packet->data, packet->len);
            // 释放数据包
            free(packet->data);
            free(packet);
        }
    }
}

// 接收任务
void uart_receive_task(void *arg) {
    uart_packet_t *packet;
    
    while(1) {
        if(xQueueReceive(uart_cfg->rx_queue, &packet, portMAX_DELAY) == pdPASS) {
            if(rx_callback) {
                // 调用用户提供的回调
                rx_callback(packet);
            } else {
                // 默认处理：释放内存
                ESP_LOG_BUFFER_HEXDUMP(TAG, packet->data, packet->len, ESP_LOG_INFO);
                free(packet->data);
                free(packet);
            }
        }
    }
}