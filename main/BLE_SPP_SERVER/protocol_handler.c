/*
 * 协议处理函数
 * 用于处理0x51-0x55命令的实际收发
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "esp_log.h"
#include "protocol_handler.h"

#define TAG "PROTOCOL_HANDLER"

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

// 位操作宏定义
#define EXTRACT_BITS(data, start, len) (((data) >> (start)) & ((1 << (len)) - 1))
#define SET_BITS(dest, start, len, value) do { \
    (dest) &= ~(((1 << (len)) - 1) << (start)); \
    (dest) |= ((value) & ((1 << (len)) - 1)) << (start); \
} while(0)

// 按位写入/读取工具（按字节MSB优先）
static void write_bits(uint8_t *buffer, uint16_t bit_offset, uint8_t bit_length, uint32_t value) {
	for (uint8_t i = 0; i < bit_length; i++) {
		uint16_t bit_pos = bit_offset + i;
		uint16_t byte_index = bit_pos / 8;
		uint8_t bit_in_byte = 7 - (bit_pos % 8);
		uint8_t bit_val = (value >> (bit_length - 1 - i)) & 0x01;
		if (bit_val) {
			buffer[byte_index] |= (1u << bit_in_byte);
		} else {
			buffer[byte_index] &= ~(1u << bit_in_byte);
		}
	}
}

static uint32_t read_bits(const uint8_t *buffer, uint16_t bit_offset, uint8_t bit_length) {
	uint32_t result = 0;
	for (uint8_t i = 0; i < bit_length; i++) {
		uint16_t bit_pos = bit_offset + i;
		uint16_t byte_index = bit_pos / 8;
		uint8_t bit_in_byte = 7 - (bit_pos % 8);
		uint8_t bit_val = (buffer[byte_index] >> bit_in_byte) & 0x01;
		result = (result << 1) | bit_val;
	}
	return result;
}

// 打印十六进制数据
static void print_hex(const char *prefix, const uint8_t *data, uint16_t len) {
    printf("%s: ", prefix);
    for (int i = 0; i < len; i++) {
        printf("%02X ", data[i]);
    }
    printf("\n");
}

/**
 * @brief 编码开关控制命令
 * @param switch_state 开关状态 (0-关, 1-开)
 * @param out_frame 输出数据帧
 * @param out_len 输出数据长度
 * @return true-成功, false-失败
 */
bool encode_switch_control(uint8_t switch_state, uint8_t *out_frame, uint16_t *out_len) {
    if (!out_frame || !out_len) return false;
    
    memset(out_frame, 0, 20);
    
    // 帧头
    out_frame[0] = CMD_SWITCH_CONTROL;
    
    // 开关状态 (bit 8)
    if (switch_state) {
        out_frame[1] |= 0x01;
    }
    
    // 帧尾
    out_frame[19] = FRAME_TAIL;
    
    *out_len = 20;
    
    ESP_LOGI(TAG, "编码开关控制: 状态=%s", switch_state ? "开" : "关");
    print_hex("发送数据", out_frame, *out_len);
    
    return true;
}

/**
 * @brief 解码开关控制命令
 * @param data 输入数据
 * @param len 数据长度
 * @param out_switch_state 输出开关状态
 * @return true-成功, false-失败
 */
bool decode_switch_control(const uint8_t *data, uint16_t len, uint8_t *out_switch_state) {
    if (!data || !out_switch_state || len < 20) return false;
    
    if (data[0] != CMD_SWITCH_CONTROL || data[19] != FRAME_TAIL) {
        ESP_LOGE(TAG, "开关控制帧格式错误");
        return false;
    }
    
    *out_switch_state = data[1] & 0x01;
    
    print_hex("接收数据", data, len);
    ESP_LOGI(TAG, "解码开关控制: 状态=%s", *out_switch_state ? "开" : "关");
    
    return true;
}

/**
 * @brief 编码设备信息命令
 * @param device_id 设备ID (A0001-A0100)
 * @param fw_version 固件版本
 * @param out_frame 输出数据帧
 * @param out_len 输出数据长度
 * @return true-成功, false-失败
 */
bool encode_device_info(uint32_t device_id, uint32_t fw_version, uint8_t *out_frame, uint16_t *out_len) {
    if (!out_frame || !out_len) return false;
    
    memset(out_frame, 0, 20);
    
    // 帧头
    out_frame[0] = CMD_DEVICE_INFO;
    
    // 设备ID (bits 8-39)
    out_frame[1] = (device_id >> 24) & 0xFF;
    out_frame[2] = (device_id >> 16) & 0xFF;
    out_frame[3] = (device_id >> 8) & 0xFF;
    out_frame[4] = device_id & 0xFF;
    
    // 固件版本 (bits 40-71)
    out_frame[5] = (fw_version >> 24) & 0xFF;
    out_frame[6] = (fw_version >> 16) & 0xFF;
    out_frame[7] = (fw_version >> 8) & 0xFF;
    out_frame[8] = fw_version & 0xFF;
    
    // 帧尾
    out_frame[19] = FRAME_TAIL;
    
    *out_len = 20;
    
    ESP_LOGI(TAG, "编码设备信息: ID=A%04lu, 版本=0.%lu.%lu", 
             device_id & 0xFFFF,
             (fw_version >> 16) & 0xFF,
             (fw_version >> 8) & 0xFF
            //  fw_version & 0xFF);
            );
    print_hex("发送数据", out_frame, *out_len);
    
    return true;
}

/**
 * @brief 解码设备信息命令
 * @param data 输入数据
 * @param len 数据长度
 * @param out_device_id 输出设备ID
 * @param out_fw_version 输出固件版本
 * @return true-成功, false-失败
 */
bool decode_device_info(const uint8_t *data, uint16_t len, uint32_t *out_device_id, uint32_t *out_fw_version) {
    if (!data || !out_device_id || !out_fw_version || len < 20) return false;
    
    if (data[0] != CMD_DEVICE_INFO || data[19] != FRAME_TAIL) {
        ESP_LOGE(TAG, "设备信息帧格式错误");
        return false;
    }
    
    *out_device_id = ((uint32_t)data[1] << 24) | ((uint32_t)data[2] << 16) | 
                     ((uint32_t)data[3] << 8) | data[4];
    
    *out_fw_version = ((uint32_t)data[5] << 24) | ((uint32_t)data[6] << 16) | 
                       ((uint32_t)data[7] << 8) | data[8];
    
    print_hex("接收数据", data, len);
    ESP_LOGI(TAG, "解码设备信息: ID=A%lu, 版本=%lu.%lu.%lu", 
             *out_device_id & 0xFFFF,
             (*out_fw_version >> 16) & 0xFF,
             (*out_fw_version >> 8) & 0xFF,
             *out_fw_version & 0xFF);
    
    return true;
}

/**
 * @brief 编码设备状态命令
 * @param battery 电量 (0-100%)
 * @param out_frame 输出数据帧
 * @param out_len 输出数据长度
 * @return true-成功, false-失败
 */
bool encode_device_status(uint8_t battery, uint8_t *out_frame, uint16_t *out_len) {
    if (!out_frame || !out_len) return false;
    
    memset(out_frame, 0, 20); // 19字节帧
    
    // 帧头
    out_frame[0] = CMD_DEVICE_STATUS;
    
    // 电量 (bits 8-15)
    out_frame[1] = battery;
    
    // 帧尾
    out_frame[19] = FRAME_TAIL;
    
    *out_len = 20;
    
    ESP_LOGI(TAG, "编码设备状态: 电量=%d%%", battery);
    print_hex("发送数据", out_frame, *out_len);
    
    return true;
}

/**
 * @brief 解码设备状态命令
 * @param data 输入数据
 * @param len 数据长度
 * @param out_battery 输出电量
 * @return true-成功, false-失败
 */
bool decode_device_status(const uint8_t *data, uint16_t len, uint8_t *out_battery) {
    if (!data || !out_battery || len < 19) return false;
    
    if (data[0] != CMD_DEVICE_STATUS || data[18] != FRAME_TAIL) {
        ESP_LOGE(TAG, "设备状态帧格式错误");
        return false;
    }
    
    *out_battery = data[1];
    
    print_hex("接收数据", data, len);
    ESP_LOGI(TAG, "解码设备状态: 电量=%d%%", *out_battery);
    
    return true;
}

/**
 * @brief 编码生物数据命令
 * @param bio 生物数据结构体
 * @param out_frame 输出数据帧
 * @param out_len 输出数据长度
 * @return true-成功, false-失败
 */
bool encode_bio_data(const bio_data_t *bio, uint8_t *out_frame, uint16_t *out_len) {
    if (!bio || !out_frame || !out_len) return false;
    
    memset(out_frame, 0, 20);
    
    // 帧头
    out_frame[0] = CMD_BIO_DATA;
    
    // BIA (bits 8-23)
    out_frame[1] = (bio->bia >> 8) & 0xFF;
    out_frame[2] = bio->bia & 0xFF;
    
    // GSR (bits 24-39)
    out_frame[3] = (bio->gsr >> 8) & 0xFF;
    out_frame[4] = bio->gsr & 0xFF;
    
    // EMG1 (bits 40-55)
    out_frame[5] = (bio->emg1 >> 8) & 0xFF;
    out_frame[6] = bio->emg1 & 0xFF;
    
    // EMG2 (bits 56-71)
    out_frame[7] = (bio->emg2 >> 8) & 0xFF;
    out_frame[8] = bio->emg2 & 0xFF;
    
    // HR (bits 72-80, 9bit)
    out_frame[9] = (bio->hr >> 1) & 0xFF;
    out_frame[10] = (out_frame[10] & 0x7F) | ((bio->hr & 0x01) << 7);
    
    // HRV (bits 81-90, 10bit)
    out_frame[10] = (out_frame[10] & 0x80) | ((bio->hrv >> 3) & 0x7F);
    out_frame[11] = ((bio->hrv & 0x07) << 5);
    
    // 帧尾
    out_frame[19] = FRAME_TAIL;
    
    *out_len = 20;
    
    ESP_LOGI(TAG, "编码生物数据: BIA=%d, GSR=%d, EMG1=%d, EMG2=%d, HR=%d, HRV=%d",
             bio->bia, bio->gsr, bio->emg1, bio->emg2, bio->hr, bio->hrv);
    print_hex("发送数据", out_frame, *out_len);
    
    return true;
}

/**
 * @brief 解码生物数据命令
 * @param data 输入数据
 * @param len 数据长度
 * @param out_bio 输出生物数据结构体
 * @return true-成功, false-失败
 */
bool decode_bio_data(const uint8_t *data, uint16_t len, bio_data_t *out_bio) {
    if (!data || !out_bio || len < 19) return false;
    
    if (data[0] != CMD_BIO_DATA || data[18] != FRAME_TAIL) {
        ESP_LOGE(TAG, "生物数据帧格式错误");
        return false;
    }
    
    out_bio->bia = ((uint16_t)data[1] << 8) | data[2];
    out_bio->gsr = ((uint16_t)data[3] << 8) | data[4];
    out_bio->emg1 = ((uint16_t)data[5] << 8) | data[6];
    out_bio->emg2 = ((uint16_t)data[7] << 8) | data[8];
    out_bio->hr = ((uint16_t)data[9] << 1) | ((data[10] >> 7) & 0x01);
    out_bio->hrv = ((uint16_t)((data[10] & 0x7F) << 3) | (data[11] >> 5));
    
    print_hex("接收数据", data, len);
    ESP_LOGI(TAG, "解码生物数据: BIA=%d, GSR=%d, EMG1=%d, EMG2=%d, HR=%d, HRV=%d",
             out_bio->bia, out_bio->gsr, out_bio->emg1, out_bio->emg2, out_bio->hr, out_bio->hrv);
    
    return true;
}

/**
 * @brief 编码电流序列命令
 * @param current 电流序列结构体
 * @param out_frame 输出数据帧
 * @param out_len 输出数据长度
 * @return true-成功, false-失败
 */
bool encode_current_seq(const current_seq_t *current, uint8_t *out_frame, uint16_t *out_len) {
    if (!current || !out_frame || !out_len) return false;
    
    memset(out_frame, 0, 20);
    
    // 帧头
    out_frame[0] = CMD_CURRENT_SEQ;
    
    // 从bit 8 开始按表连续打包（72-151 预留清零，152-159 包尾）
    uint16_t bit_offset = 8;
    write_bits(out_frame, bit_offset, 2, current->mode & 0x03);           bit_offset += 2;  // 8-9
    write_bits(out_frame, bit_offset, 8, current->low_freq_wave);         bit_offset += 8;  // 10-17
    write_bits(out_frame, bit_offset, 10, current->low_freq & 0x3FF);     bit_offset += 10; // 18-27
    write_bits(out_frame, bit_offset, 4, current->mid_freq_wave & 0x0F);  bit_offset += 4;  // 28-31
    write_bits(out_frame, bit_offset, 7, current->mid_carrier & 0x7F);    bit_offset += 7;  // 32-38
    write_bits(out_frame, bit_offset, 4, current->mid_modulation & 0x0F); bit_offset += 4;  // 39-42
    write_bits(out_frame, bit_offset, 4, current->action_wave & 0x0F);    bit_offset += 4;  // 43-46
    write_bits(out_frame, bit_offset, 6, current->action_freq & 0x3F);    bit_offset += 6;  // 47-52
    write_bits(out_frame, bit_offset, 7, current->intensity & 0x7F);      bit_offset += 7;  // 53-59
    write_bits(out_frame, bit_offset, 7, current->duty_cycle & 0x3F);     bit_offset += 7;  // 60-65
    write_bits(out_frame, bit_offset, 6, current->rise_time & 0x3F);      bit_offset += 6;  // 66-71
    // 72-151 预留：保持0（memset 已清零）
    
    // 帧尾
    out_frame[19] = FRAME_TAIL;
    
    *out_len = 20;
    
    ESP_LOGI(TAG, "编码电流序列: 模式=%d, 低频=%d/%d, 中频=%d/%d/%d, 动作=%d/%d, 强度=%d, 占空比=%d%%, 渐强=%d, 模式扩展=%d",
             current->mode, current->low_freq_wave, current->low_freq,
             current->mid_freq_wave, current->mid_carrier, current->mid_modulation,
             current->action_wave, current->action_freq,
             current->intensity, current->duty_cycle, current->rise_time, current->pattern);
    print_hex("发送数据", out_frame, *out_len);
    
    return true;
}

/**
 * @brief 解码电流序列命令
 * @param data 输入数据
 * @param len 数据长度
 * @param out_current 输出电流序列结构体
 * @return true-成功, false-失败
 */
// 557FFE7AC95559327800000000000000000000ff 
bool decode_current_seq(const uint8_t *data, uint16_t len, current_seq_t *out_current) {
    if (!data || !out_current || len < 20) return false;
    
    if (data[0] != CMD_CURRENT_SEQ || data[19] != FRAME_TAIL) {
        ESP_LOGE(TAG, "电流序列帧格式错误");
        return false;
    }
    
    // 与编码保持一致的按位解包
    uint16_t bit_offset = 8;
    out_current->mode = (uint8_t)read_bits(data, bit_offset, 2);           bit_offset += 2;
    out_current->low_freq_wave = (uint8_t)read_bits(data, bit_offset, 8);  bit_offset += 8;
    out_current->low_freq = (uint16_t)read_bits(data, bit_offset, 10);     bit_offset += 10;
    out_current->mid_freq_wave = (uint8_t)read_bits(data, bit_offset, 4);  bit_offset += 4;
    out_current->mid_carrier = (uint8_t)read_bits(data, bit_offset, 7);    bit_offset += 7;
    out_current->mid_modulation = (uint8_t)read_bits(data, bit_offset, 4); bit_offset += 4;
    out_current->action_wave = (uint8_t)read_bits(data, bit_offset, 4);    bit_offset += 4;
    out_current->action_freq = (uint8_t)read_bits(data, bit_offset, 6);    bit_offset += 6;
    out_current->intensity = (uint8_t)read_bits(data, bit_offset, 7);      bit_offset += 7;
    out_current->duty_cycle = (uint8_t)read_bits(data, bit_offset, 7);     bit_offset += 7;
    out_current->rise_time = (uint8_t)read_bits(data, bit_offset, 6);      bit_offset += 6;
    // 72-151 预留，忽略
    out_current->pattern = 0;
    
    print_hex("接收数据", data, len);
    ESP_LOGI(TAG, "解码电流序列: 模式=%d, 低频=%d/%d, 中频=%d/%d/%d, 动作=%d/%d, 强度=%d, 占空比=%d%%, 渐强=%d, 模式扩展=%d",
             out_current->mode, out_current->low_freq_wave, out_current->low_freq,
             out_current->mid_freq_wave, out_current->mid_carrier, out_current->mid_modulation,
             out_current->action_wave, out_current->action_freq,
             out_current->intensity, out_current->duty_cycle, out_current->rise_time, out_current->pattern);
    
    return true;
}