/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x_drv_algorithm_calc.c
 *
 * @brief   gh3x algorithm calc functions
 *
 * @version ref gh3x_drv_version.h
 *
 */


#include <stdio.h>
#include "gh3x_demo_algo_call_inner.h"
#include "gh3x_demo_algo_config.h"
#include "goodix_log.h"
#include "goodix_mem.h"
#include "goodix_type.h"
#include "goodix_hba.h"
#include "goodix_sys_hrv.h"
#include "goodix_spo2.h"
#include "goodix_nadt.h"
//#include "goodix_algo_bt.h"

#if 1//(__GOODIX_ALGO_CALL_MODE__)

/* hb algorithm */
#if (__USE_GOODIX_HR_ALGORITHM__)
GS32 gunHrParamsArr[GOODIX_HR_PARAMS_NUM_MAX] = {0, 0, 0, 20, 8};
/// hbd rawdata channel map
STAlgoChnlMap gstHbaAlgoChnlMap;
#endif

/* hrv algorithm */
#if (__USE_GOODIX_HRV_ALGORITHM__)
GS32 gnHrvParamsArr[GOODIX_HRV_PARAMS_NUM_MAX] = {0, 20, 10, 3, 3};
#endif

/* spo2 algorithm */
#if (__USE_GOODIX_SPO2_ALGORITHM__)
GS32 gnSpo2ParamssArr[GOODIX_SPO2_PARAMS_NUM_MAX] = {0};
/// spo2 rawdata channel map
STAlgoChnlMap gstSpo2AlgoChnlMap;
#endif

/* ecg algorithm */
#if (__USE_GOODIX_ECG_ALGORITHM__)
GS32 gnEcgParamsArr[GOODIX_ECG_PARAMS_NUM_MAX] = {0, 0, 0, 0, 0};
/// ecg frame data cnt
GU32 gunEcgFrameCnt = 0;
GU8 guchClassificationFlag;
GU8 gunEcgTimeSeconds = 0;
#endif

/* bt algorithm */
#if (__USE_GOODIX_BT_ALGORITHM__)
GS32 gnBtParamsArr[GOODIX_BT_PARAMS_NUM_MAX] = {0};
#endif

/* af algorithm */
#if (__USE_GOODIX_AF_ALGORITHM__)
GS32 gnAfParamsArr[GOODIX_AF_PARAMS_NUM_MAX] = {45, 0};
#endif

#if 1//(__USE_GOODIX_BIA_ALGORITHM__)
GS32 gnBiaPInfo[GOODIX_BIA_PARAMS_NUM_MAX];
STAlgoChnlMap gstBiaAlgoChnlMap;
#endif
#if 1//(__USE_GOODIX_GSR_ALGORITHM__)
GS32 gnGsrPInfo[GOODIX_GSR_PARAMS_NUM_MAX];
#endif
/// new data flag
GU8 guchNewDataFlag = 0;
GU8 guchGH3XSleepFlag = 0;

//sensor enable
GU8 guchAlgoGsensorEnable = 0;
GU8 guchAlgoCapEnable = 0;
GU8 guchAlgoTempEnable = 0;

GS32 GH3X_Round(GF32 fValue)
{
    GS32 nRet = 0;
    float fEpsinon = 0.00001;
    if((fValue >= -fEpsinon)&&(fValue <= fEpsinon))
    {
        nRet = 0;
    }
    else if(fValue < -fEpsinon)
    {
        nRet = (GS32)(fValue - 0.5f);
    }
    else if(fValue > fEpsinon)
    {
        nRet = (GS32)(fValue + 0.5f);
    }
    return nRet;
}

GU8 GH3X_BitCount(GU32 n)
{
    GU8 c =0 ; // 计数器
    while (n >0)
    {
        if((n &1) ==1) // 当前位是1
            ++c ; // 计数器加1
        n >>=1 ; // 移位
    }
    return c ;
}

void GH3XSleepFlagSet(GU8 uchSleepFlag)
{
    guchGH3XSleepFlag = uchSleepFlag;
}


GU8 GH3XSleepFlagGet(void)
{
    return guchGH3XSleepFlag;
}

void GH3X_AlgoSensorEnable(GU8 uchAlgoGsensorEnable,
                            GU8 uchAlgoCapEnable,
                            GU8 uchAlgoTempEnable)
{
    guchAlgoGsensorEnable = uchAlgoGsensorEnable;
    guchAlgoCapEnable = uchAlgoCapEnable;
    guchAlgoTempEnable = uchAlgoTempEnable;
}


/**
 * @fn     void GH3X_SetNewDataFlag(EMChannelMapId emFuncId, GU8 uchFlag)
 *
 * @brief  Set new sampling data flag
 *
 * @attention   None
 *
 * @param[in]   emFuncId    function id
 * @param[in]   uchFlag      new data or not
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_SetNewDataFlag(GU8 emFuncId, GU8 uchFlag)
{
    if (GH3X_IS_NOT_NEW_RAWDATA == uchFlag)
    {
        GH3X_VAL_CLEAR_LEFT_BIT(guchNewDataFlag, emFuncId);
    }
    else
    {
        GH3X_VAL_SET_BIT(guchNewDataFlag, GH3X_GET_LEFT_SHIFT_VAL(emFuncId));
    }
}

/**
 * @fn     GS8 GH3X_HbAlgorithmScenarioConfig(GU8 uchScenario)
 *
 * @brief  Hb algorithm scenario config
 *
 * @attention   None
 *
 * @param[in]  uchScenario     @ref EMHbaScenesConfigType
 *                             others: fixed 0(default) and return GH3X_RET_PARAMETER_ERROR
 * @param[out]  None
 *
 * @return  error code
 * @retval  #GH3X_RET_OK                      return successfully
 * @retval  #GH3X_RET_PARAMETER_ERROR         return paramters error
 * @retval  #GH3X_RET_ALGO_NO_SUPPORTED       return algorithm no supported error
 */
GS8 GH3X_HbAlgorithmScenarioConfig(GU8 uchScenario)
{
#if (__USE_GOODIX_HR_ALGORITHM__)

    GS8 chRet = GH3X_RET_OK;
    GH3X_ALGO_LOG_PARAM("%s\r\n", __FUNCTION__);
    if (uchScenario > GH3X_HBA_SCENARIO_CONFIG_MAX_VAL)
    {
        GH3X_ALGO_LOG_PARAM("hb algorithm scenario param error!\r\n");
        gunHrParamsArr[1] = HBA_SCENES_DEFAULT;
        chRet = GH3X_RET_PARAMETER_ERROR;
    }
    else
    {
        gunHrParamsArr[1] = uchScenario;
    }

#else

    GS8 chRet = GH3X_RET_ALGO_NO_SUPPORTED;
    GH3X_ALGO_LOG_PARAM("hba algorithm no supported error!\r\n");

#endif

    return chRet;
}

/**
 * @fn     GS8 GH3X_HbAlgorithmOutputTimeConfig(GS32 nOutputTime)
 *
 * @brief  Hb algorithm output time config
 *
 * @attention   None
 *
 * @param[in]   nOutputTime          valid 7~GS32_MAX
 * @param[out]  None
 *
 * @return  error code
 * @retval  #GH3X_RET_OK                      return successfully
 * @retval  #GH3X_RET_ALGO_NO_SUPPORTED       return algorithm no supported error
 */
GS8 GH3X_HbAlgorithmOutputTimeConfig(GS32 nLatestOutputTime, GS32 nEarliestOutputTime)
{
#if (__USE_GOODIX_HR_ALGORITHM__)

    GH3X_ALGO_LOG_PARAM("%s\r\n", __FUNCTION__);
    gunHrParamsArr[3] = nLatestOutputTime;
    gunHrParamsArr[4] = nEarliestOutputTime;
    return GH3X_RET_OK;

#else

    GH3X_ALGO_LOG_PARAM("hba algorithm no supported error!\r\n");
    return GH3X_RET_ALGO_NO_SUPPORTED;

#endif
}

GU16 GH3X_ReadSpo2CorrectionFactor(GU16 usVirtualRegAddr)
{
#if 0
    GU16 usVirtualRegData = 0;
    GU16 usValIndex = 0;

    if(usVirtualRegAddr >= GH3X_VSPO2_CH0_CORFAC0_L_REG_ADDR && usVirtualRegAddr <= GH3X_VSPO2_CH0_CORFAC2_H_REG_ADDR)
    {
        usValIndex = (usVirtualRegAddr - GH3X_VSPO2_CH0_CORFAC0_L_REG_ADDR) / GH3X_REG_ADDR_SIZE;
    }
    else if(usVirtualRegAddr >= GH3X_VSPO2_CH0_CORFAC3_L_REG_ADDR && usVirtualRegAddr <= GH3X_VSPO2_CH3_CORFAC4_H_REG_ADDR)
    {
        usValIndex = (usVirtualRegAddr - GH3X_VSPO2_CH0_CORFAC3_L_REG_ADDR) / GH3X_REG_ADDR_SIZE + 6;
    }
    else
    {
        return usVirtualRegData;
    }
    
   if ((usValIndex % GH3X_REG_ADDR_SIZE) == 0) // low word
   {
       usVirtualRegData = GH3X_GET_LOW_WORD_FROM_DWORD(
                            gnSpo2ParamssArr[usValIndex / (SP02_CORR_FAC_NUM * 2)][(usValIndex % (SP02_CORR_FAC_NUM * 2)) / GH3X_REG_ADDR_SIZE]);
   }
   else // high word
   {
       usVirtualRegData = GH3X_GET_HIGH_WORD_FROM_DWORD(
                            gnSpo2ParamssArr[usValIndex / (SP02_CORR_FAC_NUM * 2)][(usValIndex % (SP02_CORR_FAC_NUM * 2)) / GH3X_REG_ADDR_SIZE]);
   }

   
   return usVirtualRegData;
#else
     return 0;
#endif
}

/**
 * @fn     void GH3X_AlgoChnlMapInit(STAlgoChnlMap stAlgoChnlMap)
 *
 * @brief  Init algo channel group struct
 *
 * @attention   
 *
 *
 * @return  None
 */
void GH3X_AlgoChnlMapInit(STAlgoChnlMap *pstAlgoChnlMap)
{
    (*pstAlgoChnlMap).uchFlag = 0;
    (*pstAlgoChnlMap).uchNum = 0;
    for (GU32 num = 0  ; num < ALGO_CHNL_NUM ; num ++)
    {
        (*pstAlgoChnlMap).uchAlgoChnlMap[num] = 0xff;
    }
}

/**
 * @fn     void GH3X_HbaAlgoChnlMapDefultSet(void)
 
 *
 * @brief  Set algo channel group Defult value
 *
 * @attention   
 *
 *
 * @return  None
 */
void GH3X_HbaAlgoChnlMapDefultSet(void)
{
#if (__USE_GOODIX_HR_ALGORITHM__)
    if (!gstHbaAlgoChnlMap.uchFlag)
    {
        GH3X_AlgoChnlMapInit(&gstHbaAlgoChnlMap);
        gstHbaAlgoChnlMap.uchNum = __HR_ALGORITHM_SUPPORT_CHNL_NUM__;
        for (GU8 cnt = 0; cnt < __HR_ALGORITHM_SUPPORT_CHNL_NUM__; cnt++)
        {
            gstHbaAlgoChnlMap.uchAlgoChnlMap[ALGO_GREEN_CHNL_POS + cnt] = cnt;
        }
    }
#endif
}

/**
 * @fn     void GH3X_HrvAlgoChnlMapDefultSet(void)
 
 *
 * @brief  Set algo channel group Defult value
 *
 * @attention   
 *
 *
 * @return  None
 */
void GH3X_HrvAlgoChnlMapDefultSet(void)
{
#if (0)
    if (!gstHrvAlgoChnlMap.uchFlag)
    {
        GH3X_AlgoChnlMapInit(&gstHrvAlgoChnlMap);
        gstHrvAlgoChnlMap.uchNum = 1;
        gstHrvAlgoChnlMap.uchAlgoChnlMap[ALGO_GREEN_CHNL_POS + 0] = 0;
    }
#endif
}

/**
 * @fn     void GH3X_Spo2AlgoChnlMapDefultSet(void)
 
 *
 * @brief  Set algo channel group Defult value
 *
 * @attention   
 *
 *
 * @return  None
 */
void GH3X_Spo2AlgoChnlMapDefultSet(void)
{
#if (__USE_GOODIX_SPO2_ALGORITHM__)
    if (!gstSpo2AlgoChnlMap.uchFlag)
    {
        GH3X_AlgoChnlMapInit(&gstSpo2AlgoChnlMap);
        gstSpo2AlgoChnlMap.uchNum = 1;
        gstSpo2AlgoChnlMap.uchAlgoChnlMap[ALGO_RED_CHNL_POS + 0] = 0;
        gstSpo2AlgoChnlMap.uchAlgoChnlMap[ALGO_IR_CHNL_POS + 0] = __SPO2_ALGORITHM_SUPPORT_CHNL_NUM__;
    }
#endif
}

void GH3X_BpAlgoChnlMapDefultSet(void)
{
#if (__USE_GOODIX_BP_ALGORITHM__)

#endif
}


void GH3X_WriteAlgParametersWithVirtualReg(GU8 uchParasIndex, GU32 unFunctionID, GS32* nAlgParametersBuffer, GU8 nBufferMaxLen, GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
    GU16 usValIndex = (usVirtualRegAddr - (unFunctionID * GH3X_VREG_FUNCTION_OFFSET + GH3X_ADT_ALG_CFG_ADDR)) / GH3X_REG_ADDR_SIZE;
    if ((GS16)((GS16)usValIndex / (GS16)GH3X_REG_ADDR_SIZE - (GS16)uchParasIndex < 0) ||\
        (usValIndex / GH3X_REG_ADDR_SIZE - uchParasIndex >= nBufferMaxLen))
    {
        return;
    }
    if ((usValIndex % GH3X_REG_ADDR_SIZE) != 0) // high word
    {
        nAlgParametersBuffer[usValIndex / GH3X_REG_ADDR_SIZE - uchParasIndex] = \
            GH3X_MAKEUP_DWORD2(usVirtualRegValue, GH3X_GET_LOW_WORD_FROM_DWORD(nAlgParametersBuffer[usValIndex / GH3X_REG_ADDR_SIZE - uchParasIndex]));

    }
    else // low word
    {
        nAlgParametersBuffer[usValIndex / GH3X_REG_ADDR_SIZE - uchParasIndex] = \
            GH3X_MAKEUP_DWORD2(GH3X_GET_HIGH_WORD_FROM_DWORD(nAlgParametersBuffer[usValIndex / GH3X_REG_ADDR_SIZE - uchParasIndex]), usVirtualRegValue);
    }
}

void GH3X_WriteAlgChnlMapWithVirtualReg(GU32 unFunctionID, STAlgoChnlMap* pstAlgChnlMap, GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
    GU16 usVirtualRegAddrOffset = usVirtualRegAddr - (unFunctionID * GH3X_VREG_FUNCTION_OFFSET + GH3X_ADT_ALG_CFG_ADDR);
    switch (usVirtualRegAddrOffset)
    {
    case GH3X_VREG_ALGO_CHNL_MAP_OFFSET:
        GH3X_AlgoChnlMapInit(pstAlgChnlMap);
        pstAlgChnlMap->uchNum = GH3X_GET_LOW_BYTE_FROM_WORD(usVirtualRegValue);
        if (pstAlgChnlMap->uchNum)
        {
            pstAlgChnlMap->uchFlag = 1;
        }
        break;
    case GH3X_VREG_ALGO_CHNL_MAP_OFFSET + GH3X_REG_ADDR_SIZE:
        pstAlgChnlMap->uchEcgAlgoChnlMap = (GU8)(usVirtualRegValue & 0x000000FF);
        break;
    default:
        pstAlgChnlMap->uchAlgoChnlMap[usVirtualRegAddrOffset - GH3X_VREG_ALGO_CHNL_MAP_OFFSET - 2 * GH3X_REG_ADDR_SIZE]\
            = GH3X_GET_LOW_BYTE_FROM_WORD(usVirtualRegValue);
        pstAlgChnlMap->uchAlgoChnlMap[usVirtualRegAddrOffset - GH3X_VREG_ALGO_CHNL_MAP_OFFSET - 2 * GH3X_REG_ADDR_SIZE + 1]\
            = GH3X_GET_HIGH_BYTE_FROM_WORD(usVirtualRegValue);
        break;
    }
}

void GH3X_WriteHrAlgConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
#if (__USE_GOODIX_HR_ALGORITHM__) // hr
    if (usVirtualRegAddr < GH3X_HR_ALGO_CHNL_NUM_ADDR)
    {
        GH3X_AlgoChnlMapInit(&gstHbaAlgoChnlMap);
        GH3X_WriteAlgParametersWithVirtualReg(0, GH3X_FUNC_OFFSET_HR, (GS32*)&gunHrParamsArr, GOODIX_HR_PARAMS_NUM_MAX, usVirtualRegAddr,usVirtualRegValue);
        //GH3X_ALGO_LOG_PARAM("[%s]:Hba Scenario = %d\r\n", __FUNCTION__, gunHrParamsArr[1]);
        if(HBA_SCENES_SLEEP == gunHrParamsArr[1])
        {
            GH3XSleepFlagSet(1);
        }
				else
        {
            GH3XSleepFlagSet(0);
				}
    }
    else
    {
        GH3X_WriteAlgChnlMapWithVirtualReg(GH3X_FUNC_OFFSET_HR, &gstHbaAlgoChnlMap, usVirtualRegAddr, usVirtualRegValue);
    }
#endif
}

void GH3X_WriteHrvAlgConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
#if (__USE_GOODIX_HRV_ALGORITHM__) // hrv
    GH3X_WriteAlgParametersWithVirtualReg(0, GH3X_FUNC_OFFSET_HRV, (GS32*)gnHrvParamsArr, GOODIX_HRV_PARAMS_NUM_MAX, usVirtualRegAddr, usVirtualRegValue);
#endif
}

void GH3X_WriteSpo2AlgConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
#if (__USE_GOODIX_SPO2_ALGORITHM__) // spo2
    if (usVirtualRegAddr < GH3X_SPO2_ALGO_CHNL_NUM_ADDR)
    {
        GH3X_AlgoChnlMapInit(&gstSpo2AlgoChnlMap);
        GH3X_WriteAlgParametersWithVirtualReg(0, GH3X_FUNC_OFFSET_SPO2, (GS32*)gnSpo2ParamssArr, GOODIX_SPO2_PARAMS_NUM_MAX, usVirtualRegAddr,usVirtualRegValue);
    }
    else
    {
        GH3X_WriteAlgChnlMapWithVirtualReg(GH3X_FUNC_OFFSET_SPO2, &gstSpo2AlgoChnlMap, usVirtualRegAddr, usVirtualRegValue);
    }
#endif
}

void GH3X_WriteEcgAlgConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
#if (__USE_GOODIX_ECG_ALGORITHM__) // ecg
    GH3X_WriteAlgParametersWithVirtualReg(0, GH3X_FUNC_OFFSET_ECG, (GS32*)gnEcgParamsArr, GOODIX_ECG_PARAMS_NUM_MAX, usVirtualRegAddr, usVirtualRegValue);
#endif
}

void GH3X_WriteBtAlgConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
#if (__USE_GOODIX_BT_ALGORITHM__) // bt
    GH3X_WriteAlgParametersWithVirtualReg(0, GH3X_FUNC_OFFSET_BT, (GS32*)gnBtParamsArr, GOODIX_BT_PARAMS_NUM_MAX, usVirtualRegAddr, usVirtualRegValue);
#endif
}

void GH3X_WriteAfAlgConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
#if (__USE_GOODIX_AF_ALGORITHM__) // af
    GH3X_WriteAlgParametersWithVirtualReg(0, GH3X_FUNC_OFFSET_AF, (GS32*)&gnAfParamsArr, GOODIX_AF_PARAMS_NUM_MAX, usVirtualRegAddr, usVirtualRegValue);
#endif
}
void GH3X_WriteGSRAlgConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
#if (__USE_GOODIX_GSR_ALGORITHM__) // GSR
    GH3X_WriteAlgParametersWithVirtualReg(0, GH3X_FUNC_OFFSET_GSR, (GS32*)&gnGsrPInfo, GOODIX_GSR_PARAMS_NUM_MAX, usVirtualRegAddr, usVirtualRegValue);
#endif
}
void GH3X_WriteBIAAlgConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
#if (__USE_GOODIX_BIA_ALGORITHM__) // BIA
		GH3X_WriteAlgParametersWithVirtualReg(0, GH3X_FUNC_OFFSET_BIA, (GS32*)&gnBiaPInfo, GOODIX_BIA_PARAMS_NUM_MAX, usVirtualRegAddr, usVirtualRegValue);
#endif
}


void GH3X_WriteBpAlgConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
#if (__USE_GOODIX_BP_ALGORITHM__) // fpbp

#endif
}


/**
 * @fn     GU16 GH3X_ReadAlgorithmConfigWithVirtualReg(GU16 usVirtualRegAddr)
 *
 * @brief  Read algorithm param config with virtual reg
 *
 * @attention   Virtual reg addr has del control bits, so reg addr is [0:11] valid.
 *
 * @param[in]   usVirtualRegAddr        virtual reg addr
 * @param[out]  None
 *
 * @return  virtual reg val
 */
GU16 GH3X_ReadAlgorithmConfigWithVirtualReg(GU16 usVirtualRegAddr)
{
    GU16 usVirtualRegData = 0;

    return usVirtualRegData;
}

void GH3X_WriteAlgConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
    if (usVirtualRegAddr < GH3X_REG_IS_VIRTUAL0X3_BIT)
    {
        return;
    }
    GU32 unFunctionID = (usVirtualRegAddr - GH3X_REG_IS_VIRTUAL0X7_BIT) / GH3X_VREG_FUNCTION_OFFSET;
    if (usVirtualRegAddr >= GH3X_ADT_ALG_CFG_ADDR + GH3X_VREG_FUNCTION_OFFSET * unFunctionID &&\
             usVirtualRegAddr < GH3X_ADT_DRV_CFG_ADDR + GH3X_VREG_FUNCTION_OFFSET * (unFunctionID + 1))
    {
        switch (unFunctionID)
        {
        case GH3X_FUNC_OFFSET_HR:
            GH3X_WriteHrAlgConfigWithVirtualReg(usVirtualRegAddr, usVirtualRegValue);
            break;
        case GH3X_FUNC_OFFSET_HRV:
            GH3X_WriteHrvAlgConfigWithVirtualReg(usVirtualRegAddr, usVirtualRegValue);
            break;
        case GH3X_FUNC_OFFSET_FPBP:
        case GH3X_FUNC_OFFSET_PWA:
            //GH3X_WriteBpAlgConfigWithVirtualReg(usVirtualRegAddr, usVirtualRegValue);
            break;
        case GH3X_FUNC_OFFSET_SPO2:
            GH3X_WriteSpo2AlgConfigWithVirtualReg(usVirtualRegAddr, usVirtualRegValue);
            break;
        case GH3X_FUNC_OFFSET_ECG:
            GH3X_WriteEcgAlgConfigWithVirtualReg(usVirtualRegAddr, usVirtualRegValue);
            break;
        case GH3X_FUNC_OFFSET_BT:
            GH3X_WriteBtAlgConfigWithVirtualReg(usVirtualRegAddr, usVirtualRegValue);
            break;
        case GH3X_FUNC_OFFSET_AF:
            GH3X_WriteAfAlgConfigWithVirtualReg(usVirtualRegAddr, usVirtualRegValue);
        case GH3X_FUNC_OFFSET_BIA:
            GH3X_WriteBIAAlgConfigWithVirtualReg(usVirtualRegAddr, usVirtualRegValue);
            break;
        case GH3X_FUNC_OFFSET_GSR:
            GH3X_WriteGSRAlgConfigWithVirtualReg(usVirtualRegAddr, usVirtualRegValue);
            break;
        default:
            break;
        }
    }
}


void GH3X_AlgConfigDeinit(void)
{
    #if __USE_GOODIX_HR_ALGORITHM__
    GH3X_AlgoChnlMapInit(&gstHbaAlgoChnlMap);
    GH3X_Memset(gunHrParamsArr, 0, GOODIX_HR_PARAMS_NUM_MAX * sizeof(gunHrParamsArr[0]));
    #endif
    
    #if __USE_GOODIX_SPO2_ALGORITHM__
    GH3X_AlgoChnlMapInit(&gstSpo2AlgoChnlMap);
    GH3X_Spo2AlgoChnlMapDefultSet();
    GH3X_Memset(gnSpo2ParamssArr, 0, GOODIX_SPO2_PARAMS_NUM_MAX * sizeof(gnSpo2ParamssArr[0]));
    #endif
    #if __USE_GOODIX_BIA_ALGORITHM__
    GH3X_Memset(gnBiaPInfo, 0, GOODIX_BIA_PARAMS_NUM_MAX * sizeof(gnBiaPInfo[0]));
    #endif
    #if __USE_GOODIX_ECG_ALGORITHM__
    GH3X_Memset(gnEcgParamsArr, 0, GOODIX_ECG_PARAMS_NUM_MAX * sizeof(gnEcgParamsArr[0]));
    #endif
    #if __USE_GOODIX_GSR_ALGORITHM__
    GH3X_Memset(gnGsrPInfo, 0, GOODIX_GSR_PARAMS_NUM_MAX * sizeof(gnGsrPInfo[0]));
    #endif
}


#endif











/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
