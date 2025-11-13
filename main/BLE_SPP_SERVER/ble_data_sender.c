/*
 * BLE数据发送器
 * 提供便捷的函数来发送各种编码后的协议数据
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "esp_log.h"
#include "protocol_handler.h"
#include "ble_protocol_integration.h"

#define TAG "BLE_DATA_SENDER"

/**
 * @brief 发送开关控制命令到小程序
 * @param switch_state 开关状态 (0-关闭, 1-开启)
 * @return 发送结果 (0-成功, -1-失败)
 */
int send_switch_control_to_miniapp(uint8_t switch_state) {
    uint8_t frame[20];
    uint16_t len;
    
    if (!encode_switch_control(switch_state, frame, &len)) {
        ESP_LOGE(TAG, "编码开关控制命令失败");
        return -1;
    }
    
    return send_data_to_client(frame, len);
}

/**
 * @brief 发送设备信息到小程序
 * @param device_id 设备ID
 * @param fw_version 固件版本
 * @return 发送结果 (0-成功, -1-失败)
 */
int send_device_info_to_miniapp(uint32_t device_id, uint32_t fw_version) {
    uint8_t frame[20];
    uint16_t len;
    
    if (!encode_device_info(device_id, fw_version, frame, &len)) {
        ESP_LOGE(TAG, "编码设备信息失败");
        return -1;
    }
    
    return send_data_to_client(frame, len);
}

/**
 * @brief 发送设备状态到小程序
 * @param battery 电量百分比 (0-100)
 * @return 发送结果 (0-成功, -1-失败)
 */
int send_device_status_to_miniapp(uint8_t battery) {
    uint8_t frame[20];
    uint16_t len;
    
    if (!encode_device_status(battery, frame, &len)) {
        ESP_LOGE(TAG, "编码设备状态失败");
        return -1;
    }
    
    return send_data_to_client(frame, len);
}

/**
 * @brief 发送生物数据到小程序
 * @param bio 生物数据结构体指针
 * @return 发送结果 (0-成功, -1-失败)
 */
int send_bio_data_to_miniapp(const bio_data_t *bio) {
    uint8_t frame[20];
    uint16_t len;
    
    if (!encode_bio_data(bio, frame, &len)) {
        ESP_LOGE(TAG, "编码生物数据失败");
        return -1;
    }
    
    return send_data_to_client(frame, len);
}

/**
 * @brief 发送电流序列到小程序
 * @param current 电流序列结构体指针
 * @return 发送结果 (0-成功, -1-失败)
 */
int send_current_seq_to_miniapp(const current_seq_t *current) {
    uint8_t frame[20];
    uint16_t len;
    
    if (!encode_current_seq(current, frame, &len)) {
        ESP_LOGE(TAG, "编码电流序列失败");
        return -1;
    }
    
    return send_data_to_client(frame, len);
}