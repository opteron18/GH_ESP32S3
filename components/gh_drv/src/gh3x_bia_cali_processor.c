/**
  ****************************************************************************************
  * @file    gh3x_bia_cali_processor.c
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
#include "gh3x_bia_cali_processor.h"
#include "gh3x_demo_config.h"

/**
 * @brief Initialize bia cali processor
 *
 * @param pstBiaCaliPro: point of bia cali processor
 * @return BIA_CALI_PRO_INIT_E code
 *
 * @note initialize module
 */
BIA_CALI_PRO_INIT_E BiaCaliProcessorInit(BIA_CALI_PRO_T *pstBiaCaliPro)
{
    if (NULL == pstBiaCaliPro)
    {
        return BIA_CALI_PRO_INIT_FAIL;
    }

    memset(pstBiaCaliPro, 0, sizeof(BIA_CALI_PRO_T));

    return BIA_CALI_PRO_INIT_OK;
}

/**
 * @brief Record bia standard ResisIq
 *
 * @param pstBiaCaliPro: point of bia cali processor
 * @param pstIQ: IQ value
 * @param uchFreqIndex: slot number
 * @return BIA_CALI_PRO_RECORD_E code
 *
 * @note Average IQ data
 */
BIA_CALI_PRO_RECORD_E BiaCaliProcessorStandardResisIqRecord(BIA_CALI_PRO_T *pstBiaCaliPro, complex_S32 *pstDeoffsetIQ, uint8_t uchFreqIndex)
{
    BIA_CALI_PRO_RECORD_E emState = BIA_CALI_STATE_NOT_STARTED;
    
    if(pstBiaCaliPro->uchCoefFlag[uchFreqIndex] == BIA_CALI_COEF_OK){
        return BIA_CALI_STATE_SUCCESSED;
    }

    if (NULL == pstBiaCaliPro || NULL == pstDeoffsetIQ || uchFreqIndex >= BIA_FRQ_NUM_LIMIT)
    {
        return BIA_CALI_STATE_FAILED;
    }

    if (pstBiaCaliPro->uchStandardResisSampleCnt[uchFreqIndex] < BIA_CALI_SUM_NUM)
    {
        pstBiaCaliPro->uchStandardResisSampleCnt[uchFreqIndex]++;
        pstBiaCaliPro->stStandardResisIqSumOrAva[uchFreqIndex].real += pstDeoffsetIQ->real;
        pstBiaCaliPro->stStandardResisIqSumOrAva[uchFreqIndex].imag += pstDeoffsetIQ->imag;
        emState = BIA_CALI_STATE_GOING;
        EXAMPLE_LOG("[StandardResisIqRecord] FreqIndex=%d,I = %d,Q = %d\r\n", uchFreqIndex, pstDeoffsetIQ->real,pstDeoffsetIQ->imag);
    }

    if (pstBiaCaliPro->uchStandardResisSampleCnt[uchFreqIndex] == BIA_CALI_SUM_NUM)
    {
        pstBiaCaliPro->stStandardResisIqSumOrAva[uchFreqIndex].real = pstBiaCaliPro->stStandardResisIqSumOrAva[uchFreqIndex].real / BIA_CALI_SUM_NUM;
        pstBiaCaliPro->stStandardResisIqSumOrAva[uchFreqIndex].imag = pstBiaCaliPro->stStandardResisIqSumOrAva[uchFreqIndex].imag / BIA_CALI_SUM_NUM;

        double dSquareSum = (double)pstBiaCaliPro->stStandardResisIqSumOrAva[uchFreqIndex].real * \
                            (double)pstBiaCaliPro->stStandardResisIqSumOrAva[uchFreqIndex].real + \
                            (double)pstBiaCaliPro->stStandardResisIqSumOrAva[uchFreqIndex].imag * \
                            (double)pstBiaCaliPro->stStandardResisIqSumOrAva[uchFreqIndex].imag;

        pstBiaCaliPro->pstCalibrateedCoefIQ[uchFreqIndex].real = (double)BIA_CALI_R_REF * \
                                                                 (double)pstBiaCaliPro->stStandardResisIqSumOrAva[uchFreqIndex].real / dSquareSum;

        pstBiaCaliPro->pstCalibrateedCoefIQ[uchFreqIndex].imag = (-1.0) * (double)BIA_CALI_R_REF * \
                                                                 (double)pstBiaCaliPro->stStandardResisIqSumOrAva[uchFreqIndex].imag / dSquareSum;

        pstBiaCaliPro->uchCoefFlag[uchFreqIndex] = BIA_CALI_COEF_OK;
        emState = BIA_CALI_STATE_SUCCESSED;
    }

    return emState;
}

/**
 * @brief Calibrate bia IQ value
 *
 * @param pstBiaCaliPro: point of bia cali processor
 * @param pstDeoffsetIQ: IQ value with offset subtracted
 * @param uchFreqIndex: slot number
 * @param pstCalibrateedIQ: Calibrated IQ value
 * @return BIA_CALI_PRO_CALI_E code
 *
 * @note Called after BiaCaliProcessorStandardResisIqRecord successed
 */
BIA_CALI_PRO_CALI_E BiaCaliProcessorCalibrate(BIA_CALI_PRO_T *pstBiaCaliPro, complex_S32 *pstDeoffsetIQ, uint8_t uchFreqIndex, complex_double *pstCalibrateedIQ)
{
    if (NULL == pstBiaCaliPro || NULL == pstDeoffsetIQ || NULL == pstCalibrateedIQ || uchFreqIndex >= BIA_FRQ_NUM_LIMIT)
    {
        return BIA_CALI_FAIL;
    }

    if (pstBiaCaliPro->uchCoefFlag[uchFreqIndex] != BIA_CALI_COEF_OK)
    {
        return BIA_CALI_COEF_NOT_OK;
    }

    pstCalibrateedIQ->real = (double)pstDeoffsetIQ->real * (double)pstBiaCaliPro->pstCalibrateedCoefIQ[uchFreqIndex].real - \
                         (double)pstDeoffsetIQ->imag * (double)pstBiaCaliPro->pstCalibrateedCoefIQ[uchFreqIndex].imag;

    pstCalibrateedIQ->imag = (double)pstDeoffsetIQ->real * (double)pstBiaCaliPro->pstCalibrateedCoefIQ[uchFreqIndex].imag + \
                         (double)pstDeoffsetIQ->imag * (double)pstBiaCaliPro->pstCalibrateedCoefIQ[uchFreqIndex].real;

    EXAMPLE_LOG("[BiaCali] FreqIndex=%d, I = %.3f, Q = %.3f\r\n", uchFreqIndex,pstCalibrateedIQ->real,pstCalibrateedIQ->imag);
    
    return BIA_CALI_OK;
}

