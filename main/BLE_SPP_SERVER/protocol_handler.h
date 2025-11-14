/*
 * 协议处理头文件
 */

#ifndef _PROTOCOL_HANDLER_H_
#define _PROTOCOL_HANDLER_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// 生物数据结构体
typedef struct {
    uint16_t bia;           // 16bit: 0-5000 欧姆
    uint16_t gsr;           // 16bit: 0-65536 mV
    uint16_t emg1;          // 16bit: 0-65536 mV
    uint16_t emg2;          // 16bit: 0-65536 mV
    uint16_t hr;            // 9bit: 0-500 BPM
    uint16_t hrv;           // 10bit: 0-1000 ms
} bio_data_t;

// 电流序列结构体
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

// blufi配网结构体
typedef struct {
    uint8_t blufi_flag;           // 2bit: 0-4
    uint8_t low_freq_wave;  // 8bit: 0-255
    uint16_t low_freq;      // 10bit: 0-999
} blufi_req_t;

/**
 * @brief 编码开关控制命令
 * @param switch_state 开关状态 (0-关, 1-开)
 * @param out_frame 输出数据帧
 * @param out_len 输出数据长度
 * @return true-成功, false-失败
 */
bool encode_switch_control(uint8_t switch_state, uint8_t *out_frame, uint16_t *out_len);

/**
 * @brief 解码开关控制命令
 * @param data 输入数据
 * @param len 数据长度
 * @param out_switch_state 输出开关状态
 * @return true-成功, false-失败
 */
bool decode_switch_control(const uint8_t *data, uint16_t len, uint8_t *out_switch_state);

/**
 * @brief 编码设备信息命令
 * @param device_id 设备ID (A0001-A0100)
 * @param fw_version 固件版本
 * @param out_frame 输出数据帧
 * @param out_len 输出数据长度
 * @return true-成功, false-失败
 */
bool encode_device_info(uint32_t device_id, uint32_t fw_version, uint8_t *out_frame, uint16_t *out_len);

/**
 * @brief 解码设备信息命令
 * @param data 输入数据
 * @param len 数据长度
 * @param out_device_id 输出设备ID
 * @param out_fw_version 输出固件版本
 * @return true-成功, false-失败
 */
bool decode_device_info(const uint8_t *data, uint16_t len, uint32_t *out_device_id, uint32_t *out_fw_version);

/**
 * @brief 编码设备状态命令
 * @param battery 电量 (0-100%)
 * @param out_frame 输出数据帧
 * @param out_len 输出数据长度
 * @return true-成功, false-失败
 */
bool encode_device_status(uint8_t battery, uint8_t *out_frame, uint16_t *out_len);

/**
 * @brief 解码设备状态命令
 * @param data 输入数据
 * @param len 数据长度
 * @param out_battery 输出电量
 * @return true-成功, false-失败
 */
bool decode_device_status(const uint8_t *data, uint16_t len, uint8_t *out_battery);

/**
 * @brief 编码生物数据命令
 * @param bio 生物数据结构体
 * @param out_frame 输出数据帧
 * @param out_len 输出数据长度
 * @return true-成功, false-失败
 */
bool encode_bio_data(const bio_data_t *bio, uint8_t *out_frame, uint16_t *out_len);

/**
 * @brief 解码生物数据命令
 * @param data 输入数据
 * @param len 数据长度
 * @param out_bio 输出生物数据结构体
 * @return true-成功, false-失败
 */
bool decode_bio_data(const uint8_t *data, uint16_t len, bio_data_t *out_bio);

/**
 * @brief 编码电流序列命令
 * @param current 电流序列结构体
 * @param out_frame 输出数据帧
 * @param out_len 输出数据长度
 * @return true-成功, false-失败
 */
bool encode_current_seq(const current_seq_t *current, uint8_t *out_frame, uint16_t *out_len);

/**
 * @brief 解码电流序列命令
 * @param data 输入数据
 * @param len 数据长度
 * @param out_current 输出电流序列结构体
 * @return true-成功, false-失败
 */
bool decode_current_seq(const uint8_t *data, uint16_t len, current_seq_t *out_current);


/**
 * @brief 解码电流序列命令
 * @param data 输入数据
 * @param len 数据长度
 * @param out_req 输出电流序列结构体
 * @return true-成功, false-失败
 */

bool decode_start_blufi_wifi(const uint8_t *data, uint16_t len, blufi_req_t *out_req);

#ifdef __cplusplus
}
#endif

#endif /* _PROTOCOL_HANDLER_H_ */