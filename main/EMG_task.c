#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "ads1292r.h"
#include "circular_buffer.h"
#include "string.h"
#include "esp_log.h"
#include "ads1292r.h"
#include "driver/uart.h"

#define BUF_LEN 240
#define SAMPLE_ADDR 0x11

static const char* TAG = "EMG_ADS1292R";

ADS1292R_t ads;
CircularBuffer circ;

uint8_t read_buf[BUF_LEN];
uint8_t tx_buf[260];

static uint8_t ack_data_pack(uint8_t addr, uint8_t *in, uint16_t in_len, uint8_t *out) {
    out[0] = 0xA5;
    out[1] = 2 + in_len;
    out[2] = addr;
    out[3] = out[1] ^ out[2];
    memcpy(&out[4], in, in_len);
    out[in_len + 4] = 0x5A;
    return in_len + 5;
}

static TaskHandle_t emg_task_handle = NULL;

static void IRAM_ATTR drdy_isr_handler(void *arg) {
    // float ch_val[2];
    // ads1292r_read_data(&ads, ch_val);
    // // vTaskNotifyGiveFromISR(s_ads_task_handle, &xHigherPriorityTaskWoken);
    // // portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    // circular_buffer_write(&circ, (uint8_t *)ch_val, 8);
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if (emg_task_handle) {
        vTaskNotifyGiveFromISR(emg_task_handle, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }

}


void FRT_EMG_Task() {
    emg_task_handle = xTaskGetCurrentTaskHandle();  // 注册当前任务句柄

    circular_buffer_init(&circ, 1000);

    // 初始化 SPI 总线
    spi_bus_config_t buscfg = {
        .mosi_io_num = GPIO_NUM_18,
        .miso_io_num = GPIO_NUM_16,
        .sclk_io_num = GPIO_NUM_17,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 64,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));
    
    ads.cs_pin = GPIO_NUM_7;
    ads.pwdn_pin = GPIO_NUM_14;
    ads.start_pin = GPIO_NUM_13;
    ads.drdy_pin = GPIO_NUM_15;
    ads1292r_init(&ads, SPI2_HOST);

    gpio_install_isr_service(0);
    gpio_set_intr_type(ads.drdy_pin, GPIO_INTR_NEGEDGE);
    gpio_isr_handler_add(ads.drdy_pin, drdy_isr_handler, NULL);

    ads1292r_start(&ads);

    while (1) {
        // 等待 DRDY 通知，最多阻塞 100ms
        ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(100));

         // 读取数据并写入环形缓冲区

        float data[2];  // 改为 float 类型
        
        if (ads1292r_read_data(&ads, data) == ESP_OK) {
            circular_buffer_write(&circ, (uint8_t *)data, sizeof(data));
        }

        // 如果缓冲区有足够数据，打包发送
        if (circular_buffer_count(&circ) >= BUF_LEN) {
            circular_buffer_read(&circ, read_buf, BUF_LEN);
            uint8_t len = ack_data_pack(SAMPLE_ADDR, read_buf, BUF_LEN, tx_buf);
            uart_write_bytes(UART_NUM_1,tx_buf,len);
            // fwrite(tx_buf, 1, len, stdout);
        }
    }
}

void create_EMG_tasks(void) {
    xTaskCreate(
            FRT_EMG_Task,                   // 任务函数
            "FRT_GH3300_Task",                 // 任务名称
            4096,                       // 堆栈大小（字节）
            NULL,                       // 参数传递
            configMAX_PRIORITIES - 1,   // 优先级（高）
            NULL                        // 任务句柄
        );
}
