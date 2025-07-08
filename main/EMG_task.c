#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "ads1292r.h"
#include "circular_buffer.h"
#include "string.h"
#define BUF_LEN 240
#define SAMPLE_ADDR 0x11

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

static void IRAM_ATTR drdy_isr_handler(void *arg) {
    float ch_val[2];
    ads1292r_read_data(&ads, ch_val);
    circular_buffer_write(&circ, (uint8_t *)ch_val, 8);
}

void FRT_EMG_Task() {
    circular_buffer_init(&circ, 1000);

    // 初始化 SPI 总线
    spi_bus_config_t buscfg = {
        .mosi_io_num = GPIO_NUM_11,
        .miso_io_num = GPIO_NUM_12,
        .sclk_io_num = GPIO_NUM_13,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 64,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));
    
    ads.cs_pin = GPIO_NUM_7;
    ads.pwdn_pin = GPIO_NUM_4;
    ads.start_pin = GPIO_NUM_5;
    ads.drdy_pin = GPIO_NUM_2;
    ads1292r_init(&ads, SPI2_HOST);

    gpio_install_isr_service(0);
    gpio_set_intr_type(ads.drdy_pin, GPIO_INTR_NEGEDGE);
    gpio_isr_handler_add(ads.drdy_pin, drdy_isr_handler, NULL);

    ads1292r_start(&ads);

    while (1) {
        if (circular_buffer_count(&circ) > BUF_LEN) {
            circular_buffer_read(&circ, read_buf, BUF_LEN);
            uint8_t len = ack_data_pack(SAMPLE_ADDR, read_buf, BUF_LEN, tx_buf);
            fwrite(tx_buf, 1, len, stdout);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
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
