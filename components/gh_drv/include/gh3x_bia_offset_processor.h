/**
 * @copyright (c) 2003 - 2024, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x_bia_offset_processor.h
 *
 * @brief   header file of gh3x bia offset process
 *
 * @version GH(M)3X2X_DRV_LIB_DEMO_1.0
 *
 * @author  Gooidx Iot Team
 *
 */

#ifndef __GH3X_BIA_OFFSET_PROCESSOR_H__
#define __GH3X_BIA_OFFSET_PROCESSOR_H__

#include "gh3x_bia_common.h"

#define BIA_OFFSET_SKIP_NUM 20
#define BIA_OFFSET_SUM_NUM  30

typedef struct
{
    uint8_t uchFinishFlag[BIA_FRQ_NUM_LIMIT];
    complex_S32 stOffSetSumOrAva[BIA_FRQ_NUM_LIMIT];
    uint8_t uchOffsetSampleCnt[BIA_FRQ_NUM_LIMIT];
    uint8_t uchOffsetSkipNum[BIA_FRQ_NUM_LIMIT];
}BIA_OFFSET_PRO_T;

typedef enum
{
    BIA_OFFSET_PRO_INIT_OK = 0,
    BIA_OFFSET_PRO_INIT_FAIL = 1,
}BID_OFFSET_PRO_INIT_E;

typedef enum
{
    BIA_OFFSET_STATE_NOT_STARTED = 0,
    BIA_OFFSET_STATE_GOING = 1,
    BIA_OFFSET_STATE_SUCCESSED = 2,
    BIA_OFFSET_STATE_FAILED = 3
}BID_OFFSET_PRO_RECORD_E;

typedef enum
{
    BIA_OFFSET_PRO_DEOFFSET_FAILED = 0,
    BIA_OFFSET_PRO_RECORD_FAILED = 1,
    BIA_OFFSET_PRO_DEOFFSET_OK = 2
}BID_OFFSET_PRO_DEOFFSET_E;

BID_OFFSET_PRO_INIT_E BiaOffsetProcessorInit(BIA_OFFSET_PRO_T *pstBiaOffsetPro);
BID_OFFSET_PRO_RECORD_E BiaOffsetProcessorOffsetRecord(BIA_OFFSET_PRO_T *pstBiaOffsetPro, complex_S32 *pstIQ, uint8_t uchFreqIndex);
BID_OFFSET_PRO_DEOFFSET_E BiaOffsetProcessorDeoffset(BIA_OFFSET_PRO_T *pstBiaOffsetPro, complex_S32 *pstIQ, uint8_t uchFreqIndex, complex_S32 *pstDeoffsetIQ);

#endif // __GH3X_BIA_OFFSET_PROCESSOR_H__

