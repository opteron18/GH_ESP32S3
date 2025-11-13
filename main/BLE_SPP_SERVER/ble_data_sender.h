/*
 * BLE数据发送器头文件
 */

#ifndef _BLE_DATA_SENDER_H_
#define _BLE_DATA_SENDER_H_

#include <stdint.h>
#include "protocol_handler.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 发送开关控制命令到小程序
 * @param switch_state 开关状态 (0-关闭, 1-开启)
 * @return 发送结果 (0-成功, -1-失败)
 */
int send_switch_control_to_miniapp(uint8_t switch_state);

/**
 * @brief 发送设备信息到小程序
 * @param device_id 设备ID
 * @param fw_version 固件版本
 * @return 发送结果 (0-成功, -1-失败)
 */
int send_device_info_to_miniapp(uint32_t device_id, uint32_t fw_version);

/**
 * @brief 发送设备状态到小程序
 * @param battery 电量百分比 (0-100)
 * @return 发送结果 (0-成功, -1-失败)
 */
int send_device_status_to_miniapp(uint8_t battery);

/**
 * @brief 发送生物数据到小程序
 * @param bio 生物数据结构体指针
 * @return 发送结果 (0-成功, -1-失败)
 */
int send_bio_data_to_miniapp(const bio_data_t *bio);

/**
 * @brief 发送电流序列到小程序
 * @param current 电流序列结构体指针
 * @return 发送结果 (0-成功, -1-失败)
 */
int send_current_seq_to_miniapp(const current_seq_t *current);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_DATA_SENDER_H_ */