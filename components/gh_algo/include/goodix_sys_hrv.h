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

/* ????????? */
typedef enum {
    GX_HRV_ALGO_OK              = 0x00000000, // ?????????????????
    GX_HRV_ALGO_FAILED          = 0x10000001, // ??????
    GX_HRV_ALGO_NULL_PTR        = 0x10000002, // ?????????????
    GX_HRV_ALGO_INVALID_PARAM   = 0x10000003, // ??????¦¶??§¹
    GX_HRV_ALGO_OUT_OF_MEM      = 0x10000004, // ?????????
    GX_HRV_ALGO_DEINIT_ABORT    = 0x10000005, // ??????
    GX_HRV_ALGO_UPDATE          = 0x10000006, // ?§Ô?????
}goodix_hrv_ret;

#define PPG_CHANNEL_NUM		            4
#define RRI_NUM                         4
#define ACC_THR_NUM                     4

#define HRV_INTERFACE_VERSION "pv_v1.0.0"
#define HRV_INTERFACE_VERSION_LEN_MAX (20)

typedef struct {
    int32_t need_ipl;                                   // ???????????1KHz
    int32_t fs;                                         // ??????
    int32_t acc_thr[ACC_THR_NUM];                       // acc ???????????????20/10/3/3
}goodix_hrv_config;

typedef struct {
	uint8_t bit_num;                       //????¦Ë??
    int32_t *ppg_rawdata;                 // PPG??????????????
    int32_t *cur_adj_flg;                 // ??????????¦Ë
    int32_t *gain_adj_flg;                // ?????????¦Ë
	int32_t acc_x;                        // ????? x ??
    int32_t acc_y;                        // ????? y ??
    int32_t acc_z;                       // ????? z ??
    int32_t frame_id;                    // ????
    int32_t hr;                           // ????????
}goodix_hrv_input_rawdata;

typedef struct {
    int32_t rri[RRI_NUM];                               // RRI???????
    int32_t rri_confidence;                             // ????????
	int32_t rri_valid_num;                              // RRI??§¹???????
}goodix_hrv_result;

IOT_SYS_DLL_API uint8_t *goodix_hrv_version(void);

IOT_SYS_DLL_API goodix_hrv_ret goodix_hrv_init(const void *cfg_instance, uint32_t cfg_size, const char *interface_ver);

IOT_SYS_DLL_API goodix_hrv_ret goodix_hrv_calc(goodix_hrv_input_rawdata *input_data, goodix_hrv_result *output_data);

IOT_SYS_DLL_API goodix_hrv_ret goodix_hrv_deinit(void);

/**
* @brief ???hrv????????¨°???
* @return     hrv????????¨°???
*/
IOT_SYS_DLL_API const void *goodix_hrv_config_get_arr(void);

/**
* @brief ???hrv??????size
* @return     hrv??????size
*/
IOT_SYS_DLL_API int32_t  goodix_hrv_config_get_size(void);

/**
* @brief ???HRV?????????·Ú??
* @param[in/out]  ver       ?·Ú?????????
* @param[in] 	  ver_len   ?·Ú?????????????HRV_INTERFACE_VERSION_LEN_MAX
* @return     	  ??
*/
IOT_SYS_DLL_API void goodix_hrv_config_get_version(char* ver, uint8_t ver_len);


#if DEBUG_HRV
IOT_SYS_DLL_API goodix_hrv_debug *copy_debug_addr();
#endif
#ifdef __cplusplus
}
#endif

#endif // _GOODIX_SYS_HRV_LIB_H_
