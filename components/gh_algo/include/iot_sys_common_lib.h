/*
* RomaIICommon.h
*/

#ifndef _GOODIX_IOT_SYS_COMMON_LIB_H
#define _GOODIX_IOT_SYS_COMMON_LIB_H

#include "iot_sys_def.h"
#include <string.h>
#include <math.h>

float InnerRound(float x);

#define BIA_MAX(a,b)		((a)<(b)?(b):(a))
#define BIA_MIN(a,b)		((a)>(b)?(b):(a))
#define BIA_ABS_GS32(x)		abs((GS32)(x))
#define BIA_ABS_GF32(x)		fabsf((GF32)(x))
#define BIA_ABS_GF64(x)		fabs((GF64)(x))
#define BIA_ROUNDF(x)		InnerRound(x)
#define BIA_SQRT(x)			sqrt(x)
#define BIA_SQRTF(x)		sqrtf(x)
#define BIA_ATAN(x)         atan(x)

typedef struct {
	GS32 *buf;			// buffer地址入口
	GU32 num;			// buffer的元素数目
	GU32 size;			// buffer的总大小
	GU32 addCount;		// 实际添加元素次数
}ST_BIA_BUFFER_GS32_T;

GS32 InnerBufferInitF32(ST_BIA_BUFFER_GS32_T *pstBuf, GS32 *pfBuf, GS32 nSize);

void InnerBufferAddF32(ST_BIA_BUFFER_GS32_T *pstBuf, GS32 fData);

void InnerBufferDeinitF32(ST_BIA_BUFFER_GS32_T *stBuf);

GU8 InnerBufferIsFullF32(ST_BIA_BUFFER_GS32_T *stBuf);

GS32 InnerBufferIsRoundF32(ST_BIA_BUFFER_GS32_T *stBuf);

void InnerBufferGetMeanF32(ST_BIA_BUFFER_GS32_T *stBuf, GF32 *pRes);

void InnerBufferGetSumF32(ST_BIA_BUFFER_GS32_T* stBuf, GS32* pRes);

void InnerBufferGetStdF32(ST_BIA_BUFFER_GS32_T *stBuf, GF32 *pRes);

GF32 InnerBufferGetValueAtIdxF32(ST_BIA_BUFFER_GS32_T *stBuf, GU32 nIdx);

GU32 InnerBufferGetNumF32(ST_BIA_BUFFER_GS32_T *stBuf);

#endif // _GOODIX_IOT_SYS_COMMON_LIB_H
