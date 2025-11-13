/*
 * 协议演示程序
 * 展示如何在BLE通信中使用协议处理函数
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "esp_log.h"
#include "protocol_handler.h"
#include "ble_data_sender.h"
#define TAG "PROTOCOL_DEMO"

/**
 * @brief 演示开关控制命令的使用
 */
void demo_switch_control(void) {
    ESP_LOGI(TAG, "=== 开关控制演示 ===");
    
    uint8_t send_frame[20];
    uint16_t send_len;
    
    // 编码开关控制命令（开启）
    encode_switch_control(1, send_frame, &send_len);
    
    // 模拟接收端解码
    uint8_t switch_state;
    decode_switch_control(send_frame, send_len, &switch_state);
    
    ESP_LOGI(TAG, "开关控制演示完成\n");
}

/**
 * @brief 演示设备信息命令的使用
 */
void demo_device_info(void) {
    ESP_LOGI(TAG, "=== 设备信息演示 ===");
    
    uint8_t send_frame[20];
    uint16_t send_len;
    
    // 编码设备信息命令
    uint32_t device_id = 0xA0001;     // A0001
    uint32_t fw_version = 0x00010000; // 版本0.1.0
    
    encode_device_info(device_id, fw_version, send_frame, &send_len);
    
    // 模拟接收端解码
    uint32_t recv_device_id, recv_fw_version;
    decode_device_info(send_frame, send_len, &recv_device_id, &recv_fw_version);
    
    ESP_LOGI(TAG, "设备信息演示完成\n");
}

/**
 * @brief 演示设备状态命令的使用
 */
void demo_device_status(void) {
    ESP_LOGI(TAG, "=== 设备状态演示 ===");
    
    uint8_t send_frame[19];
    uint16_t send_len;
    
    // 编码设备状态命令（85%电量）
    encode_device_status(85, send_frame, &send_len);
    
    // 模拟接收端解码
    uint8_t battery;
    decode_device_status(send_frame, send_len, &battery);
    
    ESP_LOGI(TAG, "设备状态演示完成\n");
}

/**
 * @brief 演示生物数据命令的使用
 */
void demo_bio_data(void) {
    ESP_LOGI(TAG, "=== 生物数据演示 ===");
    
    uint8_t send_frame[19];
    uint16_t send_len;
    
    // 构建生物数据
    bio_data_t bio = {
        .bia = 2500,      // 2500欧姆
        .gsr = 30000,     // 30000mV
        .emg1 = 15000,    // 15000mV
        .emg2 = 18000,    // 18000mV
        .hr = 75,         // 75 BPM
        .hrv = 250        // 250ms
    };
    
    // 编码生物数据命令
    encode_bio_data(&bio, send_frame, &send_len);
    
    // 模拟接收端解码
    bio_data_t recv_bio;
    decode_bio_data(send_frame, send_len, &recv_bio);
    
    ESP_LOGI(TAG, "生物数据演示完成\n");
}

/**
 * @brief 演示电流序列命令的使用
 */
void demo_current_seq(void) {
    ESP_LOGI(TAG, "=== 电流序列演示 ===");
    
    uint8_t send_frame[20];
    uint16_t send_len;
    
    // 构建电流序列数据
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
    
    // 编码电流序列命令
    encode_current_seq(&current, send_frame, &send_len);
    
    // 模拟接收端解码
    current_seq_t recv_current;
    decode_current_seq(send_frame, send_len, &recv_current);
    
    ESP_LOGI(TAG, "电流序列演示完成\n");
}

/**
 * @brief 演示实际BLE通信场景
 */
void demo_ble_communication(void) {
    ESP_LOGI(TAG, "=== BLE通信测试 ===");
    
    // // 场景1: 设备连接后发送设备信息
    // ESP_LOGI(TAG, "场景1: 设备连接，发送设备信息");
    // uint8_t frame[20];
    // uint16_t len;

    // ESP_LOGI(TAG, "设备信息示例1");
    // // encode_device_info(0xA0001, 0x00010000, frame, &len);
    // // 这里可以通过BLE发送 frame 数据
    // if (send_device_info_to_miniapp(0xA0001, 0x00010000) == 0) {
    //     ESP_LOGI(TAG, "设备信息已通过BLE发送");
    // } else {
    //     ESP_LOGE(TAG, "设备信息BLE发送失败");
    // }

    // ESP_LOGI(TAG, "设备信息示例2");
    // // encode_device_info(0xA0001, 0x00010000, frame, &len);
    // // 这里可以通过BLE发送 frame 数据
    // if (send_device_info_to_miniapp(0xA0002, 0x00002000) == 0) {
    //     ESP_LOGI(TAG, "设备信息已通过BLE发送");
    // } else {
    //     ESP_LOGE(TAG, "设备信息BLE发送失败");
    // }


    // // 场景2: 小程序查询设备状态
    // ESP_LOGI(TAG, "场景2: 小程序查询设备状态");
    // ESP_LOGI(TAG, "设备状态示例1");
    // // encode_device_status(85, frame, &len);//电量
    // // 这里可以通过BLE发送 frame 数据
    // if (send_device_status_to_miniapp(50) == 0) {
    //     ESP_LOGI(TAG, "设备状态已通过BLE发送");
    // } else {
    //     ESP_LOGE(TAG, "设备状态BLE发送失败");
    // }

    // ESP_LOGI(TAG, "设备状态示例2");
    // // encode_device_status(85, frame, &len);//电量
    // // 这里可以通过BLE发送 frame 数据
    // if (send_device_status_to_miniapp(100) == 0) {
    //     ESP_LOGI(TAG, "设备状态已通过BLE发送");
    // } else {
    //     ESP_LOGE(TAG, "设备状态BLE发送失败");
    // }

    // // 场景3: 设备主动上报生物数据
    // ESP_LOGI(TAG, "场景3: 设备主动上报生物数据");
    // ESP_LOGI(TAG, "生物数据示例1");
    // bio_data_t bio = {
    //     .bia = 2500, .gsr = 30000, .emg1 = 15000, .emg2 = 18000, .hr = 75, .hrv = 250
    // };
    // // encode_bio_data(&bio, frame, &len);
    // // 这里可以通过BLE发送 frame 数据
    // if (send_bio_data_to_miniapp(&bio) == 0) {
    //     ESP_LOGI(TAG, "生物数据示例1已通过BLE发送");
    // } else {
    //     ESP_LOGE(TAG, "生物数据示例1 BLE发送失败");
    // }

    // ESP_LOGI(TAG, "生物数据示例2");
    // bio_data_t bio2 = {
    //     .bia = 5000, .gsr = 65535, .emg1 = 65535, .emg2 = 65535, .hr = 500, .hrv = 1000
    // };
    // // encode_bio_data(&bio, frame, &len);
    // // 这里可以通过BLE发送 frame 数据
    // if (send_bio_data_to_miniapp(&bio2) == 0) {
    //     ESP_LOGI(TAG, "生物数据示例2已通过BLE发送");
    // } else {
    //     ESP_LOGE(TAG, "生物数据示例2 BLE发送失败");
    // }


    // // 场景4: 小程序发送控制命令
    // ESP_LOGI(TAG, "场景4: 小程序发送控制命令");
    // // encode_switch_control(1, frame, &len);
    // // 这里可以通过BLE发送 frame 数据
    // if (send_switch_control_to_miniapp(1) == 0) {
    //     ESP_LOGI(TAG, "开关控制命令已通过BLE发送");
    // } else {
    //     ESP_LOGE(TAG, "开关控制命令BLE发送失败");
    // }

    // 场景5: 小程序设置电流参数
    ESP_LOGI(TAG, "场景5: 小程序设置电流参数");
    current_seq_t current = {
        .mode = 3, .low_freq_wave = 200, .low_freq = 500, .mid_freq_wave = 5,
        .mid_carrier = 50, .mid_modulation = 8, .action_wave = 6, .action_freq = 6,
        .intensity = 50, .duty_cycle = 50, .rise_time = 59
    };
    // encode_current_seq(&current, frame, &len);
    // 这里可以通过BLE发送 frame 数据
    if (send_current_seq_to_miniapp(&current) == 0) {
        ESP_LOGI(TAG, "电流序列参数已通过BLE发送");
    } else {
        ESP_LOGE(TAG, "电流序列参数BLE发送失败");
    }

    ESP_LOGI(TAG, "BLE通信发送测试完成\n");
}

/**
 * @brief 运行所有演示
 */
void run_protocol_demo(void) {
    // demo_switch_control();
    // demo_device_info();
    // demo_device_status();
    // demo_bio_data();
    // demo_current_seq();
    demo_ble_communication();
}