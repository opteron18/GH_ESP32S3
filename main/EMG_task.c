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
#include "global_config.h"

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
    // vTaskDelay(pdMS_TO_TICKS(3000));
    emg_task_handle = xTaskGetCurrentTaskHandle();  // 注册当前任务句柄

    circular_buffer_init(&circ, 1000);

    // 初始化 SPI 总线
    spi_bus_config_t buscfg = {

        #if (USE_ADS1292R)
        //origin test module
        .mosi_io_num = GPIO_NUM_18,
        .miso_io_num = GPIO_NUM_16,
        .sclk_io_num = GPIO_NUM_17,
        //-------------------------------
        #endif

        #if (USE_DVT_ADS1292R)
        .mosi_io_num = GPIO_NUM_4,
        .miso_io_num = GPIO_NUM_2,
        .sclk_io_num = GPIO_NUM_3,
        #endif
        //-------------------------------

        #if (USE_ADS1292R_TWO_CORE)
        //spi3
        .mosi_io_num = GPIO_NUM_47,
        .miso_io_num = GPIO_NUM_21,
        .sclk_io_num = GPIO_NUM_48,
        #endif

        #if (USE_EVT_ADS1192)
        //spi3
        .mosi_io_num = GPIO_NUM_19,
        .miso_io_num = GPIO_NUM_13,
        .sclk_io_num = GPIO_NUM_14,
        //spi2
        // .mosi_io_num = GPIO_NUM_10,
        // .miso_io_num = GPIO_NUM_9,
        // .sclk_io_num = GPIO_NUM_8,
        //-------------------------------
        #endif
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 64,
    };

    //spi2
    // ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));
    //-------------------------------

    //spi3
    ESP_ERROR_CHECK(spi_bus_initialize(SPI3_HOST, &buscfg, SPI_DMA_CH_AUTO));
    //-------------------------------
    
    #if (USE_ADS1292R)
    //origin test module
    ads.cs_pin = GPIO_NUM_7;
    ads.pwdn_pin = GPIO_NUM_14;
    ads.start_pin = GPIO_NUM_13;
    ads.drdy_pin = GPIO_NUM_15;
    //-------------------------------
    #endif

    #if (USE_ADS1292R_TWO_CORE)
    //origin test module
    ads.cs_pin = GPIO_NUM_18;
    ads.pwdn_pin = GPIO_NUM_15;
    ads.start_pin = GPIO_NUM_16;
    ads.drdy_pin = GPIO_NUM_17;
    //-------------------------------
    #endif

    #if (USE_EVT_ADS1192)
    //spi3 PWDN START DRDY Pin
    ads.cs_pin = GPIO_NUM_12;
    ads.pwdn_pin = GPIO_NUM_40;
    ads.start_pin = GPIO_NUM_20;
    ads.drdy_pin = GPIO_NUM_39;
    //-------------------------------
    #endif

    #if (USE_DVT_ADS1292R)
    //origin test module
    ads.cs_pin = GPIO_NUM_7;
    ads.pwdn_pin = GPIO_NUM_9;
    ads.start_pin = GPIO_NUM_8;
    ads.drdy_pin = GPIO_NUM_1;
    #endif
    //spi2
    // ads1292r_init(&ads, SPI2_HOST);
    //-------------------------------

    //spi3
    ads1292r_init(&ads, SPI3_HOST);
    //-------------------------------

    gpio_install_isr_service(0);
    gpio_set_intr_type(ads.drdy_pin, GPIO_INTR_NEGEDGE);
    gpio_isr_handler_add(ads.drdy_pin, drdy_isr_handler, NULL);

    ads1292r_start(&ads);
    vTaskDelay(pdMS_TO_TICKS(1000));
    while (1) {
        // 等待 DRDY 通知，最多阻塞 100ms
        ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(10));

         // 读取数据并写入环形缓冲区

        float data[2];  // 改为 float 类型
        
    // gpio_set_level(ads.cs_pin, 1);
    // gpio_set_level(ads.start_pin, 1);
    // gpio_set_level(ads.pwdn_pin, 1);
    // gpio_set_level(ads.drdy_pin, 1);
    // vTaskDelay(pdMS_TO_TICKS(50));
    // gpio_set_level(ads.cs_pin, 0);
    // gpio_set_level(ads.start_pin, 0);
    // gpio_set_level(ads.pwdn_pin, 0);
    // gpio_set_level(ads.drdy_pin, 0);
    // vTaskDelay(pdMS_TO_TICKS(50));

        // uint8_t id_val = 0;
        // ads1292r_read_regs(&ads, 0x3, &id_val, 1);
        // ESP_LOGE(TAG, "DEVICE ID: 0x%02X", id_val);

        if (ads1292r_read_data(&ads, data) == ESP_OK) {
            // ESP_LOGI(TAG,"CH1: %f",data[0]);
            // ESP_LOGI(TAG,"CH2: %f",data[1]);
            // circular_buffer_write(&circ, (uint8_t *)data, sizeof(data));
        }
        // vTaskDelay(pdMS_TO_TICKS(1));
        // vTaskDelay(pdMS_TO_TICKS(1));
        // 如果缓冲区有足够数据，打包发送
        // if (circular_buffer_count(&circ) >= BUF_LEN) {
        //     circular_buffer_read(&circ, read_buf, BUF_LEN);
        //     uint8_t len = ack_data_pack(SAMPLE_ADDR, read_buf, BUF_LEN, tx_buf);
        //     #if(USE_DVT_ADS1292R)
        //         #if(USE_HOST_DEBUGER)
        //             uart_write_bytes(UART_NUM_0,tx_buf,len);
        //         #endif
        //     #endif
        //     #if(USE_ADS1292R)
        //         #if(USE_HOST_DEBUGER)
        //             uart_write_bytes(UART_NUM_0,tx_buf,len);
        //         #endif
        //     #endif
        //     #if(USE_EVT_ADS1192)
        //         #if(USE_HOST_DEBUGER)
        //             uart_write_bytes(UART_NUM_0,tx_buf,len);
        //         #endif
        //     #endif
        //     // fwrite(tx_buf, 1, len, stdout);
        // }

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
