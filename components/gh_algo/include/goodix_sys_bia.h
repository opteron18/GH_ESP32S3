/*
* BIALib.h
*/

#ifndef _GOODIX_SYS_BIA_LIB_H_
#define _GOODIX_SYS_BIA_LIB_H_

#include <stdint.h>
#include "iot_sys_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 返回错误码 */
#define GX_BIA_ALGO_OK					((uint32_t)0x00000000) // 成功返回
#define GX_BIA_ALGO_FAILED				((uint32_t)0x10000001) // 失败返回
#define GX_BIA_ALGO_NULL_PTR			((uint32_t)0x10000002) // 外部传入的指针为空
#define GX_BIA_ALGO_INVALID_PARAM		((uint32_t)0x10000003) // 参数范围无效
#define GX_BIA_ALGO_OUT_OF_MEM			((uint32_t)0x10000004) // 内存分配失败
#define GX_BIA_ALGO_DEINIT_ABORT		((uint32_t)0x10000005) // 释放失败

#define BIA_LEAD_ON						1															 
#define BIA_LEAD_OFF          0                              

#define BIA_STOP_OFFSET     0
#define BIA_LEAD_OFFSET     1
#define BIA_CONFID_OFFSET   2
#define BIA_I_DATA_OFFSET   3
#define BIA_Q_DATA_OFFSET   4
#define BIA_FFM_OFFSET      5
#define BIA_BFR_OFFSET      6
#define BIA_TBW_OFFSET      7
#define BIA_TBWR_OFFSET     8
#define BIA_BMR_OFFSET      9
#define BIA_BMI_OFFSET      10 
#define BIA_SMA_OFFSET      11
#define BIA_MAG_OFFSET      12 
#define BIA_PHASE_OFFSET    13
#define BIA_CALI_I_OFFSET   14 
#define BIA_CALI_Q_OFFSET   15 

typedef struct {
	int32_t frame_id;
	int32_t bia_raw_I;
	int32_t bia_raw_Q;
	int32_t bia_confg_flg; //0:采集校准电阻;1:测量人体阻抗 2：电极脱落检测

	int32_t sweep_PN_flg; //是否更新PN node
	int32_t idx_slot;	//采集不同配置标志位
	int32_t	idx_frame; //采集不同链路标志位


}ST_BIA_INPUT_INFO;

IOT_SYS_DLL_API GU8 *BIAAlgoVersion(void);

IOT_SYS_DLL_API GU32 BIAAlgoInit(GS32 PInfo[], GU32 uFs, GU32 uBiaMeasureMode);

IOT_SYS_DLL_API GU32 BIACalculateAlgo(ST_BIA_INPUT_INFO *rawdata, GS32 resultArr[]);

IOT_SYS_DLL_API GU32 BIAAlgoDeInit(void);

#ifdef __cplusplus
}
#endif

#endif // _GOODIX_SYS_BIA_LIB_H_
