/*
 * 数据协议测试代码
 * 用于测试0x51-0x55命令的发送和接收
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "esp_log.h"

#define TAG "PROTOCOL_TEST"

// 数据帧格式定义
#define FRAME_HEADER_SIZE   1
#define FRAME_TAIL_SIZE     1
#define FRAME_MAX_SIZE      20
#define FRAME_DATA_SIZE     (FRAME_MAX_SIZE - FRAME_HEADER_SIZE - FRAME_TAIL_SIZE)

// 命令定义
#define CMD_SWITCH_CONTROL  0x51
#define CMD_DEVICE_INFO     0x52
#define CMD_DEVICE_STATUS   0x53
#define CMD_BIO_DATA        0x54
#define CMD_CURRENT_SEQ     0x55
#define FRAME_TAIL          0xFF

// 数据提取宏定义
#define EXTRACT_BITS(data, start, len) (((data) >> (start)) & ((1 << (len)) - 1))
#define SET_BITS(dest, start, len, value) do { \
    (dest) &= ~(((1 << (len)) - 1) << (start)); \
    (dest) |= ((value) & ((1 << (len)) - 1)) << (start); \
} while(0)

// 数据结构定义
typedef struct {
    uint8_t switch_state;    // 1bit: 0-关, 1-开
} switch_control_t;

typedef struct {
    uint32_t device_id;     // 32bit: A0001-A0100
    uint32_t fw_version;    // 32bit: 版本号编码
} device_info_t;

typedef struct {
    uint8_t battery;        // 8bit: 0-100%
} device_status_t;

typedef struct {
    uint16_t bia;           // 16bit: 0-5000 欧姆
    uint16_t gsr;           // 16bit: 0-65536 mV
    uint16_t emg1;          // 16bit: 0-65536 mV
    uint16_t emg2;          // 16bit: 0-65536 mV
    uint16_t hr;            // 9bit: 0-500 BPM
    uint16_t hrv;           // 10bit: 0-1000 ms
} bio_data_t;

typedef struct {
    uint8_t mode;           // 2bit: 0-4
    uint8_t low_freq_wave;  // 8bit: 0-255
    uint16_t low_freq;      // 10bit: 0-999
    uint8_t mid_freq_wave;  // 4bit: 0-10
    uint8_t mid_carrier;    // 7bit: 0-100
    uint8_t mid_modulation; // 4bit: 0-10
    uint8_t action_wave;    // 4bit: 0-10
    uint8_t action_freq;    // 6bit: 0-64
    uint8_t intensity;      // 7bit: 0-100
    uint8_t duty_cycle;     // 6bit: 0-60
    uint8_t rise_time;      // 6bit: 0-64
    uint16_t pattern;       // 11bit: 模式扩展
} current_seq_t;

// 函数声明
void test_switch_control_encode(void);
void test_switch_control_decode(const uint8_t *data, uint16_t len);
void test_device_info_encode(void);
void test_device_info_decode(const uint8_t *data, uint16_t len);
void test_device_status_encode(void);
void test_device_status_decode(const uint8_t *data, uint16_t len);
void test_bio_data_encode(void);
void test_bio_data_decode(const uint8_t *data, uint16_t len);
void test_current_seq_encode(void);
void test_current_seq_decode(const uint8_t *data, uint16_t len);

// 打印十六进制数据
static void print_hex(const char *prefix, const uint8_t *data, uint16_t len) {
    printf("%s: ", prefix);
    for (int i = 0; i < len; i++) {
        printf("%02X ", data[i]);
    }
    printf("\n");
}

// 测试开关控制编码
void test_switch_control_encode(void) {
    ESP_LOGI(TAG, "=== 开关控制编码测试 ===");
    
    uint8_t frame[FRAME_MAX_SIZE] = {0};
    switch_control_t ctrl = {1}; // 开关打开
    
    // 构建帧
    frame[0] = CMD_SWITCH_CONTROL;
    
    // 设置开关状态 (bit 8)
    if (ctrl.switch_state) {
        frame[1] |= 0x01; // 设置bit 0
    }
    
    frame[19] = FRAME_TAIL;
    
    print_hex("开关控制发送", frame, FRAME_MAX_SIZE);
    
    // 详细解析
    ESP_LOGI(TAG, "开关状态: %s", ctrl.switch_state ? "开" : "关");
    ESP_LOGI(TAG, "帧头: 0x%02X", frame[0]);
    ESP_LOGI(TAG, "开关位: %d (bit 8)", (frame[1] & 0x01));
    ESP_LOGI(TAG, "帧尾: 0x%02X", frame[19]);
}

// 测试开关控制解码
void test_switch_control_decode(const uint8_t *data, uint16_t len) {
    ESP_LOGI(TAG, "=== 开关控制解码测试 ===");
    
    if (len < FRAME_MAX_SIZE || data[0] != CMD_SWITCH_CONTROL || data[19] != FRAME_TAIL) {
        ESP_LOGE(TAG, "无效数据帧");
        return;
    }
    
    switch_control_t ctrl;
    ctrl.switch_state = (data[1] & 0x01); // 提取bit 8
    
    ESP_LOGI(TAG, "接收数据:");
    print_hex("原始数据", data, len);
    ESP_LOGI(TAG, "开关状态: %s", ctrl.switch_state ? "开" : "关");
}

// 测试设备信息编码
void test_device_info_encode(void) {
    ESP_LOGI(TAG, "=== 设备信息编码测试 ===");
    
    uint8_t frame[FRAME_MAX_SIZE] = {0};
    device_info_t info = {
        .device_id = 0xA0001,    // A0001
        .fw_version = 0x00010000 // 0.1.0
    };
    
    // 构建帧
    frame[0] = CMD_DEVICE_INFO;
    
    // 设置设备ID (bits 8-39)
    frame[1] = (info.device_id >> 24) & 0xFF;
    frame[2] = (info.device_id >> 16) & 0xFF;
    frame[3] = (info.device_id >> 8) & 0xFF;
    frame[4] = info.device_id & 0xFF;
    
    // 设置固件版本 (bits 40-71)
    frame[5] = (info.fw_version >> 24) & 0xFF;
    frame[6] = (info.fw_version >> 16) & 0xFF;
    frame[7] = (info.fw_version >> 8) & 0xFF;
    frame[8] = info.fw_version & 0xFF;
    
    frame[19] = FRAME_TAIL;
    
    print_hex("设备信息发送", frame, FRAME_MAX_SIZE);
    
    // 详细解析
    ESP_LOGI(TAG, "设备ID: A%lu", info.device_id & 0xFFFF);
    ESP_LOGI(TAG, "固件版本: %lu.%lu.%lu", 
             (info.fw_version >> 16) & 0xFF,
             (info.fw_version >> 8) & 0xFF,
             info.fw_version & 0xFF);
}

// 测试设备信息解码
void test_device_info_decode(const uint8_t *data, uint16_t len) {
    ESP_LOGI(TAG, "=== 设备信息解码测试 ===");
    
    if (len < FRAME_MAX_SIZE || data[0] != CMD_DEVICE_INFO || data[19] != FRAME_TAIL) {
        ESP_LOGE(TAG, "无效数据帧");
        return;
    }
    
    device_info_t info;
    
    // 提取设备ID
    info.device_id = ((uint32_t)data[1] << 24) | ((uint32_t)data[2] << 16) | 
                     ((uint32_t)data[3] << 8) | data[4];
    
    // 提取固件版本
    info.fw_version = ((uint32_t)data[5] << 24) | ((uint32_t)data[6] << 16) | 
                      ((uint32_t)data[7] << 8) | data[8];
    
    print_hex("原始数据", data, len);
    ESP_LOGI(TAG, "设备ID: A%lu", info.device_id & 0xFFFF);
    ESP_LOGI(TAG, "固件版本: %lu.%lu.%lu", 
             (info.fw_version >> 16) & 0xFF,
             (info.fw_version >> 8) & 0xFF,
             info.fw_version & 0xFF);
}

// 测试设备状态编码
void test_device_status_encode(void) {
    ESP_LOGI(TAG, "=== 设备状态编码测试 ===");
    
    uint8_t frame[19] = {0}; // 19字节帧
    device_status_t status = {85}; // 85%电量
    
    // 构建帧
    frame[0] = CMD_DEVICE_STATUS;
    frame[1] = status.battery; // 电量 (bits 8-15)
    frame[18] = FRAME_TAIL;    // 帧尾
    
    print_hex("设备状态发送", frame, 19);
    
    ESP_LOGI(TAG, "电量: %d%%", status.battery);
}

// 测试设备状态解码
void test_device_status_decode(const uint8_t *data, uint16_t len) {
    ESP_LOGI(TAG, "=== 设备状态解码测试 ===");
    
    if (len < 19 || data[0] != CMD_DEVICE_STATUS || data[18] != FRAME_TAIL) {
        ESP_LOGE(TAG, "无效数据帧");
        return;
    }
    
    device_status_t status;
    status.battery = data[1]; // 提取电量
    
    print_hex("原始数据", data, len);
    ESP_LOGI(TAG, "电量: %d%%", status.battery);
}

// 测试生物数据编码
void test_bio_data_encode(void) {
    ESP_LOGI(TAG, "=== 生物数据编码测试 ===");
    
    uint8_t frame[19] = {0};
    bio_data_t bio = {
        .bia = 2500,      // 2500欧姆
        .gsr = 30000,     // 30000mV
        .emg1 = 15000,    // 15000mV
        .emg2 = 18000,    // 18000mV
        .hr = 75,         // 75 BPM
        .hrv = 250        // 250ms
    };
    
    // 构建帧
    frame[0] = CMD_BIO_DATA;
    
    // BIA (bits 8-23)
    frame[1] = (bio.bia >> 8) & 0xFF;
    frame[2] = bio.bia & 0xFF;
    
    // GSR (bits 24-39)
    frame[3] = (bio.gsr >> 8) & 0xFF;
    frame[4] = bio.gsr & 0xFF;
    
    // EMG1 (bits 40-55)
    frame[5] = (bio.emg1 >> 8) & 0xFF;
    frame[6] = bio.emg1 & 0xFF;
    
    // EMG2 (bits 56-71)
    frame[7] = (bio.emg2 >> 8) & 0xFF;
    frame[8] = bio.emg2 & 0xFF;
    
    // HR (bits 72-80, 9bit)
    frame[9] = (bio.hr >> 1) & 0xFF;
    frame[10] = (frame[10] & 0x7F) | ((bio.hr & 0x01) << 7);
    
    // HRV (bits 81-90, 10bit)
    frame[10] = (frame[10] & 0x80) | ((bio.hrv >> 3) & 0x7F);
    frame[11] = ((bio.hrv & 0x07) << 5);
    
    frame[18] = FRAME_TAIL;
    
    print_hex("生物数据发送", frame, 19);
    
    ESP_LOGI(TAG, "BIA: %d欧姆", bio.bia);
    ESP_LOGI(TAG, "GSR: %dmV", bio.gsr);
    ESP_LOGI(TAG, "EMG1: %dmV", bio.emg1);
    ESP_LOGI(TAG, "EMG2: %dmV", bio.emg2);
    ESP_LOGI(TAG, "HR: %d BPM", bio.hr);
    ESP_LOGI(TAG, "HRV: %dms", bio.hrv);
}

// 测试生物数据解码
void test_bio_data_decode(const uint8_t *data, uint16_t len) {
    ESP_LOGI(TAG, "=== 生物数据解码测试 ===");
    
    if (len < 19 || data[0] != CMD_BIO_DATA || data[18] != FRAME_TAIL) {
        ESP_LOGE(TAG, "无效数据帧");
        return;
    }
    
    bio_data_t bio;
    
    // 提取各项数据
    bio.bia = ((uint16_t)data[1] << 8) | data[2];
    bio.gsr = ((uint16_t)data[3] << 8) | data[4];
    bio.emg1 = ((uint16_t)data[5] << 8) | data[6];
    bio.emg2 = ((uint16_t)data[7] << 8) | data[8];
    bio.hr = ((uint16_t)data[9] << 1) | ((data[10] >> 7) & 0x01);
    bio.hrv = ((uint16_t)((data[10] & 0x7F) << 3) | (data[11] >> 5));
    
    print_hex("原始数据", data, len);
    ESP_LOGI(TAG, "BIA: %d欧姆", bio.bia);
    ESP_LOGI(TAG, "GSR: %dmV", bio.gsr);
    ESP_LOGI(TAG, "EMG1: %dmV", bio.emg1);
    ESP_LOGI(TAG, "EMG2: %dmV", bio.emg2);
    ESP_LOGI(TAG, "HR: %d BPM", bio.hr);
    ESP_LOGI(TAG, "HRV: %dms", bio.hrv);
}

// 测试电流序列编码
void test_current_seq_encode(void) {
    ESP_LOGI(TAG, "=== 电流序列编码测试 ===");
    
    uint8_t frame[FRAME_MAX_SIZE] = {0};
    current_seq_t current = {
        .mode = 3,              // 模式3
        .low_freq_wave = 128,   // 低频波形
        .low_freq = 500,        // 低频频率
        .mid_freq_wave = 5,     // 中频波形
        .mid_carrier = 50,      // 中频载波
        .mid_modulation = 3,    // 中频调制
        .action_wave = 2,       // 动作波形
        .action_freq = 25,      // 动作频率
        .intensity = 75,        // 强度
        .duty_cycle = 30,       // 占空比
        .rise_time = 10,        // 渐强时长
        .pattern = 1023         // 模式扩展
    };
    
    // 构建帧
    frame[0] = CMD_CURRENT_SEQ;
    
    // 模式 (bits 8-9)
    frame[1] = (current.mode & 0x03);
    
    // 低频波形 (bits 10-17)
    frame[2] = current.low_freq_wave;
    
    // 低频频率 (bits 18-27)
    frame[3] = (current.low_freq >> 2) & 0xFF;
    frame[4] = ((current.low_freq & 0x03) << 6);
    
    // 中频波形 (bits 28-31)
    frame[4] |= (current.mid_freq_wave & 0x0F) << 2;
    
    // 中频载波频率 (bits 32-38)
    frame[5] = current.mid_carrier & 0x7F;
    
    // 中频调制频率 (bits 39-42)
    frame[6] = (current.mid_modulation & 0x0F) << 4;
    
    // 动作波形 (bits 43-46)
    frame[6] |= (current.action_wave & 0x0F);
    
    // 动作频率 (bits 47-52)
    frame[7] = ((current.action_freq & 0x3F) << 2);
    
    // 强度 (bits 53-59)
    frame[8] = current.intensity & 0x7F;
    
    // 占空比 (bits 60-65)
    frame[9] = (current.duty_cycle & 0x3F) << 2;
    
    // 渐强时长 (bits 66-71)
    frame[10] = (current.rise_time & 0x3F) << 2;
    
    // 模式扩展 (bits 72-82)
    frame[11] = (current.pattern >> 3) & 0xFF;
    frame[12] = (current.pattern & 0x07) << 5;
    
    frame[19] = FRAME_TAIL;
    
    print_hex("电流序列发送", frame, FRAME_MAX_SIZE);
    
    ESP_LOGI(TAG, "模式: %d", current.mode);
    ESP_LOGI(TAG, "低频波形: %d", current.low_freq_wave);
    ESP_LOGI(TAG, "低频频率: %d", current.low_freq);
    ESP_LOGI(TAG, "中频波形: %d", current.mid_freq_wave);
    ESP_LOGI(TAG, "中频载波: %d", current.mid_carrier);
    ESP_LOGI(TAG, "中频调制: %d", current.mid_modulation);
    ESP_LOGI(TAG, "动作波形: %d", current.action_wave);
    ESP_LOGI(TAG, "动作频率: %d", current.action_freq);
    ESP_LOGI(TAG, "强度: %d", current.intensity);
    ESP_LOGI(TAG, "占空比: %d%%", current.duty_cycle);
    ESP_LOGI(TAG, "渐强时长: %d", current.rise_time);
    ESP_LOGI(TAG, "模式扩展: %d", current.pattern);
}

// 测试电流序列解码
void test_current_seq_decode(const uint8_t *data, uint16_t len) {
    ESP_LOGI(TAG, "=== 电流序列解码测试 ===");
    
    if (len < FRAME_MAX_SIZE || data[0] != CMD_CURRENT_SEQ || data[19] != FRAME_TAIL) {
        ESP_LOGE(TAG, "无效数据帧");
        return;
    }
    
    current_seq_t current;
    
    // 提取各项数据
    current.mode = data[1] & 0x03;
    current.low_freq_wave = data[2];
    current.low_freq = ((data[3] << 2) | (data[4] >> 6)) & 0x3FF;
    current.mid_freq_wave = (data[4] >> 2) & 0x0F;
    current.mid_carrier = data[5] & 0x7F;
    current.mid_modulation = (data[6] >> 4) & 0x0F;
    current.action_wave = data[6] & 0x0F;
    current.action_freq = (data[7] >> 2) & 0x3F;
    current.intensity = data[8] & 0x7F;
    current.duty_cycle = (data[9] >> 2) & 0x3F;
    current.rise_time = (data[10] >> 2) & 0x3F;
    current.pattern = ((data[11] << 3) | (data[12] >> 5)) & 0x7FF;
    
    print_hex("原始数据", data, len);
    ESP_LOGI(TAG, "模式: %d", current.mode);
    ESP_LOGI(TAG, "低频波形: %d", current.low_freq_wave);
    ESP_LOGI(TAG, "低频频率: %d", current.low_freq);
    ESP_LOGI(TAG, "中频波形: %d", current.mid_freq_wave);
    ESP_LOGI(TAG, "中频载波: %d", current.mid_carrier);
    ESP_LOGI(TAG, "中频调制: %d", current.mid_modulation);
    ESP_LOGI(TAG, "动作波形: %d", current.action_wave);
    ESP_LOGI(TAG, "动作频率: %d", current.action_freq);
    ESP_LOGI(TAG, "强度: %d", current.intensity);
    ESP_LOGI(TAG, "占空比: %d%%", current.duty_cycle);
    ESP_LOGI(TAG, "渐强时长: %d", current.rise_time);
    ESP_LOGI(TAG, "模式扩展: %d", current.pattern);
}

// 主测试函数
void protocol_test_all(void) {
    ESP_LOGI(TAG, "开始协议测试...");
    
    // 编码测试
    test_switch_control_encode();
    test_device_info_encode();
    test_device_status_encode();
    test_bio_data_encode();
    test_current_seq_encode();
    
    ESP_LOGI(TAG, "\n=== 解码测试 ===");
    
    // 创建测试数据用于解码
    uint8_t test_data[20];
    
    // 开关控制测试数据
    memset(test_data, 0, sizeof(test_data));
    test_data[0] = CMD_SWITCH_CONTROL;
    test_data[1] = 0x01; // 开关开
    test_data[19] = FRAME_TAIL;
    test_switch_control_decode(test_data, sizeof(test_data));
    
    // 设备信息测试数据
    memset(test_data, 0, sizeof(test_data));
    test_data[0] = CMD_DEVICE_INFO;
    test_data[1] = 0x00; test_data[2] = 0x0A; test_data[3] = 0x00; test_data[4] = 0x01; // A0001
    test_data[5] = 0x00; test_data[6] = 0x00; test_data[7] = 0x01; test_data[8] = 0x00; // 0.1.0
    test_data[19] = FRAME_TAIL;
    test_device_info_decode(test_data, sizeof(test_data));
    
    // 设备状态测试数据
    memset(test_data, 0, sizeof(test_data));
    test_data[0] = CMD_DEVICE_STATUS;
    test_data[1] = 85; // 85%电量
    test_data[18] = FRAME_TAIL;
    test_device_status_decode(test_data, 19);
    
    // 生物数据测试数据
    memset(test_data, 0, sizeof(test_data));
    test_data[0] = CMD_BIO_DATA;
    test_data[1] = 0x09; test_data[2] = 0xC4; // 2500
    test_data[3] = 0x75; test_data[4] = 0x30; // 30000
    test_data[5] = 0x3A; test_data[6] = 0x98; // 15000
    test_data[7] = 0x46; test_data[8] = 0x50; // 18000
    test_data[9] = 0x4B; // 75 BPM
    test_data[10] = 0x00; test_data[11] = 0x00; // 250ms HRV
    test_data[18] = FRAME_TAIL;
    test_bio_data_decode(test_data, 19);
    
    // 电流序列测试数据
    memset(test_data, 0, sizeof(test_data));
    test_data[0] = CMD_CURRENT_SEQ;
    test_data[1] = 0x03; // 模式
    test_data[2] = 128;  // 低频波形
    test_data[3] = 0x07; test_data[4] = 0xC0; // 低频频率500
    test_data[8] = 75;   // 强度
    test_data[19] = FRAME_TAIL;
    test_current_seq_decode(test_data, sizeof(test_data));
    
    ESP_LOGI(TAG, "协议测试完成!");
}

// 位操作辅助函数测试
void bit_operation_test(void) {
    ESP_LOGI(TAG, "=== 位操作测试 ===");
    
    uint16_t data = 0x1234;
    
    // 提取位测试
    uint8_t bits_4_7 = EXTRACT_BITS(data, 4, 4);
    uint8_t bits_8_11 = EXTRACT_BITS(data, 8, 4);
    
    ESP_LOGI(TAG, "原始数据: 0x%04X", data);
    ESP_LOGI(TAG, "提取bits[4:7]: 0x%01X", bits_4_7);
    ESP_LOGI(TAG, "提取bits[8:11]: 0x%01X", bits_8_11);
    
    // 设置位测试
    uint16_t test_data = 0;
    SET_BITS(test_data, 2, 3, 0x05); // 设置bits[2:4]为5
    SET_BITS(test_data, 8, 4, 0x0A); // 设置bits[8:11]为10
    
    ESP_LOGI(TAG, "设置位后: 0x%04X", test_data);
}

// 测试入口函数
void run_protocol_tests(void) {
    ESP_LOGI(TAG, "开始协议测试...");
    
    bit_operation_test();
    protocol_test_all();
    
    ESP_LOGI(TAG, "所有测试完成!");
}