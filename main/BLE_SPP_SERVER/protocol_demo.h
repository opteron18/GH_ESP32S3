/*
 * 协议演示头文件
 */

#ifndef _PROTOCOL_DEMO_H_
#define _PROTOCOL_DEMO_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 运行协议演示
 */
void run_protocol_demo(void);

/**
 * @brief 演示开关控制命令
 */
void demo_switch_control(void);

/**
 * @brief 演示设备信息命令
 */
void demo_device_info(void);

/**
 * @brief 演示设备状态命令
 */
void demo_device_status(void);

/**
 * @brief 演示生物数据命令
 */
void demo_bio_data(void);

/**
 * @brief 演示电流序列命令
 */
void demo_current_seq(void);

/**
 * @brief 演示BLE通信场景
 */
void demo_ble_communication(void);

#ifdef __cplusplus
}
#endif

#endif /* _PROTOCOL_DEMO_H_ */