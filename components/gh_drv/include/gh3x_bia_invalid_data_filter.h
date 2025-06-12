/**
 * @copyright (c) 2003 - 2024, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x_bia_invalid_data_filter.h
 *
 * @brief   header file of gh3x bia invalid data filter
 *
 * @version GH(M)3X2X_DRV_LIB_DEMO_1.0
 *
 * @author  Gooidx Iot Team
 *
 */

#ifndef __GH3X_BIA_INVALID_DATA_FILTER_H__
#define __GH3X_BIA_INVALID_DATA_FILTER_H__

#include "gh3x_bia_common.h"

#define BIA_INVALID_DATA_NUM 3

typedef struct
{
    uint8_t puchSkipedSampleCnt[BIA_FRQ_NUM_LIMIT];
    uint8_t uchState[BIA_FRQ_NUM_LIMIT];
    uint8_t uchLastNodeId;
    uint8_t uchLastSlotId;
}BIA_DATA_FILTER_T;

typedef enum
{
    BIA_DATA_FILTER_IDLE = 0,
    BIA_DATA_FILTERING = 1,
    BIA_DATA_FILTERED = 2
}BID_DATA_FILTER_STATE_E;

typedef enum
{
    BIA_DATA_FILTER_INIT_OK = 0,
    BIA_DATA_FILTER_INIT_FAIL = 1,
}BID_DATA_FILTER_INIT_E;

typedef enum
{
    BIA_DATA_INVALID = 0,
    BIA_DATA_VALID = 1,
}BID_DATA_FLAG_E;

BID_DATA_FILTER_INIT_E BiaInvalidDataFilterInit(BIA_DATA_FILTER_T *pstBiaDataFilter);
BID_DATA_FLAG_E BiaInvalidDataFilterExe(BIA_DATA_FILTER_T *pstBiaDataFilter, uint8_t uchNodeId, uint8_t uchSlotId);

#endif // __GH3X_BIA_INVALID_DATA_FILTER_H__

