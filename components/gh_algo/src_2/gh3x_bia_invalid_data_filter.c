/**
  ****************************************************************************************
  * @file    gh3x_bia_invalid_data_filter.c
  * @author  GHealth Driver Team
  * @brief   gh3x driver example
  ****************************************************************************************
  * @attention  
  #####Copyright (c) 2024 GOODIX
   All rights reserved.
  
  ****************************************************************************************
  */

/* includes */
#include "stdint.h"
#include "string.h"
#include "gh3x_bia_invalid_data_filter.h"

/**
 * @brief Initialize bia invalid data filter
 *
 * @param pstBiaCaliPro: point of bia data filter
 * @return BID_OFFSET_PRO_INIT_E code
 *
 * @note initialize module
 */
BID_DATA_FILTER_INIT_E BiaInvalidDataFilterInit(BIA_DATA_FILTER_T *pstBiaDataFilter)
{
    if (NULL == pstBiaDataFilter)
    {
        return BIA_DATA_FILTER_INIT_FAIL;
    }

    memset(pstBiaDataFilter, 0, sizeof(BIA_DATA_FILTER_T));
    pstBiaDataFilter->uchLastNodeId = 0xff;
    pstBiaDataFilter->uchLastSlotId = 0xff;

    return BIA_DATA_FILTER_INIT_OK;
}

/**
 * @brief Filter invalid data
 *
 * @param pstBiaCaliPro: point of bia data filter
 * @param uchNodeId: node number
 * @param uchSlotId: slot number
 * @return BID_DATA_FLAG_E code
 *
 * @note Skip some value when node ID is being changed
 */
BID_DATA_FLAG_E BiaInvalidDataFilterExe(BIA_DATA_FILTER_T *pstBiaDataFilter, uint8_t uchNodeId, uint8_t uchSlotId)
{
    BID_DATA_FLAG_E uchValidFlag = BIA_DATA_INVALID;

    if (NULL == pstBiaDataFilter || uchSlotId >= BIA_FRQ_NUM_LIMIT)
    {
        return BIA_DATA_INVALID;
    }

    if (pstBiaDataFilter->uchLastNodeId != uchNodeId || pstBiaDataFilter->uchLastSlotId != uchSlotId)
    {
        pstBiaDataFilter->uchLastNodeId = uchNodeId;
        pstBiaDataFilter->uchLastSlotId = uchSlotId;
        pstBiaDataFilter->uchState[uchSlotId] = BIA_DATA_FILTERING;
        pstBiaDataFilter->puchSkipedSampleCnt[uchSlotId] = 0;
    }

    if (BIA_DATA_FILTERING == pstBiaDataFilter->uchState[uchSlotId] && pstBiaDataFilter->puchSkipedSampleCnt[uchSlotId] < BIA_INVALID_DATA_NUM)
    {
        pstBiaDataFilter->puchSkipedSampleCnt[uchSlotId]++;
        uchValidFlag = BIA_DATA_INVALID;
    }
    else if (BIA_DATA_FILTER_IDLE == pstBiaDataFilter->uchState[uchSlotId])
    {
        uchValidFlag = BIA_DATA_INVALID;
    }
    else
    {
        pstBiaDataFilter->uchState[uchSlotId] = BIA_DATA_FILTERED;
        pstBiaDataFilter->puchSkipedSampleCnt[uchSlotId] = 0;
        uchValidFlag = BIA_DATA_VALID;
    }

    return uchValidFlag;

}

