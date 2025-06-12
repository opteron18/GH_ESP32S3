/**
 * @copyright (c) 2003 - 2024, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x_bia_cali_processor.h
 *
 * @brief   header file of gh3x bia cali process
 *
 * @version GH(M)3X2X_DRV_LIB_DEMO_1.0
 *
 * @author  Gooidx Iot Team
 *
 */

#ifndef __GH3X_BIA_CALI_PROCESSOR_H__
#define __GH3X_BIA_CALI_PROCESSOR_H__

#include "gh3x_bia_common.h"

#define BIA_CALI_SUM_NUM  3
#define BIA_CALI_R_REF    (1000.0)

typedef enum
{
    BIA_CALI_PRO_INIT_OK = 0,
    BIA_CALI_PRO_INIT_FAIL = 1
}BIA_CALI_PRO_INIT_E;

typedef enum
{
    BIA_CALI_STATE_NOT_STARTED = 0,
    BIA_CALI_STATE_GOING = 1,
    BIA_CALI_STATE_SUCCESSED = 2,
    BIA_CALI_STATE_FAILED = 3
}BIA_CALI_PRO_RECORD_E;

typedef enum
{
    BIA_CALI_COEF_FAIL = 0,
    BIA_CALI_COEF_OK = 1
}BIA_CALI_PRO_CALI_COEF_E;

typedef enum
{
    BIA_CALI_FAIL = 0,
    BIA_CALI_COEF_NOT_OK = 1,
    BIA_CALI_OK = 2
}BIA_CALI_PRO_CALI_E;

typedef struct
{
    uint8_t uchStandardResisSampleCnt[BIA_FRQ_NUM_LIMIT];
    uint8_t uchCoefFlag[BIA_FRQ_NUM_LIMIT];
    complex_S32 stStandardResisIqSumOrAva[BIA_FRQ_NUM_LIMIT];
    complex_double pstCalibrateedCoefIQ[BIA_FRQ_NUM_LIMIT];
}BIA_CALI_PRO_T;

BIA_CALI_PRO_INIT_E BiaCaliProcessorInit(BIA_CALI_PRO_T *pstBiaCaliPro);
BIA_CALI_PRO_RECORD_E BiaCaliProcessorStandardResisIqRecord(BIA_CALI_PRO_T *pstBiaCaliPro, complex_S32 *pstDeoffsetIQ, uint8_t uchFreqIndex);
BIA_CALI_PRO_CALI_E BiaCaliProcessorCalibrate(BIA_CALI_PRO_T *pstBiaCaliPro, complex_S32 *pstDeoffsetIQ, uint8_t uchFreqIndex, complex_double *pstCalibrateedIQ);

#endif // __GH3X_BIA_CALI_PROCESSOR_H__

