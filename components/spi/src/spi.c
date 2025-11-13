#include "spi.h"
#include "esp_log.h"

#define TAG "SPI_DRIVER"

#include "driver/gpio.h"
#include "global_config.h"

// 默认引脚占位，可在初始化函数中修改
#if (USE_GH3300_EVK)
    #define PIN_NUM_MISO        7
    #define PIN_NUM_MOSI        8
    #define PIN_NUM_CLK         6
    #define PIN_NUM_CS          13
    #define PIN_NUM_RSN         18
    #define GH3X_INT_GPIO       4
#endif

#if (USE_GH3300_EVT)
    #define PIN_NUM_MISO        9
    #define PIN_NUM_MOSI        10
    #define PIN_NUM_CLK         8
    #define PIN_NUM_CS          11
    #define PIN_NUM_RSN         17
    #define GH3X_INT_GPIO       15
#endif

#if (USE_GH3300_TWO_ESP32S3)
    #define PIN_NUM_MISO        12
    #define PIN_NUM_MOSI        14
    #define PIN_NUM_CLK         13
    #define PIN_NUM_CS          10
    #define PIN_NUM_RSN         9
    #define GH3X_INT_GPIO       11
#endif

#if (USE_GH3300_DVT)
    #define PIN_NUM_MISO        12
    #define PIN_NUM_MOSI        14
    #define PIN_NUM_CLK         13
    #define PIN_NUM_CS          10
    #define PIN_NUM_RSN         9
    #define GH3X_INT_GPIO       11
#endif

#define GH3X_INT_NAME      "GH3X_INT"


static spi_device_handle_t ads_spi;

esp_err_t spi_driver_init(void)
{
    spi_bus_config_t buscfg = {
        .miso_io_num = PIN_NUM_MISO,
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 64
    };

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 8 * 1000 * 1000, // 1 MHz
        // .duty_cycle_pos = 64,     //Duty cycle of positive clock, in 1/256th increments (128 = 50%/50% duty). Setting this to 0 (=not setting it) is equivalent to setting this to 128.
        .mode = 0,                          // CPOL = 0, CPHA = 1
        // .spics_io_num = PIN_NUM_CS,
        .queue_size = 3,
        .spics_io_num = -1,
    };
    
    esp_err_t ret;

    ret = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPI bus init failed");
        return ret;
    }

    ret = spi_bus_add_device(SPI2_HOST, &devcfg, &ads_spi);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPI device add failed");
        return ret;
    }

    gpio_config_t io_conf = {
    .pin_bit_mask = (1ULL << PIN_NUM_CS),
    .mode = GPIO_MODE_OUTPUT,
    .pull_up_en = GPIO_PULLUP_DISABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
    gpio_set_level(PIN_NUM_CS, 1); // CS 初始为高电平

    ESP_LOGI(TAG, "SPI bus/device init complete");
    return ESP_OK;
}


esp_err_t spi_cs_set_level(uint32_t level){

    esp_err_t ret;
    ret = gpio_set_level(PIN_NUM_CS, level);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPI CS set level");
        return ret;
    }
    return ESP_OK;

}

esp_err_t spi_send_command(uint8_t *tx_data, size_t length)
{
    spi_transaction_t t = {
        .length = 8 * length,
        .tx_buffer = tx_data,
        .rx_buffer = NULL,
    };
    return spi_device_transmit(ads_spi, &t);
}

esp_err_t spi_send_receive(uint8_t *tx_data, uint8_t *rx_data, size_t length)
{
    spi_transaction_t t = {
        .length = 8 * length,
        .tx_buffer = tx_data,
        .rx_buffer = rx_data,
    };
    return spi_device_transmit(ads_spi, &t);
}
// int i;

 esp_err_t gh_spi_receive(uint8_t *rx_data, size_t length)
{
    spi_transaction_t t = {
        .length = 8 * length,       // 正确设置比特长度
        .rx_buffer = rx_data,
        .tx_buffer = NULL           // 必须显式置空发送缓冲区
    };

    // esp_err_t ret;
    // spi_transaction_t *ret_trans;

    // esp_err_t ret = spi_device_get_trans_result(ads_spi, &t, portMAX_DELAY);

    esp_err_t ret = spi_device_transmit(ads_spi, &t);
    
    // // 正确打印方式（二进制数据需按字节打印）
    // ESP_LOGI("GH_LOG","------------------Received %d bytes:", length);
    // for(int i=0; i<length; i++){
    //     ESP_LOGI("GH_LOG","[%02x]", rx_data[i]);
    // }
    // return ESP_OK;
    return ret;
}

// spi_transaction_t t_test;
esp_err_t gh_spi_send(uint8_t *tx_data, size_t length)
{
    // t_test.length = 8 * length;
    // t_test.tx_buffer = tx_data;
    // t_test.rx_buffer = NULL;
    // i++;
    spi_transaction_t t_test = {
        .length = 8 * length,
        .tx_buffer = tx_data,
        .rx_buffer = NULL,
    };
    //  ESP_LOGI(TAG, "tx_buffer: %p, length: %zu", (void*)t_test.tx_buffer, length);
    // if (length > 0) {  // 避免 length 为 0 时越界
    //     ESP_LOGI(TAG, "tx_data content:");
    //     for (size_t i = 0; i < length; i++) {
    //         ESP_LOGI(TAG, "  [%zu]: 0x%02X", i, tx_data[i]);  // 按字节打印十六进制值
    //     }
    // }
    esp_err_t ret = spi_device_transmit(ads_spi, &t_test);
    return ret;
    // ESP_LOGI(TAG,"tx_buffer:%p",(void*)t_test.tx_buffer);
    // esp_err_t ret = spi_device_transmit(ads_spi, &t_test);
    // // ESP_LOGI(TAG, "transmit_ret: %s", esp_err_to_name(ret));  // 更直观
    // return ret;
    // return spi_device_transmit(ads_spi, &t_test);;
}

esp_err_t esp_spi_reset_pin_init(void)
{
    esp_err_t ret;
    gpio_config_t gpio_init_struct = {0};
    gpio_init_struct.intr_type = GPIO_INTR_DISABLE; /* 失能引脚中断 */
    gpio_init_struct.mode = GPIO_MODE_INPUT_OUTPUT; /* 输入输出模式 */
    gpio_init_struct.pull_up_en = GPIO_PULLUP_ENABLE; /* 使能上拉 */
    gpio_init_struct.pull_down_en = GPIO_PULLDOWN_DISABLE; /* 失能下拉 */
    gpio_init_struct.pin_bit_mask = 1ull << PIN_NUM_RSN; /* 设置的引脚的位掩码*/
    ret = gpio_config(&gpio_init_struct); /* 配置 GPIO */
    gpio_set_level(PIN_NUM_RSN, 1);
    // esp_rom_delay_us(10000);
    // gpio_set_level(PIN_NUM_RSN, 0);
    // esp_rom_delay_us(10000);
    // gpio_set_level(PIN_NUM_RSN, 1);
    // 配置复位引脚为GPIO输出模式
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Reset pin direction set failed");
        return ret;
    }
    
    ESP_LOGI(TAG, "GH3300 reset pin (GPIO%d) initialized", PIN_NUM_RSN);
    return ESP_OK;
}

esp_err_t esp_spi_reset_pin_ctrl(uint32_t level)
{
    esp_err_t ret;
    
    ret = gpio_set_level(PIN_NUM_RSN, level);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPI RSN set level");
        return ret;
    }
    return ESP_OK;
}

// 修改后的接收函数


esp_err_t gh_ESP_LOG(char *string)
{
    ESP_LOGI("GH_LOG","%s",string);
    return ESP_OK;
}

// 用户需要在这里定义自己的中断回调函数
void IRAM_ATTR gh3x_isr_handler(void* arg)
{
    // 示例中断处理逻辑（可以发送事件通知任务等）
    // ESP_EARLY_LOGI(TAG, "Interrupt triggered on GPIO4");
}

void gh3_int_init(void)
{
    // 配置 GPIO4 为输入，上升沿中断
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_POSEDGE,       // 上升沿触发
        .mode = GPIO_MODE_INPUT,              // 设置为输入模式
        .pin_bit_mask = (1ULL << GH3X_INT_GPIO),  // 配置 GPIO4
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE     // 根据电路实际情况调整
    };
    gpio_config(&io_conf);

    // 安装 GPIO ISR 服务
    gpio_install_isr_service(ESP_INTR_FLAG_LOWMED);  // 中等优先级
    gpio_isr_handler_add(GH3X_INT_GPIO, gh3x_isr_handler, NULL);  // 注册中断处理函数

    ESP_LOGI(TAG, "Interrupt on GPIO4 initialized.");
}