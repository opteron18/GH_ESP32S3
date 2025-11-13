/*
 * BLE协议集成头文件
 */

#ifndef _BLE_PROTOCOL_INTEGRATION_H_
#define _BLE_PROTOCOL_INTEGRATION_H_

#include <stdint.h>
#include "esp_gatts_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 发送数据到BLE客户端
 * @param data 要发送的数据
 * @param len 数据长度
 * @return 发送结果 (0-成功, -1-失败)
 */
int send_data_to_client(const uint8_t *data, uint16_t len);

/**
 * @brief 处理接收到的协议数据
 * @param data 接收到的数据
 * @param len 数据长度
 * @return 处理结果 (0-成功, -1-失败)
 */
int handle_protocol_data(const uint8_t *data, uint16_t len);

/**
 * @brief 处理BLE写入事件的数据
 * @param prep_req 写入请求参数
 * @return 处理结果 (0-成功, -1-失败)
 */
int handle_ble_write_event(esp_ble_gatts_cb_param_t *prep_req);

/**
 * @brief 主动上报设备状态
 * 可以定时调用此函数上报设备状态
 */
void report_device_status(void);

/**
 * @brief 处理小程序控制命令
 * @param cmd_type 命令类型
 * @param param 参数
 */
void handle_control_command(uint8_t cmd_type, uint32_t param);

/**
 * @brief 演示BLE协议集成
 */
void demo_ble_protocol_integration(void);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_PROTOCOL_INTEGRATION_H_ */