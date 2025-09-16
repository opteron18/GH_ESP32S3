/**
  ****************************************************************************************
  * @file    gh3x_bia_offset_processor.c
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
#include "gh3x_bia_offset_processor.h"

/**
 * @brief Initialize bia offset processor
 *
 * @param pstBiaCaliPro: point of bia offset processor
 * @return BID_OFFSET_PRO_INIT_E code
 *
 * @note initialize module
 */
BID_OFFSET_PRO_INIT_E BiaOffsetProcessorInit(BIA_OFFSET_PRO_T *pstBiaOffsetPro)
{
    if (NULL == pstBiaOffsetPro)
    {
        return BIA_OFFSET_PRO_INIT_FAIL;
    }

    memset(pstBiaOffsetPro, 0, sizeof(BIA_OFFSET_PRO_T));

    return BIA_OFFSET_PRO_INIT_OK;
}

/**
 * @brief Record bia value for offset
 *
 * @param pstBiaCaliPro: point of bia offset processor
 * @param pstIQ: IQ value
 * @param uchFreqIndex: slot number
 * @return BID_OFFSET_PRO_RECORD_E code
 *
 * @note Average IQ data after skip some value
 */
BID_OFFSET_PRO_RECORD_E BiaOffsetProcessorOffsetRecord(BIA_OFFSET_PRO_T *pstBiaOffsetPro, complex_S32 *pstIQ, uint8_t uchFreqIndex)
{
    BID_OFFSET_PRO_RECORD_E emState = BIA_OFFSET_STATE_NOT_STARTED;
    
    if(pstBiaOffsetPro->uchFinishFlag[uchFreqIndex] == BIA_OFFSET_STATE_SUCCESSED){
        return BIA_OFFSET_STATE_SUCCESSED;
    }

    if (NULL == pstBiaOffsetPro || NULL == pstIQ || uchFreqIndex >= BIA_FRQ_NUM_LIMIT)
    {
        pstBiaOffsetPro->uchFinishFlag[uchFreqIndex] = BIA_OFFSET_STATE_FAILED;

        return BIA_OFFSET_STATE_FAILED;
    }

    if (pstBiaOffsetPro->uchOffsetSkipNum[uchFreqIndex] < BIA_OFFSET_SKIP_NUM)
    {
        pstBiaOffsetPro->uchOffsetSkipNum[uchFreqIndex]++;
        pstBiaOffsetPro->uchFinishFlag[uchFreqIndex] = BIA_OFFSET_STATE_NOT_STARTED;

        return BIA_OFFSET_STATE_NOT_STARTED;
    }

    if (pstBiaOffsetPro->uchOffsetSampleCnt[uchFreqIndex] < BIA_OFFSET_SUM_NUM)
    {
        pstBiaOffsetPro->uchOffsetSampleCnt[uchFreqIndex]++;
        pstBiaOffsetPro->stOffSetSumOrAva[uchFreqIndex].real += pstIQ->real;
        pstBiaOffsetPro->stOffSetSumOrAva[uchFreqIndex].imag += pstIQ->imag;
        pstBiaOffsetPro->uchFinishFlag[uchFreqIndex] = BIA_OFFSET_STATE_GOING;
        emState = BIA_OFFSET_STATE_GOING;
    }

    if (pstBiaOffsetPro->uchOffsetSampleCnt[uchFreqIndex] == BIA_OFFSET_SUM_NUM)
    {
        pstBiaOffsetPro->stOffSetSumOrAva[uchFreqIndex].real = pstBiaOffsetPro->stOffSetSumOrAva[uchFreqIndex].real / BIA_OFFSET_SUM_NUM;
        pstBiaOffsetPro->stOffSetSumOrAva[uchFreqIndex].imag = pstBiaOffsetPro->stOffSetSumOrAva[uchFreqIndex].imag / BIA_OFFSET_SUM_NUM;
        pstBiaOffsetPro->uchFinishFlag[uchFreqIndex] = BIA_OFFSET_STATE_SUCCESSED;
        emState = BIA_OFFSET_STATE_SUCCESSED;
    }

    return emState;
}

/**
 * @brief Deoffset bia value
 *
 * @param pstBiaCaliPro: point of bia offset processor
 * @param pstIQ: IQ value
 * @param uchFreqIndex: slot number
 * @param pstDeoffsetIQ: deoffseted IQ value
 * @return BID_OFFSET_PRO_DEOFFSET_E code
 *
 * @note Deoffset IQ value, call after BiaOffsetProcessorOffsetRecord successed
 */
BID_OFFSET_PRO_DEOFFSET_E BiaOffsetProcessorDeoffset(BIA_OFFSET_PRO_T *pstBiaOffsetPro, complex_S32 *pstIQ, uint8_t uchFreqIndex, complex_S32 *pstDeoffsetIQ)
 {
    if (NULL == pstBiaOffsetPro || NULL == pstIQ || uchFreqIndex >= BIA_FRQ_NUM_LIMIT || NULL == pstDeoffsetIQ)
    {
        return BIA_OFFSET_PRO_DEOFFSET_FAILED;
    }

    if (pstBiaOffsetPro->uchFinishFlag[uchFreqIndex] != BIA_OFFSET_STATE_SUCCESSED)
    {
        return BIA_OFFSET_PRO_RECORD_FAILED;
    }

    pstDeoffsetIQ->real = pstIQ->real - pstBiaOffsetPro->stOffSetSumOrAva[uchFreqIndex].real;
    pstDeoffsetIQ->imag = pstIQ->imag - pstBiaOffsetPro->stOffSetSumOrAva[uchFreqIndex].imag;

    return BIA_OFFSET_PRO_DEOFFSET_OK;
}

