/*
 * BLE协议测试程序
 * 用于测试真实BLE数据接收和解析功能
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "esp_log.h"
#include "ble_protocol_integration.h"
#include "protocol_handler.h"

#define TAG "BLE_PROTOCOL_TEST"

/**
 * @brief 测试开关控制命令解析
 */
void test_switch_control_parsing(void) {
    ESP_LOGI(TAG, "=== 测试开关控制命令解析 ===");
    
    // 模拟BLE接收到的开关控制命令
    uint8_t switch_cmd[] = {
        0x51, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF
    };
    
    // 调用协议处理函数
    int result = handle_protocol_data(switch_cmd, sizeof(switch_cmd));
    if (result == 0) {
        ESP_LOGI(TAG, "开关控制命令解析成功");
    } else {
        ESP_LOGE(TAG, "开关控制命令解析失败");
    }
}

/**
 * @brief 测试电流序列命令解析
 */
void test_current_seq_parsing(void) {
    ESP_LOGI(TAG, "=== 测试电流序列命令解析 ===");
    
    // 模拟BLE接收到的电流序列命令
    uint8_t current_cmd[] = {
        0x55, 0x03, 0x80, 0x07, 0xC0, 0x32, 0x28, 0x92, 0x4B, 0x64,
        0x28, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF
    };
    
    // 调用协议处理函数
    int result = handle_protocol_data(current_cmd, sizeof(current_cmd));
    if (result == 0) {
        ESP_LOGI(TAG, "电流序列命令解析成功");
    } else {
        ESP_LOGE(TAG, "电流序列命令解析失败");
    }
}

/**
 * @brief 测试设备信息查询命令解析
 */
void test_device_info_parsing(void) {
    ESP_LOGI(TAG, "=== 测试设备信息查询命令解析 ===");
    
    // 模拟BLE接收到的设备信息查询命令
    uint8_t device_info_cmd[] = {
        0x52, 0x00, 0x0A, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF
    };
    
    // 调用协议处理函数
    int result = handle_protocol_data(device_info_cmd, sizeof(device_info_cmd));
    if (result == 0) {
        ESP_LOGI(TAG, "设备信息查询命令解析成功");
    } else {
        ESP_LOGE(TAG, "设备信息查询命令解析失败");
    }
}

/**
 * @brief 测试设备状态查询命令解析
 */
void test_device_status_parsing(void) {
    ESP_LOGI(TAG, "=== 测试设备状态查询命令解析 ===");
    
    // 模拟BLE接收到的设备状态查询命令
    uint8_t device_status_cmd[] = {
        0x53, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF
    };
    
    // 调用协议处理函数
    int result = handle_protocol_data(device_status_cmd, sizeof(device_status_cmd));
    if (result == 0) {
        ESP_LOGI(TAG, "设备状态查询命令解析成功");
    } else {
        ESP_LOGE(TAG, "设备状态查询命令解析失败");
    }
}

/**
 * @brief 测试生物数据上报命令解析
 */
void test_bio_data_parsing(void) {
    ESP_LOGI(TAG, "=== 测试生物数据上报命令解析 ===");
    
    // 模拟BLE接收到的生物数据上报命令
    uint8_t bio_data_cmd[] = {
        0x54, 0x09, 0xC4, 0x75, 0x30, 0x3A, 0x98, 0x46, 0x50, 0x00,
        0x96, 0xFA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF
    };
    
    // 调用协议处理函数
    int result = handle_protocol_data(bio_data_cmd, sizeof(bio_data_cmd));
    if (result == 0) {
        ESP_LOGI(TAG, "生物数据上报命令解析成功");
    } else {
        ESP_LOGE(TAG, "生物数据上报命令解析失败");
    }
}

/**
 * @brief 运行所有BLE协议测试
 */
void run_ble_protocol_tests(void) {
    ESP_LOGI(TAG, "开始BLE协议测试...");
    
    test_switch_control_parsing();
    test_current_seq_parsing();
    test_device_info_parsing();
    test_device_status_parsing();
    test_bio_data_parsing();
    
    ESP_LOGI(TAG, "所有BLE协议测试完成!");
}