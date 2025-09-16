// #include <stdio.h>
// #include <string.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "freertos/queue.h"
// #include "esp_log.h"
// #include "esp_adc/adc_oneshot.h"
// #include "esp_adc/adc_cali.h"
// #include "esp_adc/adc_cali_scheme.h"

// #define TAG "ADC_KEYS"

// // ADC配置
// #define ADC_CHANNEL        ADC_CHANNEL_8     // ADC1通道3 (GPIO39)
// #define ADC_UNIT           ADC_UNIT_1
// #define ADC_BIT_WIDTH      ADC_BITWIDTH_12
// #define ADC_ATTEN          ADC_ATTEN_DB_12
// #define NUM_SAMPLES        16               // 采样数量
// #define KEY_SCAN_INTERVAL  20               // 按键扫描间隔(ms)

// // 按键电压阈值
// typedef enum {
//     KEY_NONE = 0,
//     KEY_UP,
//     KEY_DOWN,
//     KEY_LEFT,
//     KEY_RIGHT,
//     KEY_OK,
//     KEY_COUNT
// } key_id_t;

// // 按键电压范围（单位：毫伏）
// static const uint32_t key_thresholds[KEY_COUNT] = {
//     [KEY_NONE]  = 0,
//     [KEY_UP]    = 500,
//     [KEY_DOWN]  = 1100,
//     [KEY_LEFT]  = 1800,
//     [KEY_RIGHT] = 2400,
//     [KEY_OK]    = 3000
// };

// // 按键状态结构
// typedef struct {
//     key_id_t current_key;
//     key_id_t prev_key;
//     uint32_t stable_time;
//     bool pressed;
// } key_state_t;

// // 按键事件队列
// static QueueHandle_t key_event_queue = NULL;

// // ADC句柄和校准句柄
// static adc_oneshot_unit_handle_t adc_handle = NULL;
// static adc_cali_handle_t cali_handle = NULL;

// // 初始化ADC校准
// static bool adc_calibration_init(void)
// {
//     adc_cali_handle_t handle = NULL;
//     esp_err_t ret = ESP_FAIL;
//     bool calibrated = false;

//     // 获取支持的校准方案
//     adc_cali_scheme_ver_t scheme_mask = 0;
//     esp_err_t check_ret = adc_cali_check_scheme(&scheme_mask);
    
//     // 检查曲线拟合方案是否可用
//     if (check_ret == ESP_OK && (scheme_mask & ADC_CALI_SCHEME_VER_CURVE_FITTING)) {
//         ESP_LOGI(TAG, "Curve fitting scheme supported");
        
//         // 配置校准参数（去除default_vref成员）
//         adc_cali_curve_fitting_config_t cali_config = {
//             .unit_id = ADC_UNIT,
//             .atten = ADC_ATTEN,
//             .bitwidth = ADC_BIT_WIDTH,
//         };
        
//         // 创建校准方案
//         ret = adc_cali_create_scheme_curve_fitting(&cali_config, &handle);
//         if (ret == ESP_OK) {
//             ESP_LOGI(TAG, "Calibration scheme created successfully");
//             cali_handle = handle;
//             calibrated = true;
//         } else {
//             ESP_LOGE(TAG, "Failed to create calibration scheme: %s", esp_err_to_name(ret));
//         }
//     } else {
//         ESP_LOGW(TAG, "Curve fitting calibration not supported or check failed: %s", esp_err_to_name(check_ret));
//     }
    
//     return calibrated;
// }

// // 初始化ADC
// void adc_keys_init(void)
// {
//     // ADC初始化配置
//     adc_oneshot_unit_init_cfg_t init_config = {
//         .unit_id = ADC_UNIT,
//         .ulp_mode = ADC_ULP_MODE_DISABLE,
//     };
//     ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc_handle));

//     // ADC通道配置
//     adc_oneshot_chan_cfg_t channel_config = {
//         .bitwidth = ADC_BIT_WIDTH,
//         .atten = ADC_ATTEN,
//     };
//     ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, ADC_CHANNEL, &channel_config));

//     // 初始化校准
//     if (!adc_calibration_init()) {
//         ESP_LOGW(TAG, "Calibration not available, using raw ADC values");
//     }

//     // 创建按键事件队列
//     key_event_queue = xQueueCreate(10, sizeof(key_id_t));
//     if (!key_event_queue) {
//         ESP_LOGE(TAG, "Failed to create key event queue");
//     }
// }

// // 读取ADC值（毫伏）
// static uint32_t read_adc_voltage(void)
// {
//     int raw_value;
//     uint32_t sum = 0;
    
//     // 多次采样取平均
//     for (int i = 0; i < NUM_SAMPLES; i++) {
//         ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, ADC_CHANNEL, &raw_value));
//         sum += raw_value;
//         vTaskDelay(1);
//     }
    
//     int avg = sum / NUM_SAMPLES;
    
//     // 如果校准可用，转换为电压
//     if (cali_handle) {
//         int cali_voltage;
//         esp_err_t ret = adc_cali_raw_to_voltage(cali_handle, avg, &cali_voltage);
        
//         if (ret == ESP_OK) {
//             return (uint32_t)cali_voltage;
//         } else {
//             ESP_LOGW(TAG, "Raw to voltage conversion failed: %s", esp_err_to_name(ret));
//         }
//     }
    
//     // 返回原始ADC值（未经校准）
//     return (uint32_t)avg;
// }

// // 识别按键值
// static key_id_t get_key_id(uint32_t voltage)
// {
//     for (int i = KEY_COUNT - 1; i > 0; i--) {
//         if (voltage >= key_thresholds[i]) {
//             return (key_id_t)i;
//         }
//     }
//     return KEY_NONE;
// }

// // 按键扫描任务
// void key_scan_task(void *arg)
// {
//     key_state_t key_state = {
//         .current_key = KEY_NONE,
//         .prev_key = KEY_NONE,
//         .stable_time = 0,
//         .pressed = false
//     };

//     const TickType_t xInterval = pdMS_TO_TICKS(KEY_SCAN_INTERVAL);
//     TickType_t xLastWakeTime = xTaskGetTickCount();

//     while (1) {
//         // 读取电压并识别按键
//         uint32_t voltage = read_adc_voltage();
//         key_id_t key_id = get_key_id(voltage);
        
//         ESP_LOGI(TAG, "Key %d pressed (%lumV)", key_state.current_key, voltage);
//         // 更新按键状态
//         key_state.current_key = key_id;
        
//         // 状态检测逻辑
//         if (key_state.current_key != key_state.prev_key) {
//             // 按键状态改变
//             key_state.stable_time = 0;
//         } else {
//             // 稳定状态
//             key_state.stable_time += KEY_SCAN_INTERVAL;
//         }

//         // 检测按键按下 (状态稳定超过50ms)
//         if (!key_state.pressed && 
//             key_state.stable_time >= 50 && 
//             key_state.current_key != KEY_NONE) 
//         {
//             key_state.pressed = true;
//             ESP_LOGI(TAG, "Key %d pressed (%lumV)", key_state.current_key, voltage);
            
//             // 发送按键事件
//             if (key_event_queue) {
//                 xQueueSend(key_event_queue, &key_state.current_key, 0);
//             }
//         }

//         // 检测按键释放 (检测到KEY_NONE并稳定)
//         if (key_state.pressed && 
//             key_state.current_key == KEY_NONE && 
//             key_state.stable_time >= 50) 
//         {
//             key_state.pressed = false;
//             ESP_LOGI(TAG, "Key released");
//         }

//         // 保存当前状态
//         key_state.prev_key = key_state.current_key;
        
//         // 精确延时
//         vTaskDelayUntil(&xLastWakeTime, xInterval);
//     }
// }

// // 按键处理任务（示例）
// void key_process_task(void *arg)
// {
//     key_id_t key_id;
    
//     while (1) {
//         if (xQueueReceive(key_event_queue, &key_id, portMAX_DELAY)) {
//             switch (key_id) {
//                 case KEY_UP:
//                     ESP_LOGI(TAG, "Action: UP pressed");
//                     break;
//                 case KEY_DOWN:
//                     ESP_LOGI(TAG, "Action: DOWN pressed");
//                     break;
//                 case KEY_LEFT:
//                     ESP_LOGI(TAG, "Action: LEFT pressed");
//                     break;
//                 case KEY_RIGHT:
//                     ESP_LOGI(TAG, "Action: RIGHT pressed");
//                     break;
//                 case KEY_OK:
//                     ESP_LOGI(TAG, "Action: OK pressed");
//                     break;
//                 default:
//                     break;
//             }
//         }
//     }
// }

// void FRT_adc_buttom_TaskCreate()
// {
//     // 初始化ADC按键
//     adc_keys_init();
    
//     // 创建任务
//     xTaskCreate(key_scan_task, "KeyScan", 4096, NULL, 5, NULL);
//     xTaskCreate(key_process_task, "KeyProcess", 4096, NULL, 5, NULL);
    
//     ESP_LOGI(TAG, "ADC multi-key demo started on GPIO39");
// }

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

static const char* TAG = "ADC_MULTIKEY";

// ADC配置
#define ADC_UNIT            ADC_UNIT_1
#define ADC_CHANNEL         ADC_CHANNEL_5  // GPIO39 对应 ADC1_CHANNEL_8
#define ADC_ATTEN           ADC_ATTEN_DB_12 // 支持0-3.3V电压范围
#define ADC_BITWIDTH        ADC_BITWIDTH_12

// 按键配置
#define KEY_COUNT           5
#define KEY_SAMPLE_PERIOD   50    // 采样周期 50ms
#define KEY_DEBOUNCE_TIME   3     // 防抖时间，连续3次采样相同才认为有效
#define KEY_LONG_PRESS_TIME 20    // 长按时间 1000ms (20 * 50ms)

// 按键类型定义
typedef enum {
    KEY_NONE = 0,
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5
} key_id_t;

// 按键事件类型
typedef enum {
    KEY_EVENT_NONE = 0,
    KEY_EVENT_PRESS,      // 按键按下
    KEY_EVENT_RELEASE,    // 按键释放
    KEY_EVENT_LONG_PRESS, // 长按
    KEY_EVENT_CLICK       // 单击
} key_event_t;

// 按键消息结构
typedef struct {
    key_id_t key_id;
    key_event_t event;
} key_msg_t;

// 按键状态结构
typedef struct {
    key_id_t current_key;
    key_id_t last_key;
    uint8_t debounce_count;
    uint8_t long_press_count;
    bool is_pressed;
    bool long_press_sent;
} key_state_t;

// 按键电压阈值表 (mV)
// 根据实际电路的分压电阻调整这些值
static const int key_voltage_table[KEY_COUNT + 1][2] = {
    {3170,    3200},   // KEY_NONE: 0-100mV
    {3000,  3100},   // KEY_1: 200-400mV
    {2150,  2200},   // KEY_2: 500-700mV
    {1000,  1100},  // KEY_3: 800-1000mV
    {0, 100},  // KEY_4: 1100-1300mV
    {1600, 1400}   // KEY_5: 1400-1600mV
};

// 全局变量
static adc_oneshot_unit_handle_t adc_handle = NULL;
static adc_cali_handle_t adc_cali_handle = NULL;
static QueueHandle_t key_queue = NULL;
static key_state_t key_state = {0};

// ADC校准初始化
static bool adc_calibration_init(void)
{
    bool calibrated = false;

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    ESP_LOGI(TAG, "Calibration scheme version is %s", "Curve Fitting");
    adc_cali_curve_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT,
        .atten = ADC_ATTEN,
        .bitwidth = ADC_BITWIDTH,
    };
    
    esp_err_t ret = adc_cali_create_scheme_curve_fitting(&cali_config, &adc_cali_handle);
    if (ret == ESP_OK) {
        calibrated = true;
    }
#elif ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    ESP_LOGI(TAG, "Calibration scheme version is %s", "Line Fitting");
    adc_cali_line_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT,
        .atten = ADC_ATTEN,
        .bitwidth = ADC_BITWIDTH,
    };
    
    esp_err_t ret = adc_cali_create_scheme_line_fitting(&cali_config, &adc_cali_handle);
    if (ret == ESP_OK) {
        calibrated = true;
    }
#endif

    return calibrated;
}

// ADC初始化
static void adc_init(void)
{
    // ADC单次转换初始化
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT,
        .ulp_mode = false,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc_handle));

    // 配置ADC通道
    adc_oneshot_chan_cfg_t config = {
        .atten = ADC_ATTEN,
        .bitwidth = ADC_BITWIDTH,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, ADC_CHANNEL, &config));

    // 初始化ADC校准
    if (adc_calibration_init()) {
        ESP_LOGI(TAG, "ADC calibration initialized");
    } else {
        ESP_LOGW(TAG, "ADC calibration not supported");
    }
}

// 读取ADC电压值
static int read_adc_voltage(void)
{
    int adc_raw = 0;
    int voltage = 0;
    
    ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, ADC_CHANNEL, &adc_raw));
    
    if (adc_cali_handle != NULL) {
        ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc_cali_handle, adc_raw, &voltage));
    } else {
        // 如果没有校准，使用简单的线性转换
        voltage = (adc_raw * 3300) / 4095;
    }
    
    return voltage;
}

// 根据电压值判断按键
static key_id_t voltage_to_key(int voltage)
{
    for (int i = 0; i <= KEY_COUNT; i++) {
        if (voltage >= key_voltage_table[i][0] && voltage <= key_voltage_table[i][1]) {
            return (key_id_t)i;
        }
    }
    return KEY_NONE;
}

// 发送按键消息
static void send_key_event(key_id_t key_id, key_event_t event)
{
    key_msg_t msg = {
        .key_id = key_id,
        .event = event
    };
    
    if (xQueueSend(key_queue, &msg, 0) != pdTRUE) {
        ESP_LOGW(TAG, "Key queue full, message dropped");
    }
}

// 按键扫描任务
static void key_scan_task(void *pvParameters)
{
    ESP_LOGI(TAG, "Key scan task started");
    
    while (1) {
        int voltage = read_adc_voltage();
        key_id_t detected_key = voltage_to_key(voltage);
        
        // 防抖处理
        if (detected_key == key_state.current_key) {
            key_state.debounce_count++;
        } else {
            key_state.current_key = detected_key;
            key_state.debounce_count = 1;
        }
        
        // 防抖时间达到，认为按键状态有效
        if (key_state.debounce_count >= KEY_DEBOUNCE_TIME) {
            key_id_t stable_key = key_state.current_key;
            
            // 检测按键状态变化
            if (stable_key != key_state.last_key) {
                // 按键释放事件
                if (key_state.last_key != KEY_NONE && stable_key == KEY_NONE) {
                    send_key_event(key_state.last_key, KEY_EVENT_RELEASE);
                    
                    // 如果之前没有发送长按事件，则发送单击事件
                    if (!key_state.long_press_sent) {
                        send_key_event(key_state.last_key, KEY_EVENT_CLICK);
                    }
                    
                    key_state.is_pressed = false;
                    key_state.long_press_count = 0;
                    key_state.long_press_sent = false;
                }
                // 按键按下事件
                else if (key_state.last_key == KEY_NONE && stable_key != KEY_NONE) {
                    send_key_event(stable_key, KEY_EVENT_PRESS);
                    key_state.is_pressed = true;
                    key_state.long_press_count = 0;
                    key_state.long_press_sent = false;
                }
                
                key_state.last_key = stable_key;
            }
            
            // 长按检测
            if (key_state.is_pressed && stable_key != KEY_NONE) {
                key_state.long_press_count++;
                if (key_state.long_press_count >= KEY_LONG_PRESS_TIME && !key_state.long_press_sent) {
                    send_key_event(stable_key, KEY_EVENT_LONG_PRESS);
                    key_state.long_press_sent = true;
                }
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS(KEY_SAMPLE_PERIOD));
    }
}

// 按键事件处理任务
static void key_event_task(void *pvParameters)
{
    key_msg_t msg;
    
    ESP_LOGI(TAG, "Key event task started");
    
    while (1) {
        if (xQueueReceive(key_queue, &msg, portMAX_DELAY) == pdTRUE) {
            const char* key_names[] = {"NONE", "KEY1", "KEY2", "KEY3", "KEY4", "KEY5"};
            const char* event_names[] = {"NONE", "PRESS", "RELEASE", "LONG_PRESS", "CLICK"};
            
            ESP_LOGI(TAG, "Key Event: %s - %s", 
                     key_names[msg.key_id], 
                     event_names[msg.event]);
            
            // 在这里添加具体的按键处理逻辑
            switch (msg.key_id) {
                case KEY_1:
                    if (msg.event == KEY_EVENT_CLICK) {
                        ESP_LOGI(TAG, "KEY1 clicked - Execute function 1");
                    } else if (msg.event == KEY_EVENT_LONG_PRESS) {
                        ESP_LOGI(TAG, "KEY1 long pressed - Execute long function 1");
                    }
                    break;
                    
                case KEY_2:
                    if (msg.event == KEY_EVENT_CLICK) {
                        ESP_LOGI(TAG, "KEY2 clicked - Execute function 2");
                    } else if (msg.event == KEY_EVENT_LONG_PRESS) {
                        ESP_LOGI(TAG, "KEY2 long pressed - Execute long function 2");
                    }
                    break;
                    
                case KEY_3:
                    if (msg.event == KEY_EVENT_CLICK) {
                        ESP_LOGI(TAG, "KEY3 clicked - Execute function 3");
                    } else if (msg.event == KEY_EVENT_LONG_PRESS) {
                        ESP_LOGI(TAG, "KEY3 long pressed - Execute long function 3");
                    }
                    break;
                    
                case KEY_4:
                    if (msg.event == KEY_EVENT_CLICK) {
                        ESP_LOGI(TAG, "KEY4 clicked - Execute function 4");
                    } else if (msg.event == KEY_EVENT_LONG_PRESS) {
                        ESP_LOGI(TAG, "KEY4 long pressed - Execute long function 4");
                    }
                    break;
                    
                case KEY_5:
                    if (msg.event == KEY_EVENT_CLICK) {
                        ESP_LOGI(TAG, "KEY5 clicked - Execute function 5");
                    } else if (msg.event == KEY_EVENT_LONG_PRESS) {
                        ESP_LOGI(TAG, "KEY5 long pressed - Execute long function 5");
                    }
                    break;
                    
                default:
                    break;
            }
        }
    }
}

// ADC电压监测任务（用于调试和校准）
static void adc_monitor_task(void *pvParameters)
{
    while (1) {
        int voltage = read_adc_voltage();
        key_id_t key = voltage_to_key(voltage);
        
        // 每秒打印一次ADC值，用于调试
        ESP_LOGI(TAG, "ADC Voltage: %d mV, Detected Key: %d", voltage, key);
        
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void FRT_adc_buttom_TaskCreate(void)
{
    ESP_LOGI(TAG, "Starting ADC Multi-Key Application");
    
    // 初始化ADC
    adc_init();
    
    // 创建按键消息队列
    key_queue = xQueueCreate(10, sizeof(key_msg_t));
    if (key_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create key queue");
        return;
    }
    
    // 创建按键扫描任务
    xTaskCreate(key_scan_task, "key_scan", 4096, NULL, 5, NULL);
    
    // 创建按键事件处理任务
    xTaskCreate(key_event_task, "key_event", 4096, NULL, 4, NULL);
    
    // 创建ADC监测任务（可选，用于调试）
    // 注释掉下面这行可以减少日志输出
    xTaskCreate(adc_monitor_task, "adc_monitor", 4096, NULL, 2, NULL);
    
    ESP_LOGI(TAG, "All tasks created successfully");
}