/*
 * BLE协议集成示例
 * 展示如何在现有BLE SPP服务器中集成协议处理
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "esp_log.h"
#include "protocol_handler.h"
#include "ble_spp_server_demo.h"
#include "esp_gatts_api.h"

#define TAG "BLE_PROTOCOL_INTEGRATION"

// 外部引用BLE连接相关信息
extern uint16_t spp_conn_id;
extern esp_gatt_if_t spp_gatts_if;
extern uint16_t spp_handle_table[SPP_IDX_NB];

int handle_protocol_data(const uint8_t *data, uint16_t len);

/**
 * @brief 处理BLE写入事件的数据
 * @param prep_req 写入请求参数
 * @return 处理结果 (0-成功, -1-失败)
 */
int handle_ble_write_event(esp_ble_gatts_cb_param_t *prep_req) {
    if (!prep_req) {
        ESP_LOGE(TAG, "无效参数");
        return -1;
    }
    
    uint16_t handle = prep_req->write.handle;
    uint16_t len = prep_req->write.len;
    uint8_t *data = prep_req->write.value;
    
    // 检查是否为SPP数据写入特征值
    if (handle == spp_handle_table[SPP_IDX_SPP_DATA_RECV_VAL]) {
        ESP_LOGI(TAG, "接收到BLE数据: 长度=%d", len);
        // 调用协议处理函数
        return handle_protocol_data(data, len);
    }
    
    ESP_LOGW(TAG, "未知写入句柄: 0x%04X", handle);
    return -1;
}



/**
 * @brief 发送数据到BLE客户端
 * @param data 要发送的数据
 * @param len 数据长度
 * @return 发送结果 (0-成功, -1-失败)
 */
int send_data_to_client(const uint8_t *data, uint16_t len) {
    if (!data || len == 0) {
        ESP_LOGE(TAG, "无效数据");
        return -1;
    }
    
    // 检查BLE连接状态
    if (spp_gatts_if == 0xff || spp_conn_id == 0xffff) {
        ESP_LOGE(TAG, "BLE未连接");
        return -1;
    }
    
    // 分包发送数据（考虑MTU限制）
    uint16_t mtu_size = 512; // 默认MTU大小
    uint16_t sent_len = 0;
    
    while (sent_len < len) {
        uint16_t chunk_size = (len - sent_len) > (mtu_size - 3) ? (mtu_size - 3) : (len - sent_len);
        esp_err_t ret = esp_ble_gatts_send_indicate(
            spp_gatts_if, 
            spp_conn_id, 
            spp_handle_table[SPP_IDX_SPP_DATA_NTY_VAL], 
            chunk_size, 
            (uint8_t *)(data + sent_len), 
            false
        );
        
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "发送数据失败: %s", esp_err_to_name(ret));
            return -1;
        }
        
        sent_len += chunk_size;
        ESP_LOGI(TAG, "已发送 %d/%d 字节", sent_len, len);
    }
    
    ESP_LOGI(TAG, "数据发送成功: %d 字节", len);
    return 0;
}

/**
 * @brief 处理接收到的协议数据
 * @param data 接收到的数据
 * @param len 数据长度
 * @return 处理结果
 */
int handle_protocol_data(const uint8_t *data, uint16_t len) {
    if (!data || len < 1) {
        ESP_LOGE(TAG, "无效数据");
        return -1;
    }
    
    uint8_t cmd = data[0];
    ESP_LOGI(TAG, "接收到命令: 0x%02X", cmd);
    
    switch (cmd) {
        case 0x51: { // 开关控制
            uint8_t switch_state;
            if (decode_switch_control(data, len, &switch_state)) {
                ESP_LOGI(TAG, "开关控制: %s", switch_state ? "开" : "关");
                // 这里可以添加实际的开关控制逻辑
                return 0;
            }
            break;
        }
        
        case 0x52: { // 设备信息查询
            uint32_t device_id;
            uint32_t fw_version;
            
            // 解码设备信息
            if (decode_device_info(data, len, &device_id, &fw_version)) {
                ESP_LOGI(TAG, "设备信息查询: ID=A%lu, 版本=%lu.%lu.%lu", 
                         device_id & 0xFFFF,
                         (fw_version >> 16) & 0xFF,
                         (fw_version >> 8) & 0xFF,
                         fw_version & 0xFF);
                
                // 构造响应数据
                uint8_t response[20];
                uint16_t response_len;
                
                if (encode_device_info(device_id, fw_version, response, &response_len)) {
                    ESP_LOGI(TAG, "发送设备信息响应");
                    // 这里可以通过BLE发送 response 数据
                    return 0;
                }
            } else {
                ESP_LOGE(TAG, "设备信息解码失败");
            }
            break;
        }
        
        case 0x53: { // 设备状态查询
            uint8_t battery;
            
            // 解码设备状态
            if (decode_device_status(data, len, &battery)) {
                ESP_LOGI(TAG, "设备状态查询: 电量=%d%%", battery);
                
                // 构造响应数据
                uint8_t response[19];
                uint16_t response_len;
                
                if (encode_device_status(battery, response, &response_len)) {
                    ESP_LOGI(TAG, "发送设备状态响应");
                    // 这里可以通过BLE发送 response 数据
                    return 0;
                }
            } else {
                ESP_LOGE(TAG, "设备状态解码失败");
            }
            break;
        }
        
        case 0x54: { // 生物数据上报
            bio_data_t bio;
            
            // 解码生物数据
            if (decode_bio_data(data, len, &bio)) {
                ESP_LOGI(TAG, "生物数据上报: BIA=%d, GSR=%d, EMG1=%d, EMG2=%d, HR=%d, HRV=%d",
                         bio.bia, bio.gsr, bio.emg1, bio.emg2, bio.hr, bio.hrv);
                
                // 构造响应数据
                uint8_t response[20];
                uint16_t response_len;
                
                if (encode_bio_data(&bio, response, &response_len)) {
                    ESP_LOGI(TAG, "发送生物数据响应");
                    // 这里可以通过BLE发送 response 数据
                    return 0;
                }
            } else {
                ESP_LOGE(TAG, "生物数据解码失败");
            }
            break;
        }
        
        case 0x55: { // 电流序列设置
            current_seq_t current;
            if (decode_current_seq(data, len, &current)) {
                ESP_LOGI(TAG, "电流序列设置成功");
                // 这里可以添加实际的电流控制逻辑
                return 0;
            }
            break;
        }

        case 0x56: { // 电流序列设置
            ESP_LOGI(TAG, "正在处理0x56");
            blufi_req_t out_req;
            if (decode_start_blufi_wifi(data, len, &out_req)) {
                ESP_LOGI(TAG, "电流序列设置成功");
                // 这里可以添加实际的电流控制逻辑
                return 0;
            }
            break;
        }
        
        default:
            ESP_LOGE(TAG, "未知命令: 0x%02X", cmd);
            return -1;
    }
    
    ESP_LOGE(TAG, "命令处理失败");
    return -1;
}

/**
 * @brief 主动上报设备状态
 * 可以定时调用此函数上报设备状态
 */
void report_device_status(void) {
    ESP_LOGI(TAG, "=== 主动上报设备状态 ===");
    
    // 上报设备信息
    uint8_t info_frame[20];
    uint16_t info_len;
    encode_device_info(0xA0001, 0x00010000, info_frame, &info_len);
    if (send_data_to_client(info_frame, info_len) == 0) {
        ESP_LOGI(TAG, "设备信息已发送到客户端");
    } else {
        ESP_LOGE(TAG, "设备信息发送失败");
    }
    
    // 上报设备状态
    uint8_t status_frame[19];
    uint16_t status_len;
    encode_device_status(85, status_frame, &status_len);
    if (send_data_to_client(status_frame, status_len) == 0) {
        ESP_LOGI(TAG, "设备状态已发送到客户端");
    } else {
        ESP_LOGE(TAG, "设备状态发送失败");
    }
    
    // 上报生物数据
    bio_data_t bio = {
        .bia = 2500, .gsr = 30000, .emg1 = 15000, 
        .emg2 = 18000, .hr = 75, .hrv = 250
    };
    uint8_t bio_frame[19];
    uint16_t bio_len;
    encode_bio_data(&bio, bio_frame, &bio_len);
    if (send_data_to_client(bio_frame, bio_len) == 0) {
        ESP_LOGI(TAG, "生物数据已发送到客户端");
    } else {
        ESP_LOGE(TAG, "生物数据发送失败");
    }
}

/**
 * @brief 处理小程序控制命令
 * @param cmd_type 命令类型
 * @param param 参数
 */
void handle_control_command(uint8_t cmd_type, uint32_t param) {
    ESP_LOGI(TAG, "处理控制命令: 类型=0x%02X, 参数=%lu", cmd_type, param);
    
    switch (cmd_type) {
        case 0x01: { // 开关控制
            uint8_t frame[20];
            uint16_t len;
            encode_switch_control(param ? 1 : 0, frame, &len);
            ESP_LOGI(TAG, "开关控制命令已生成");
            break;
        }
        
        case 0x02: { // 电流序列设置
            current_seq_t current = {
                .mode = 2,
                .low_freq_wave = 100,
                .low_freq = 300,
                .mid_freq_wave = 3,
                .mid_carrier = 40,
                .mid_modulation = 2,
                .action_wave = 1,
                .action_freq = 20,
                .intensity = 60,
                .duty_cycle = 25,
                .rise_time = 8,
                .pattern = 512
            };
            
            uint8_t frame[20];
            uint16_t len;
            encode_current_seq(&current, frame, &len);
            ESP_LOGI(TAG, "电流序列设置命令已生成");
            break;
        }
        
        default:
            ESP_LOGW(TAG, "未知控制命令类型: 0x%02X", cmd_type);
            break;
    }
}

/**
 * @brief 演示BLE协议集成
 */
void demo_ble_protocol_integration(void) {
    ESP_LOGI(TAG, "=== BLE协议集成演示 ===");
    
    // 演示1: 处理接收到的协议数据
    ESP_LOGI(TAG, "演示1: 处理接收数据");
    
    // 模拟接收开关控制命令
    uint8_t test_cmd1[] = {
        0x51, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF
    };
    handle_protocol_data(test_cmd1, sizeof(test_cmd1));
    
    // 模拟接收电流序列设置命令
    uint8_t test_cmd2[] = {
        0x55, 0x03, 0x80, 0x07, 0xC0, 0x32, 0x28, 0x92, 0x4B, 0x64,
        0x28, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF
    };
    handle_protocol_data(test_cmd2, sizeof(test_cmd2));
    
    // 演示2: 主动上报
    ESP_LOGI(TAG, "演示2: 主动上报");
    report_device_status();
    
    // 演示3: 处理控制命令
    ESP_LOGI(TAG, "演示3: 处理控制命令");
    handle_control_command(0x01, 1); // 开关控制
    handle_control_command(0x02, 0); // 电流序列设置
    
    ESP_LOGI(TAG, "BLE协议集成演示完成");
}