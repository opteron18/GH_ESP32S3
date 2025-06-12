#ifndef _WWD_MAIN_H_
#define _WWD_MAIN_H_
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "goodix_type.h"

#define NADT_INTERFACE_VERSION "pv_v1.0.0"
#define NADT_INTERFACE_VERSION_LEN_MAX (20)

#define IOT_NADT_MAX(a,b)                ((a)<(b)?(b):(a))
#define IOT_NADT_MIN(a,b)                ((a)>(b)?(b):(a))
#define IOT_NADT_ABS(x)                  abs(x)
#define IOT_NADT_ABS_GS32(x)             abs((GS32)(x))
//#define IOT_NADT_ABS_GF32(x)             fabsf((GF32)(x))
//#define IOT_NADT_ABS_GF64(x)             fabs((GF64)(x))
//#define IOT_NADT_SQRT(x)                 sqrt(x)
#define IOT_NADT_SQRTF(x)                sqrtf(x)
#define IOT_NADT_ROUND(x)                round(x)
#define NADT_PTR_NULL                   ((void *)0)

typedef enum
{
	GX_ALGO_NADT_SUCCESS = 0x00000000,			/**< 成功     */
	GX_ALGO_NADT_NO_MEMORY = 0x00000001,			/**< 内存空间不够     */
	GX_ALGO_NADT_UNINIT = 0x00000002,
	GX_ALGO_NADT_RWONG_INPUT = 0x00000003,			/**< 输入数据格式不合法     */
}goodix_nadt_ret;

#define NADT_MID_INFO_OUT 1

//算法中间变量
typedef struct
{
	//
	GU8 wearResult;
	GU8 moveFlag;
	GU8 liveFlag;
	GU8 bgJumpFlag;
	GU8 bgPeriodFlag;
	////
	////ADT判断
	//GS32 adtIRAvg;						//ADT值
	//
	//运动判断
	GS32 accStd;						//ACC标准差
	//
	//活体检测
	GS32 rawPVLen;						//波谷数
	GS32 curHR;							//心率
	GS32 difRrInter;					//心率变化
	GU8 heartBeatFlag;                  //心率标记
	GU8 confiHeartBeatFlag;             //高置信度心率标记
	GU8 wearAcfScore;					//自相关分数
	GS32 signal;					    //信号量
	//基线判断
	GU16 baseLineRatio;					//基线单调率
	GS32 baseLineDiff;					//基线diff值
	//
	//高频背景光检测
	GS16 jumpBgDiff;					//背景光diff
	GU8 bgPeakNum;						//背景光波峰个数
	//
	//背景光周期检测
	GS16 periodBgDiff;					//背景光变化量
	GU16 bgPeriod;						//背景光周期
	GU16 bgPeriodDiff;					//背景光周期差异
}NADT_ST_MID_INFO;



typedef struct
{
	GU16 sampleRateNADT;				//采样率
	GU8 unwearDetectLevel;				//佩戴等级
	GU8 sleepStatus;					//睡眠状态
	//
	GU16 checkTimeoutThr;				//活体检测超时时间
	GU8 unwearCheckCntThr;				//脱落确认时间
	GU8 wearCheckCntThr;				//活体确认时间
	//
	//ADT判断
	GS32 adtWearOnThr;					//ADT wear on阈值
	GS32 adtWearOffThr;					//ADT wear off阈值
	//
	//运动判断
	GS32 accStdThr;						//ACC标准差
	//
	//活体检测
	GU8 liveDetectEnable;				//活体检测使能
	GS32 rawPVLenThr;					//高次谐波阈值
	GU16 heartBeatThrLow;				//心率下限
	GU16 heartBeatThrHigh;				//心率上限
	GS32 difRrInterThr;					//心率变化阈值
	GU8 wearAcfScoreThr;				//自相关分数阈值
	GU16 baseLinePosRatioThr;			//基线单调率阈值
	GS32 baseLineDiffThr;			    //基线平坦性阈值
	GS32 sigScaleThr;
	//
	//高频背景光检测
	GU8 bgJumpDetectEnable;				//背景光突变检测使能
	GS16 jumpBgDiffThr;					//背景光变化量阈值
	GU8 bgPeakValleyNumThr;				//背景光峰谷个数阈值
	//
	//背景光周期检测
	GU8 bgPeriodDetectEnable;			//背景光周期检测使能
	GS16 periodBgDiffThr;				//背景光变化量阈值
	GU16 bgPeriodThr;					//背景光周期阈值
	GU16 bgPeriodDiffThr;				//背景光周期差异阈值
}goodix_nadt_config;


#ifdef __cplusplus    
extern "C" {          
#endif

	DRVDLL_API goodix_nadt_ret NADTProcInit(const void *cfg_instance, GS32 cfg_size, const char *interface_ver);
	DRVDLL_API goodix_nadt_ret NADTProcDeinit(void);
	DRVDLL_API goodix_nadt_ret NADTProc(GS32 lPacketInfoArr[], GS32 lResult[], NADT_ST_MID_INFO *stMidInfo);
	DRVDLL_API goodix_nadt_ret GetNadtVersion(GU8 version[120]);
	DRVDLL_API GU8 NADTWearDetectByCap(GS32 lPacketInfoArr[]);

	DRVDLL_API const void *goodix_nadt_config_get_arr(void);
	DRVDLL_API GS32 goodix_nadt_config_get_size(void);
	DRVDLL_API void goodix_nadt_config_get_version(char* ver, GU8 ver_len);

#ifdef __cplusplus
}
#endif

#endif
