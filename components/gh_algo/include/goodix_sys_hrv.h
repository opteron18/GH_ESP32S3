/*
* HRVLib.h
*/

#ifndef _GOODIX_SYS_HRV_LIB_H_
#define _GOODIX_SYS_HRV_LIB_H_

#include <stdint.h>
#include "goodix_type.h"
#include "iot_sys_def.h"
#if DEBUG_HRV
#include "..\..\HRVTest\goodix_debug.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* 返回错误码 */
typedef enum {
    GX_HRV_ALGO_OK              = 0x00000000, // 成功返回，或无出值刷新
    GX_HRV_ALGO_FAILED          = 0x10000001, // 失败返回
    GX_HRV_ALGO_NULL_PTR        = 0x10000002, // 外部传入的指针为空
    GX_HRV_ALGO_INVALID_PARAM   = 0x10000003, // 参数范围无效
    GX_HRV_ALGO_OUT_OF_MEM      = 0x10000004, // 内存分配失败
    GX_HRV_ALGO_DEINIT_ABORT    = 0x10000005, // 释放失败
    GX_HRV_ALGO_UPDATE          = 0x10000006, // 有出值刷新
}goodix_hrv_ret;

#define PPG_CHANNEL_NUM		            4
#define RRI_NUM                         4
#define ACC_THR_NUM                     4

#define HRV_INTERFACE_VERSION "pv_v1.0.0"
#define HRV_INTERFACE_VERSION_LEN_MAX (20)

typedef struct {
    int32_t need_ipl;                                   // 是否需要插值到1KHz
    int32_t fs;                                         // 采样率
    int32_t acc_thr[ACC_THR_NUM];                       // acc 相关阈值，默认值为：20/10/3/3
}goodix_hrv_config;

typedef struct {
	uint8_t bit_num;                       //数据位数
    int32_t *ppg_rawdata;                 // PPG原始信号，最多四通道
    int32_t *cur_adj_flg;                 // 电流调光标志位
    int32_t *gain_adj_flg;                // 增益调光标志位
	int32_t acc_x;                        // 加速计 x 轴
    int32_t acc_y;                        // 加速计 y 轴
    int32_t acc_z;                       // 加速计 z 轴
    int32_t frame_id;                    // 帧序号
    int32_t hr;                           // 当前心率值
}goodix_hrv_input_rawdata;

typedef struct {
    int32_t rri[RRI_NUM];                               // RRI结果数组
    int32_t rri_confidence;                             // 结果置信度
	int32_t rri_valid_num;                              // RRI有效结果数量
}goodix_hrv_result;

IOT_SYS_DLL_API uint8_t *goodix_hrv_version(void);

IOT_SYS_DLL_API goodix_hrv_ret goodix_hrv_init(const void *cfg_instance, uint32_t cfg_size, const char *interface_ver);

IOT_SYS_DLL_API goodix_hrv_ret goodix_hrv_calc(goodix_hrv_input_rawdata *input_data, goodix_hrv_result *output_data);

IOT_SYS_DLL_API goodix_hrv_ret goodix_hrv_deinit(void);

/**
* @brief 获取hrv的默认配置参数
* @return     hrv的默认配置参数
*/
IOT_SYS_DLL_API const void *goodix_hrv_config_get_arr(void);

/**
* @brief 获取hrv的配置size
* @return     hrv的配置size
*/
IOT_SYS_DLL_API int32_t  goodix_hrv_config_get_size(void);

/**
* @brief 获取HRV配置文件的版本号
* @param[in/out]  ver       版本号保存数组
* @param[in] 	  ver_len   版本号长度，建议使用HRV_INTERFACE_VERSION_LEN_MAX
* @return     	  无
*/
IOT_SYS_DLL_API void goodix_hrv_config_get_version(char* ver, uint8_t ver_len);


#if DEBUG_HRV
IOT_SYS_DLL_API goodix_hrv_debug *copy_debug_addr();
#endif
#ifdef __cplusplus
}
#endif

#endif // _GOODIX_SYS_HRV_LIB_H_
