/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x_drv_control.c
 *
 * @brief   gh3x control functions
 *
 * @version ref gh3x_drv_version.h
 *
 */


#include <stdio.h>
#include "gh3x_demo_config.h"
#include "gh3x_drv_version.h"
#include "gh3x_drv.h"
#include "gh3x_drv_interface.h"
#include "gh3x_drv_control.h"
#include "gh3x_drv_registers.h"
#include "gh3x_fifo_analyze.h"

static GU32 gunSoftEvent = 0;
static STPPGSlotInfo gstPPGSlotInfo;
static STGh3xFifoMonitorInfo gstFifoMonitorInfo = {0};
/// init hook func ptr
pfnNormalHookFunc gpInitHookFunc = GH3X_PTR_NULL;

/// start hook func ptr
pfnNormalHookFunc gpStartHookFunc = GH3X_PTR_NULL;

/// stop hook func ptr
pfnNormalHookFunc gpStopHookFunc = GH3X_PTR_NULL;

/// get rawdata hook func ptr
pfnGetRawdataHookFunc gpGetRawdataHookFunc = GH3X_PTR_NULL;

/// set reset pin level spi
pfnSetPinLevel gpResetPinLevelControlFunc = GH3X_PTR_NULL;

/// delay us func ptr
pfnDelayTime gpDelayUsFunc = GH3X_PTR_NULL;

/// delay Ms func ptr
pfnDelayTime gpDelayMsFunc = GH3X_PTR_NULL;

/// chip version
//static char gszChipVerString[50] = {0};

/// fifo power ctrl fifo control reg cache var
GU16 gusPmuFifoModuleCtrlVal = 0;

/// for fifo power ctrl cache status
GU8 guchPmuFifoModuleCtrlCacheStatus = 0;

/// gh3x status
GU8 guchInitStatus = 0;

/// max rawdata num read from fifo every time
GU16 gusMaxNumReadFromFifo = 0;

/// function started bitmap, use for sampling control
GU32 gunFuncStartedBitmap = 0;
void Gh3xFifoMonitorInfoInit(void);
void Gh3xFifoMonitorInfoDeInit(void);
GU32 DRV_DSP_RawdataCalc(GU32 unRawdata);
GS32 GetLen(unsigned int data);
GU32 DRV_DSP_RawdataCalc(GU32 unRawdata);
GU32 DRV_DSP_ChipRawdataCalc(GU32 unRawdata);
void GH3X_CalRawdataBuf(GU8 *puchRawdata, GU16 usRawdataLen);



STPPGSlotInfo* get_stPPGSlotInfo(void)
{
   return  &gstPPGSlotInfo; 
}
 
 

/**
 * @fn     GU8 DRV_DSP_ChipRawdataCheck(GU32 unRawdata)
 *
 * @brief  rawdata check last byte in algo lib
 *
 * @attention   none
 *
 * @param[in]   unRawdata   single rawdata
 * @param[out]  None
 *
 * @return  errcode
 * @retval  0               check success
 * @retval  1               check failed
 */
GU8 DRV_DSP_ChipRawdataCheck(GU32 unRawdata)
{
    GU32 unRawdataTemp = DRV_DSP_ChipRawdataCalc(unRawdata);
    return(unRawdataTemp != unRawdata);
}

void GhClearFifo(void)
{
    GU16 usPpgStart = GH3X_ReadRegBitField(GH3X_REG_SLOT_SAMPLE_START_ADDR,GH3X_REG_SLOT_SAMPLE_START_LSB,GH3X_REG_SLOT_SAMPLE_START_MSB);
    GU16 usTempEcgEn = GH3X_ReadRegBitField(GH3X_REG_ECG_CTRL0_ADDR,GH3X_REG_ECG0_EN_BIT,GH3X_REG_ECGDC_EN_BIT);
    GU16 usTempGsrEn = GH3X_ReadRegBitField(GH3X_REG_GSR_CTRL1_ADDR,GH3X_REG_GSR_EN_BIT,GH3X_REG_GSR_EN_BIT);
    GU16 usTempBiaEn = GH3X_ReadRegBitField(GH3X_REG_BIA_CTRL0_ADDR,GH3X_REG_BIA_EN_BIT,GH3X_REG_BIA_EN_BIT);
    if(usTempEcgEn)
    {
        GH3X_WriteRegBitField(GH3X_REG_ECG_CTRL0_ADDR, GH3X_REG_ECG0_EN_BIT, GH3X_REG_ECGDC_EN_BIT,0);
    }
    if(usTempGsrEn)
    {
        GH3X_WriteRegBitField(GH3X_REG_GSR_CTRL1_ADDR, GH3X_REG_GSR_EN_BIT, GH3X_REG_GSR_EN_BIT, 0);
    }
    if(usTempBiaEn)
    {
        GH3X_WriteRegBitField(GH3X_REG_BIA_CTRL0_ADDR, GH3X_REG_BIA_EN_BIT, GH3X_REG_BIA_EN_BIT, 0);
    }
    if(usPpgStart)
    {
        GH3X_WriteRegBitField(GH3X_REG_SLOT_SAMPLE_START_ADDR, GH3X_REG_SLOT_SAMPLE_START_LSB, GH3X_REG_SLOT_SAMPLE_START_MSB, 0);
    }
    GU16 fifoen = GH3X_ReadRegBitField(0x0008,14,15);
    GH3X_WriteRegBitField(0x0008,14,15,0);
    GH3X_WriteRegBitField(0x0008,14,15,fifoen);
    if(usTempEcgEn)
    {
        GH3X_WriteRegBitField(GH3X_REG_ECG_CTRL0_ADDR, GH3X_REG_ECG0_EN_BIT, GH3X_REG_ECGDC_EN_BIT,usTempEcgEn);
    }
    if(usTempGsrEn)
    {
        GH3X_WriteRegBitField(GH3X_REG_GSR_CTRL1_ADDR, GH3X_REG_GSR_EN_BIT, GH3X_REG_GSR_EN_BIT, usTempGsrEn);
    }
    if(usTempBiaEn)
    {
        GH3X_WriteRegBitField(GH3X_REG_BIA_CTRL0_ADDR, GH3X_REG_BIA_EN_BIT, GH3X_REG_BIA_EN_BIT, usTempBiaEn);
    }
    if(usPpgStart)
    {
        GH3X_WriteRegBitField(GH3X_REG_SLOT_SAMPLE_START_ADDR, GH3X_REG_SLOT_SAMPLE_START_LSB, GH3X_REG_SLOT_SAMPLE_START_MSB, usPpgStart);
    }
 
}

GU8 GH3X_CheckRawdataBuf(GU8 *puchRawdata, GU16* pusRawdataLen) 
{ 
    GU8 ret = 0;
    GU16 len = *pusRawdataLen;
    GU8* puchRawdatabuf = puchRawdata;
    static int cnt = 0;
    if((cnt++)>= 20)
    {
        // puchRawdatabuf++;
        // puchRawdatabuf++;
        // len -= 5;
    }

    for (GU16 usIndex = 0; usIndex < len; usIndex += GH3X_FIFO_RAWDATA_SIZE)
    {
        GU16 usTag = ((((GU16)puchRawdatabuf[4 + usIndex]) << 8) | ((GU16)puchRawdatabuf[3 + usIndex]));
        GU8 uchDatatype = ((usTag >> 13) & 0x7);
        GU8 uchEcgtype = ((usTag >> 11) & 0x3);
#if __FIFO_MONITOR_EN__
        if(gstFifoMonitorInfo.usNeedSkipFifoNum > 0)
        {
            gstFifoMonitorInfo.usNeedSkipFifoNum--;
        }

        if(uchDatatype == 0 && gstFifoMonitorInfo.usNeedSkipFifoNum == 0 && gstFifoMonitorInfo.uchInitFlag == 1)
        {
            GU8 slotcfg = (usTag >> 8) & 0x1F;
            GU8 adcNum = (usTag >> 2) & 0x03;
            if(gstFifoMonitorInfo.uchLowestChnlAppearedFlag == 1 && slotcfg != gstFifoMonitorInfo.uchLowestChnlSlotcfg)
            {
                gstFifoMonitorInfo.usAllChnlRealTimeSampleCnt++;
            }
            if(gstFifoMonitorInfo.usAllChnlRealTimeSampleCnt > gstFifoMonitorInfo.usEstiAllChnlSampleCntlimit)
            {
                ret = 1;
                break;
            }
            if(slotcfg == gstFifoMonitorInfo.uchLowestChnlSlotcfg && adcNum == gstFifoMonitorInfo.uchLowestChnlAdcNum)
            {
                if(gstFifoMonitorInfo.uchLowestChnlAppearedFlag == 1 
                   && gstFifoMonitorInfo.usAllChnlRealTimeSampleCnt != gstFifoMonitorInfo.usEstiAllChnlSampleCntlimit)
                {
                    ret = 1;
                    break;
                }
                else if(gstFifoMonitorInfo.uchLowestChnlAppearedFlag == 1)
                {
                    GH3X_SAMPLE_LOG_PARAM("gstFifoMonitorInfo.usAllChnlRealTimeSampleCnt == gstFifoMonitorInfo.usEstiAllChnlSampleCntlimit\r\n");
                }
                gstFifoMonitorInfo.uchLowestChnlAppearedFlag = 1;
                gstFifoMonitorInfo.usAllChnlRealTimeSampleCnt = 0;
            }
        }
        else
#endif
        {
            if ((uchDatatype > 4 && uchDatatype <= 6) || (uchDatatype == 4 && uchEcgtype == 0))
            {
                GU32 usRawdataTemp = GH3X_MAKEUP_DWORD(0,puchRawdatabuf[usIndex + 2],
                                                    puchRawdatabuf[usIndex + 1],
                                                    puchRawdatabuf[usIndex + 0]);
                ret = DRV_DSP_ChipRawdataCheck(usRawdataTemp);
                if(ret)
                {
                    break;
                }
            }
        }
    }
    GH3X_SAMPLE_LOG_PARAM("gstFifoMonitorInfo = %d,%d,%d,%d,%d,%d,cnt = %d,ret = %d\r\n",gstFifoMonitorInfo.uchLowestChnlSlotcfg, 
                                                                          gstFifoMonitorInfo.uchLowestChnlAdcNum, 
                                                                          gstFifoMonitorInfo.uchLowestChnlAppearedFlag, 
                                                                          gstFifoMonitorInfo.usNeedSkipFifoNum, 
                                                                          gstFifoMonitorInfo.usAllChnlRealTimeSampleCnt, 
                                                                          gstFifoMonitorInfo.usEstiAllChnlSampleCntlimit,
                                                                          cnt,
                                                                          ret);
    if(ret)
    {
#if __FIFO_MONITOR_EN__
        Gh3xFifoMonitorInfoInit();
#endif
        GhClearFifo();
        *pusRawdataLen = 0;
        cnt = 0;
    }
    return ret;
}





/**
 * @fn     void GH3X_SetMaxNumWhenReadFifo(GU16 usMaxNum)
 *
 * @brief  Set max number of rawdata read from fifo every time
 *
 * @attention   None.
 *
 * @param[in]   usMaxNum        max number of rawdata read from fifo
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_SetMaxNumWhenReadFifo(GU16 usMaxNum)
{
    gusMaxNumReadFromFifo = usMaxNum;
}

/* util function, memcpy & memset & strlen */

/**
 * @fn     void *GH3X_Memcpy(void *pDest, const void *pSrc, GU32 unByteSize)
 *
 * @brief  memcpy() Re-implementation
 *
 * @attention   None
 *
 * @param[in]   pSrc        pointer to source buffer
 * @param[in]   unByteSize  source buffer byte size
 * @param[out]  pDest       pointer to destination buffer
 *
 * @return  pointer to destination buffer
 */
void *GH3X_Memcpy(void *pDest, const void *pSrc, GU32 unByteSize)
{
    GU8 *puchSrc = (GU8 *)pSrc;
    GU8 *puchDest = (GU8 *)pDest;
    GU32 unAlign = ((GU32)puchDest | (GU32)puchSrc) << GH3X_UPROTOCOL_ALIGN_LEFTSHIFT;

    if ((pDest == GH3X_PTR_NULL) || (pSrc == GH3X_PTR_NULL))
    {
        return GH3X_PTR_NULL;
    }
    if (unAlign == 0) // if align 4
    {
        while (unByteSize >= GH3X_UPROTOCOL_SIZE_T)
        {
            *(GU32 *)puchDest = *(GU32 *)puchSrc;
            puchSrc += GH3X_UPROTOCOL_SIZE_T;
            puchDest += GH3X_UPROTOCOL_SIZE_T;
            unByteSize -= GH3X_UPROTOCOL_SIZE_T;
        }
    }
    while (unByteSize) // bytes
    {
        *(GU8 *)puchDest = *(GU8 *)puchSrc;
        puchSrc++;
        puchDest++;
        unByteSize--;
    }
    return pDest;
}

/**
 * @fn     void *GH3X_Memset(void* pDest, GCHAR chVal, GU32 unByteSize)
 *
 * @brief  memset() Re-implementation
 *
 * @attention   None
 *
 * @param[in]   chVal       char val for set
 * @param[in]   unByteSize       source buffer len
 * @param[out]  pDest       pointer to destination buffer
 *
 * @return  pointer to destination buffer
 */
void *GH3X_Memset(void* pDest, GCHAR chVal, GU32 unByteSize)
{
    GU8 *puchDest = (GU8 *)pDest;
    GU32 unAlign = ((GU32)puchDest) << GH3X_UPROTOCOL_ALIGN_LEFTSHIFT;
    GU32 unWordVal = GH3X_MAKEUP_DWORD(chVal, chVal, chVal, chVal);

    if (pDest == GH3X_PTR_NULL)
    {
        return GH3X_PTR_NULL;
    }
    if (unAlign == 0) // if align 4
    {
        while (unByteSize >= GH3X_UPROTOCOL_SIZE_T)
        {
            *(GU32 *)puchDest = unWordVal;
            puchDest += GH3X_UPROTOCOL_SIZE_T;
            unByteSize -= GH3X_UPROTOCOL_SIZE_T;
        }
    }
    while (unByteSize) // bytes
    {
        *(GU8 *)puchDest = (GU8)chVal;
        puchDest++;
        unByteSize--;
    }
    return pDest;
}

/**
 * @fn     GU32 GH3X_Strlen(const GCHAR *pszSrc)
 *
 * @brief  strlen() Re-implementation
 *
 * @attention   None
 *
 * @param[in]   pszSrc      pointer to string
 * @param[out]  None
 *
 * @return  string len
 */
GU32 GH3X_Strlen(const GCHAR *pszSrc)
{
    GU32 unCnt = 0;
    if (pszSrc != GH3X_PTR_NULL)
    {
        while ((*pszSrc) != 0) // if not equal '\0'
        {
            unCnt++;
            pszSrc++;
        }
    }
    return unCnt;
}

/**
 * @fn     GU16 GH3X_FindGu16MinVal(GU16 *pusBuffer, GU8 uchLen)
 *
 * @brief  Find min val
 *
 * @attention   len must > 0
 * 
 * @param[in]   pusBuffer        pointer to buffer
 * @param[in]   uchLen           buffer length
 * @param[out]  None
 *
 * @return  min val, if len = 0, return 0
 */
GU16 GH3X_FindGu16MinVal(GU16 *pusBuffer, GU8 uchLen)
{
    GU16 usMinVal = pusBuffer[0];
    GU8 uchIndex = 1;

    for (uchIndex = 1; uchIndex < uchLen; uchIndex++)
    {
        if (pusBuffer[uchIndex] < usMinVal)
        {
            usMinVal = pusBuffer[uchIndex];
        }
    }
    return usMinVal; 
}

/**
 * @fn     GU16 GH3X_FindGu16MaxVal(GU16 *pusBuffer, GU8 uchLen)
 *
 * @brief  Find max val
 *
 * @attention   len must > 0
 * 
 * @param[in]   pusBuffer        pointer to buffer
 * @param[in]   uchLen           buffer length
 * @param[out]  None
 *
 * @return  max val, if len = 0, return 0
 */
GU16 GH3X_FindGu16MaxVal(GU16 *pusBuffer, GU8 uchLen)
{
    GU16 usMaxVal = pusBuffer[0];
    GU8 uchIndex = 1;

    for (uchIndex = 1; uchIndex < uchLen; uchIndex++)
    {
        if (pusBuffer[uchIndex] > usMaxVal)
        {
            usMaxVal = pusBuffer[uchIndex];
        }
    }
    return usMaxVal; 
}

/**
 * @fn     void GH3X_FindGu16MaxMinVal(GU16 *pusMaxVal, GU16 *pusMinVal, GU16 *pusBuffer, GU8 uchLen)
 *
 * @brief  Find min val & max val
 *
 * @attention   len must > 0, ptr not null
 * 
 * @param[in]   pusBuffer        pointer to buffer
 * @param[in]   uchLen           buffer length
 * @param[out]  pusMaxVal        pointer to max val
 * @param[out]  pusMinVal        pointer to min val
 *
 * @return  None
 */
void GH3X_FindGu16MaxMinVal(GU16 *pusMaxVal, GU16 *pusMinVal, GU16 *pusBuffer, GU8 uchLen)
{
    GU16 usMaxVal = pusBuffer[0];
    GU16 usMinVal = pusBuffer[0];
    GU8 uchIndex = 1;

    for (uchIndex = 1; uchIndex < uchLen; uchIndex++)
    {
        if (pusBuffer[uchIndex] > usMaxVal)
        {
            usMaxVal = pusBuffer[uchIndex];
        }
        if (pusBuffer[uchIndex] < usMinVal)
        {
            usMinVal = pusBuffer[uchIndex];
        }
    }
    SET_VAL_VIA_PTR(pusMaxVal, usMaxVal);
    SET_VAL_VIA_PTR(pusMinVal, usMinVal);
}

/**
 * @fn     GCHAR *GH3X_GetChipVersion(void)
 *
 * @brief  Get chip version, only for protocol, format like v11_ev01(11: chip id, 01 efuse ver)
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  chip version string
 */
GCHAR *GH3X_GetChipVersion(void)
{
 
    return "no snprintf";
    
};

/**
 * @fn     GCHAR *GH3X_GetDriverLibVersion(void)
 *
 * @brief  Get driver version
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  library version string
 */
GCHAR *GH3X_GetDriverLibVersion(void)
{
    return (GCHAR *)GH3X_VERSION_STRING;
}

/**
 * @fn     void GH3X_RegisterHookFunc(void (*pInitHookFunc)(void),
 *                            void (*pStartHookFunc)(void),
 *                           void (*pStopHookFunc)(void),
 *                            void (*pGetRawdataHookFunc)(GU8 *puchReadDataBuffer, GU16 usReadDataLen))
 *
 * @brief  Register hook function callback
 *
 * @attention   None
 *
 * @param[in]   pInitHookFunc           pointer to init hook function 
 * @param[in]   pStartHookFunc          pointer to start hook function 
 * @param[in]   pStopHookFunc           pointer to stop hook function 
 * @param[in]   pGetRawdataHookFunc     pointer to get rawdata hook function
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_RegisterHookFunc(void (*pInitHookFunc)(void),
                             void (*pStartHookFunc)(void),
                             void (*pStopHookFunc)(void),
                             void (*pGetRawdataHookFunc)(GU8 *puchReadDataBuffer, GU16 usReadDataLen),
                             void (*pResetPinLevelControlFunc)(GU8 uchPinLevel),
                             void (*pPlatformDelayUsFunc)(GU16 usUsec),
                             void (*pPlatformDelayMsFunc)(GU16 usMsec))
{
    gpInitHookFunc = pInitHookFunc;
    gpStartHookFunc = pStartHookFunc;
    gpStopHookFunc = pStopHookFunc;
    gpGetRawdataHookFunc = pGetRawdataHookFunc;
    gpResetPinLevelControlFunc = pResetPinLevelControlFunc;
    gpDelayUsFunc = pPlatformDelayUsFunc;
    gpDelayMsFunc = pPlatformDelayMsFunc;
    GH3X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
}

/**
 * @fn     GS8 GH3X_ExitLowPowerMode(void)
 *
 * @brief  Exit lowpower mode, in this mode, can read&write reg with gh3x
 *
 * @attention   Power consumption bigger than lowpower mode, detaile ref gh3x datasheet
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  errcode
 * @retval  #GH3X_RET_OK               return successfully
 * @retval  #GH3X_RET_GENERIC_ERROR    exit low power mode error
 */
GS8 GH3X_ExitLowPowerMode(void)
{
    GS8 chRet = GH3X_RET_GENERIC_ERROR;
    GH3X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
    GH3X_CHIP_WAKEUP(chRet);
    return chRet;
}

/**
 * @fn     GS8 GH3X_EnterLowPowerMode(void)
 *
 * @brief  Enter lowpower mode, in this mode, can't read&write reg with gh3x.
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  errcode
 * @retval  #GH3X_RET_OK               return successfully
 * @retval  #GH3X_RET_GENERIC_ERROR    enter low power mode error
 */
GS8 GH3X_EnterLowPowerMode(void)
{
    GS8 chRet = GH3X_RET_GENERIC_ERROR;
    GH3X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
    GH3X_CHIP_SLEEP(chRet);
    return chRet;
}

/**
 * @fn     GS8 GH3X_CommunicateConfirm(void)
 *
 * @brief  Communication operation interface confirm
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  errcode
 * @retval  #GH3X_RET_OK               return successfully
 * @retval  #GH3X_RET_COMM_ERROR       gh3x communicate error
 */
GS8 GH3X_CommunicateConfirm(void)
{
    GU8 uchIndex = GH3X_COMMUNICATE_CONFIRM_MAX_CNT;
    GS8 chRet = GH3X_RET_COMM_ERROR;
    GU16 uchReadData;
    GU16 uchWriteData;

    GH3X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
    do
    {
        if (GH3X_ReadReg(GH3X_CHIP_READY_CODE_REG_ADDR) == GH3X_CHIP_READY_REG_VAL)
        {
            uchReadData = GH3X_ReadReg(GH3X_REG_TIMER_CTRL_ADDR);
            GH3X_DEBUG_LOG_PARAM("slot_time0 reg = %d\r\n", (int)uchReadData);
            uchWriteData = ~uchReadData;
            GH3X_WriteReg(GH3X_REG_TIMER_CTRL_ADDR, uchWriteData);
            uchReadData = GH3X_ReadReg(GH3X_REG_TIMER_CTRL_ADDR);
            GH3X_DEBUG_LOG_PARAM("slot_time0 reg = %d\r\n", (int)uchReadData);
            if (uchWriteData == uchReadData)
            {
                chRet = GH3X_RET_OK;
                uchWriteData = ~uchReadData;
                GH3X_WriteReg(GH3X_REG_TIMER_CTRL_ADDR, uchWriteData);
                break;
            }
        }
        else
        {
            GH3X_DEBUG_LOG_PARAM("ready code reg is error !!! %s\r\n", __FUNCTION__);
        }
        uchIndex--;
    } while (uchIndex > 0);
    return chRet;
}

/**
 * @fn     GS8 GH3X_Init(const STInitConfig *pstInitConfigParam)
 *
 * @brief  Init gh3x with configure parameters
 *
 * @attention   None
 *
 * @param[in]   pstInitConfigParam      pointer to gh3x init config param
 * @param[out]  None
 *
 * @return  errcode
 * @retval  #GH3X_RET_OK               return successfully
 * @retval  #GH3X_RET_COMM_ERROR       gh3x communicate error
 */
GS8 GH3X_Init(const STInitConfig *pstInitConfigParam)
{
    guchInitStatus = GH3X_STATUS_NO_INIT;

    GH3X_DEBUG_LOG_PARAM("%s : Lib Version : %s\r\n", __FUNCTION__, GH3X_GetDriverLibVersion());
    if (GH3X_CommunicateConfirm() != GH3X_RET_OK) /* confirm GH3X communicate. */
    {
        GH3X_DEBUG_LOG_PARAM("communicate confirm error!\r\n");
        return GH3X_RET_COMM_ERROR;
    }

    /* clear int status. */
    GU16 usEvent[2+5] = {0xFFFF,0xFFFF};
    GH3X_WriteRegs(GH3X_REG_INT_EVENT1_ADDR, usEvent,2);

    if (pstInitConfigParam != GH3X_PTR_NULL)
    {
        /* load config */
        if (GH3X_LoadNewRegConfigArr(pstInitConfigParam->pstRegConfigArr,
            pstInitConfigParam->usConfigArrLen, 0) != GH3X_RET_OK)
        {
            return GH3X_RET_COMM_ERROR;
        }
    }
    else
    {
        GH3X_DEBUG_LOG_PARAM("gh3x init param is null!\r\n");
    }

    /* call hook */
    HOOK_FUNC_CALL(gpInitHookFunc, () );

    /* set chip status inited. */
    GH3X_SET_CHIP_INIED();
    guchInitStatus = GH3X_STATUS_INITED;
    return GH3X_RET_OK;
}

/**
 * @fn     GS8 GH3X_StartSampling(void)
 *
 * @brief  Start gh3x sampling
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  errcode
 * @retval  #GH3X_RET_OK                  return successfully
 * @retval  #GH3X_RET_NO_INITED_ERROR     gh3x has not inited
 * @retval  #GH3X_RET_GENERIC_ERROR       gh3x has started, need restart should call GH3X_StopSampling then start
 */
GS8 GH3X_StartSampling(void)
{
    GS8 chRet = GH3X_RET_GENERIC_ERROR;
    GH3X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
    if (guchInitStatus != GH3X_STATUS_STARTED)
    {
        if (guchInitStatus == GH3X_STATUS_INITED)
        {
            //GH3X_REG_SLOT_SAMPLE_START_ADDR
            //GH3X_WriteRegBitField((GU32)GH3X_REG_SLOT_SAMPLE_START_ADDR, (GU8)GH3X_REG_SLOT_SAMPLE_START_LSB, (GU8)GH3X_REG_SLOT_SAMPLE_START_MSB, 1);
            chRet = GH3X_RET_OK;
        }
        else
        {
            guchInitStatus = GH3X_STATUS_NO_INIT;
            chRet = GH3X_RET_NO_INITED_ERROR;
            GH3X_DEBUG_LOG_PARAM("gh3x start error that don't inited!\r\n");
        }
    }
    else
    {
        GH3X_DEBUG_LOG_PARAM("gh3x started,restart call stop->start!\r\n");
    }

    return chRet;
}

/**
 * @fn     GS8 GH3X_StopSampling(void)
 *
 * @brief  Stop gh3x sampling
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 * 
 * @return  errcode
 * @retval  #GH3X_RET_OK                  return successfully
 * @retval  #GH3X_RET_NO_INITED_ERROR     gh3x has not inited
 */
GS8 GH3X_StopSampling(void)
{
    GS8 chRet = GH3X_RET_NO_INITED_ERROR;

    GH3X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
    if ((guchInitStatus == GH3X_STATUS_INITED) || (guchInitStatus == GH3X_STATUS_STARTED))
    {
        guchInitStatus = GH3X_STATUS_INITED;
        //GH3X_WriteRegBitField((GU32)GH3X_REG_SLOT_SAMPLE_START_ADDR, (GU8)GH3X_REG_SLOT_SAMPLE_START_LSB, (GU8)GH3X_REG_SLOT_SAMPLE_START_MSB, 0);
        HOOK_FUNC_CALL(gpStopHookFunc, () );  /* call hook */
        chRet = GH3X_RET_OK;
    }
    else
    {
        GH3X_DEBUG_LOG_PARAM("gh3x stop error that don't inited!\r\n");
    }

    return chRet;
}

/**
 * @fn     GU8 GH3X_ConfirmLeadStatus(GU8 uchCmp, GU8 uchIq, GU8 uchLogic)
 *
 * @brief  confirm lead status
 *
 * @attention   None
 *
 * @param[in]   uchCmp
 * @param[out]  uchIq
 * @param[out]  uchLogic
 * @return  ret
 */
GU8 GH3X_ConfirmLeadStatus(GU8 uchCmp, GU8 uchIq, GU8 uchLogic)
{
    GU8 ret = 0;
    GH3X_DEBUG_LOG_PARAM("%s uchCmp = 0%d,uchIq = 0%d,uchLogic = 0%d,\r\n", __FUNCTION__,uchCmp,uchIq,uchLogic);
    if(uchLogic == 0)
    {
        ret = uchCmp;
    }
    else if(uchLogic == 1)
    {
        ret = uchIq;
    }
    else if(uchLogic == 2)
    {
        ret = (uchIq & uchCmp);
    }
    else if(uchLogic == 3)
    {
        ret = (uchIq | uchCmp);
    }
    return ret;
}

/**
 * @fn     void GH3X_ConfirmIrqStatus(GU32* unGotEvent)
 *
 * @brief  Handle some irq event situation
 *
 * @attention   None
 *
 * @param[in]   unGotEvent
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_ConfirmIrqStatus(GU32* unGotEvent)
{
    GU8 lead_on_and_off[3];
    lead_on_and_off[2] = ((*unGotEvent & (GH3X_IRQ_MSK_GSR_LEAD_OFF_BIT)) && (*unGotEvent & (GH3X_IRQ_MSK_GSR_LEAD_ON_BIT))) ;
    lead_on_and_off[1] = ((*unGotEvent & (GH3X_IRQ_MSK_ECG1_LEAD_OFF_BIT)) && (*unGotEvent & (GH3X_IRQ_MSK_ECG1_LEAD_ON_BIT))) ;
    lead_on_and_off[0] = ((*unGotEvent & (GH3X_IRQ_MSK_ECG0_LEAD_OFF_BIT)) && (*unGotEvent & (GH3X_IRQ_MSK_ECG0_LEAD_ON_BIT))) ;
                           
    if(lead_on_and_off[0] || lead_on_and_off[1] || lead_on_and_off[2])
    {
        GU16 cmp_iq_logic = GH3X_ReadReg(0x0538);
        GU16 cmp_iq_status = GH3X_ReadReg(0x0610);
        for(GU8 i = 0; i < sizeof(lead_on_and_off) ;i++)
        {
            if(lead_on_and_off[i])
            {
                GU16 logic_ch = (cmp_iq_logic >> (i * 3)) & 0x07;
                if(logic_ch == 0)
                {
                    GU8 bit = (i + 1) % 3;
                    //only cmp
                    lead_on_and_off[i] = (cmp_iq_status >> bit) & 0x01;
                }
                else if(logic_ch == 1)
                {
                    //only iq
                    GU8 bit = (i + 8);
                    lead_on_and_off[i] = (cmp_iq_status >> bit) & 0x01;
                }
                else
                {
                    lead_on_and_off[i] = 0;
                }
                GU8 intm_o1_bit = ((i + 1) % 3) * 2 + 2 - 1;
                *unGotEvent = *unGotEvent & (~(1 << intm_o1_bit));
                *unGotEvent = *unGotEvent & (~(1 << (intm_o1_bit + 1)));
                if(lead_on_and_off[i])
                {
                    *unGotEvent = *unGotEvent | ((1 << intm_o1_bit));
                }
                else
                {
                    *unGotEvent = *unGotEvent | (1 << (intm_o1_bit + 1));
                }
            }
        }
    }

    if((*unGotEvent & (GH3X_IRQ_MSK_GSR_LEAD_OFF_BIT)) || (*unGotEvent & (GH3X_IRQ_MSK_GSR_LEAD_ON_BIT)))
    {
        GU16 chip_id = GH3X_ReadReg(0x04);
        if(chip_id != 0x3031 && chip_id != 0x0)
        {
             return;
        }
        if(*unGotEvent & (GH3X_IRQ_MSK_GSR_LEAD_OFF_BIT))
        {
            *unGotEvent &= (~GH3X_IRQ_MSK_GSR_LEAD_OFF_BIT);
            *unGotEvent |= GH3X_IRQ_MSK_ECG1_LEAD_OFF_BIT;
        }
        if(*unGotEvent & (GH3X_IRQ_MSK_GSR_LEAD_ON_BIT))
        {
            *unGotEvent &= (~GH3X_IRQ_MSK_GSR_LEAD_ON_BIT);
            //*unGotEvent |= GH3X_IRQ_MSK_ECG1_LEAD_ON_BIT;
        }
    }
    
}

/**
 * @fn     GU32 GH3X_GetIrqStatus(void)
 *
 * @brief  Get irq status reg val
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  irq status val, ref irq status mask
 */
GU32 GH3X_GetIrqStatus(void)
{
    GU16 usEvent[2+5];
    GH3X_ReadRegs(GH3X_REG_INT_EVENT1_ADDR, usEvent, 2);
    GU32 ret = ((GU32)usEvent[0]&0xFFFF) + (((GU32)usEvent[1] << 16)&0XFFFF0000);
    GH3X_WriteRegs(GH3X_REG_INT_EVENT1_ADDR, usEvent,2);
    GH3X_DEBUG_LOG_PARAM("%s event = 0x%8x\r\n", __FUNCTION__,ret);
    GH3X_ConfirmIrqStatus(&ret);
    return ret;
}
void GH3X_ChangeFifoEndian(GU8 *puchRawdataBuf,int len)
{
    if(len > 10)
        return;
    char tempst[10];
    for(int i = 0 ; i < len;i++)
    {
        tempst[i] = (puchRawdataBuf[i]);
    }
    for(int i = 0 ; i < len;i++)
    {
        puchRawdataBuf[i] = tempst[len - i -1];
    }
}


/**
 * @fn     GS16 GH3X_ReadFifodata(GU8 *puchReadFifoData, GU16* pusReadFifoDataLen)
 *
 * @brief  Read GH3X_ Fifo Data
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  puchReadFifoData      pointer to GH3X_ fifo data
 * @param[out]  pusReadFifoDataLen          pointer to GH3X_ fifo data length in byte
 *
 * @return  error code
 * @retval  #GH3X_RET_OK                      return read fifo successful
 * @retval  #GH3X_RET_PARAMETER_ERROR         return param error
 * @retval  #GH3X_RET_READ_FIFO_CONTINUE      return fifo is not empty
 */
GS16 GH3X_ReadFifodata(GU8 *puchReadFifoData, GU16* pusReadFifoDataLen)
{
    //GU16 usFifoLength = 0;
    GS16 sRet = GH3X_RET_OK;
    GU16 usFifoReadByteNum = 5 * (GH3X_ReadReg(GH3X_REG_FIFO_USE_ADDR) & 0x3FFF);
    GH3X_ClearSoftEvent(GH3X_SOFT_EVENT_NEED_TRY_READ_FIFO);
    * pusReadFifoDataLen = 0;
    if (usFifoReadByteNum == 0)
    {
        return sRet;
    }
    GH3X_SAMPLE_LOG_PARAM("%s usFifoReadByteNum = %d\r\n", __FUNCTION__,usFifoReadByteNum);
    if ((puchReadFifoData != GH3X_PTR_NULL) && (pusReadFifoDataLen != GH3X_PTR_NULL))
    {
        /*
        usFifoLength = GH3X_FIFO_CNT_CALC(GH3X_ReadReg(GH3X_INT_FIFO_UR_REG_ADDR));
        */
        if (usFifoReadByteNum > gusMaxNumReadFromFifo)
        {
            usFifoReadByteNum = gusMaxNumReadFromFifo;
            sRet = GH3X_RET_READ_FIFO_CONTINUE;
            GH3X_SetSoftEvent(GH3X_SOFT_EVENT_NEED_TRY_READ_FIFO);
        }

        if ((usFifoReadByteNum > 0) && (usFifoReadByteNum <= GH3X_FIFO_DATA_BYTES_MAX_LEN))
        {
            GH3X_ReadFifo(puchReadFifoData, usFifoReadByteNum);
#if (__GH3X_INTERFACE__ != __GH3X_INTERFACE_SPI__)
            if(GH3X_CheckRawdataBuf(puchReadFifoData, &usFifoReadByteNum))
            {
                *pusReadFifoDataLen = 0;
                return GH3X_RET_PARAMETER_ERROR;
            }
#endif
            GH3X_CalRawdataBuf(puchReadFifoData, usFifoReadByteNum);
        }
        else
        {
            if (usFifoReadByteNum > GH3X_FIFO_DATA_BYTES_MAX_LEN)
            {
                usFifoReadByteNum = 0;
                GH3X_DEBUG_LOG_PARAM("get rawdata fifo len greater than max, pelease check i2c/spi!\r\n");
            }
            else
            {
                GH3X_DEBUG_LOG_PARAM("get rawdata fifo equl 0!\r\n");
            }
        }
        /* call hook */
        HOOK_FUNC_CALL(gpGetRawdataHookFunc, (puchReadFifoData, usFifoReadByteNum));
        (*pusReadFifoDataLen) = usFifoReadByteNum;
    }// if (pstRawdata->puchReadBuffer != GH3X_PTR_NULL)
    else
    {
        GH3X_DEBUG_LOG_PARAM("get rawdata error that pointer is null!\r\n");
        sRet = GH3X_RET_PARAMETER_ERROR;
    }

    return sRet;
}


void GH3x_SetIaEn(GU8 uchEcgEn,GU8 uchBiaEn,GU8 uchGsrEn,GU8 uchStart)
{
    GU8 uchIaAcEn =0;
    GU8 uchIaDcEn =0;
    GU16 uchRegMask = (uchStart == 1)? 0xFFFF : 0x0000;

    if(GH3X_CHECK_BIT_SET(uchEcgEn, (1<<GH3X_REG_ECG1_EN_BIT)) || 
        GH3X_CHECK_BIT_SET(uchEcgEn, (1<<GH3X_REG_ECG0_EN_BIT)) ||
        uchBiaEn)
    {
        uchIaAcEn = 1;
    }
    if(GH3X_CHECK_BIT_SET(uchEcgEn, (1<<GH3X_REG_ECGDC_EN_BIT)))
    {
        uchIaDcEn = 1;
    }
    if(uchGsrEn)
    {
        GU16 usGsr1AcModeSel =  GH3X_ReadRegBitField(GH3X_REG_GSR_CTRL1_ADDR,GH3X_REG_GSR_MODE_BIT,GH3X_REG_GSR_MODE_BIT);
        if(usGsr1AcModeSel)
            uchIaAcEn = 1;
        else
            uchIaDcEn = 1;
    }
    GU16 uchIaEn = uchIaDcEn + (uchIaAcEn << 1);
    GU8 trx_manual_mode = 0;
    if(uchIaEn && uchStart)
    {
        trx_manual_mode = GH3X_ReadRegBitField(0x200, 0, 0);
        GH3X_WriteRegBitField(0x200, 0, 0, 0);        
        if((uchIaEn& 0x03) != GH3X_ReadRegBitField(GH3X_REG_PMU_REG0_ADDR,GH3X_REG_IA_DC_EN_BIT,GH3X_REG_IA_AC_EN_BIT))
        {
            GH3X_WriteRegBitField(GH3X_REG_PMU_REG0_ADDR, GH3X_REG_IA_DC_EN_BIT, GH3X_REG_IA_AC_EN_BIT,0);
            GH3X_WriteRegBitField(GH3X_REG_PMU_REG0_ADDR, GH3X_REG_IA_DC_EN_BIT, GH3X_REG_IA_AC_EN_BIT,(uchIaEn& 0x03));
            GH3x_WaitPllReady(5);
        }
    }
    if((uchEcgEn != 0 ))
    {
        GU16 usTempEcgEn = GH3X_ReadRegBitField(GH3X_REG_ECG_CTRL0_ADDR,GH3X_REG_ECG0_EN_BIT,GH3X_REG_ECGDC_EN_BIT);
        usTempEcgEn = (uchStart == 1) ? ((uchEcgEn & 0x07)| usTempEcgEn) : ((~(uchEcgEn & 0x07)) & usTempEcgEn);
        GH3X_WriteRegBitField(GH3X_REG_ECG_CTRL0_ADDR, GH3X_REG_ECG0_EN_BIT, GH3X_REG_ECGDC_EN_BIT,usTempEcgEn);
    }
    if(uchBiaEn)
    {
        GH3X_WriteRegBitField(GH3X_REG_BIA_CTRL0_ADDR, GH3X_REG_BIA_EN_BIT, GH3X_REG_BIA_EN_BIT, uchBiaEn & uchRegMask);
    }
    if(uchGsrEn)
    {
        GH3X_WriteRegBitField(GH3X_REG_GSR_CTRL1_ADDR, GH3X_REG_GSR_EN_BIT, GH3X_REG_GSR_EN_BIT, uchGsrEn & uchRegMask);
    }
    if(uchStart && trx_manual_mode){
        GH3X_WriteRegBitField(0x200, 0, 0, 1);
    }
}

GU32 GH3x_SetSlotEn(GU64 unSlotCfg, GU8 uchStart)
{
    //stop slot
    GU16 usSlotIdxArr[GH3X_SLOT_NUM_MAX/2] = {0};
    GH3X_WriteRegBitField(GH3X_REG_SLOT_SAMPLE_START_ADDR, GH3X_REG_SLOT_SAMPLE_START_LSB, GH3X_REG_SLOT_SAMPLE_START_MSB, 0);
    GH3X_WriteRegs(GH3X_REG_SLOT_EN0_ADDR,usSlotIdxArr, 2);
    //re range slot index
    GH3X_Memset(usSlotIdxArr, 0x3F, sizeof(usSlotIdxArr));
    GU8* puchSlotIdx = (GU8*)usSlotIdxArr;
    GU8* puchConfigSlotIdx = (GU8*)gstPPGSlotInfo.usSlotIndex;

//    GU16 usCnt = 0;
    GU32 unSlotEnBit = 0;
    for(int i = 0 ; i < GH3X_SLOT_NUM_MAX; i++)
    {
        if((puchConfigSlotIdx[i] != 0x3F) && (unSlotCfg &((GU64)(1) << puchConfigSlotIdx[i])) != 0)
        {
            *puchSlotIdx = puchConfigSlotIdx[i] ;
            puchSlotIdx++;
            unSlotEnBit = (unSlotEnBit << 1) + 1;
            
#if __SUPPORT_SOFT_AGC_CONFIG__
            GU8 uchSlotIdx = puchConfigSlotIdx[i];
            GH3xNewAgcMeanInfoReset(uchSlotIdx);            
#endif             
        }
    }
    GH3X_Memcpy(gstPPGSlotInfo.usRunningSlotIndex, usSlotIdxArr, sizeof(gstPPGSlotInfo.usRunningSlotIndex));

    GH3X_WriteRegs(GH3X_REG_SLOT_INDEX_CTRL0_ADDR,usSlotIdxArr, GH3X_SLOT_NUM_MAX/2);
    usSlotIdxArr[0] = (unSlotEnBit & 0xFFFF);
    usSlotIdxArr[1] = ((unSlotEnBit >> 16) & 0xFFFF);
    GH3X_WriteRegs(GH3X_REG_SLOT_EN0_ADDR,usSlotIdxArr, 2);
    if(unSlotEnBit != 0)
    {      
        GH3X_WriteRegBitField(GH3X_REG_SLOT_SAMPLE_START_ADDR, GH3X_REG_SLOT_SAMPLE_START_LSB, GH3X_REG_SLOT_SAMPLE_START_MSB, 1);
    }
    return 0;
}

void GH3x_CalFunctionEnBit(const STFrameInfo * const pstFrameInfo,GU64* punSlotBit,GU8* pEcgEnBit,GU8* pBiaEnBit,GU8* pGsrEnBit)
{
    if(pstFrameInfo == 0)
    {
        return;
    }
    GU16 usChnlTag;
    for(GU8 uchChnlCnt = 0; uchChnlCnt < pstFrameInfo->pstGh3xData->uchChnlNum; uchChnlCnt ++)
    {
        usChnlTag = pstFrameInfo->pstGh3xData->pusChnlMap[uchChnlCnt] ; //pstFrameInfo->pchChnlMap[uchChnlCnt];
        if(CHNLMAP_GET_DATATYPE(usChnlTag) == DATATYPE_PPG)
        {
            if (punSlotBit != 0)
                *punSlotBit |= ((GU64)1 << (GH3X_WORD_TAG_GET_SLOT_NUM(usChnlTag)));
        }
        else if((usChnlTag >> 11) == 0x19) //bp
        {
            if (punSlotBit != 0)
                *punSlotBit |= ((GU64)1 << (((usChnlTag >> 9) & 0x03) + 33));
        }
        else if((usChnlTag >> 11) == 0x1A) //temp
        {
            if (punSlotBit != 0)
                *punSlotBit |= ((GU64)1 << (32));
        }
        else if(CHNLMAP_GET_DATATYPE(usChnlTag) == DATATYPE_BIA) //bia
        {
            if (pBiaEnBit != 0)
                *pBiaEnBit |= 1;
        }
        else if((usChnlTag >> 11) == 0x18) //gsr
        {
            if (pGsrEnBit != 0)
                *pGsrEnBit |= 1;
        }
        else if(pEcgEnBit != 0 && CHNLMAP_GET_DATATYPE(usChnlTag) == DATATYPE_ECG && CHNLMAP_GET_ECGTYPE(usChnlTag) != 2) //ecg
        {
            if(((usChnlTag >> 9) & 0x03) == 0x2)
            {
                *pEcgEnBit |= (1 << GH3X_REG_ECG0_EN_BIT);
            }
            else if(((usChnlTag >> 9) & 0x03) == 0x3)
            {
                *pEcgEnBit |= (1 << GH3X_REG_ECG1_EN_BIT);
            }
            else if(((usChnlTag >> 9) & 0x02) == 0x0)
            {
                *pEcgEnBit |= (1 << GH3X_REG_ECGDC_EN_BIT);
            }
        }
    }
}
void GH3X_SetWearStatusReg(GU8 uchWearStatus)
{
    GH3X_ExitLowPowerMode();
    uchWearStatus &= 0x01;
    GH3X_WriteRegBitField(GH3X_REG_WEARON_CR_ADDR, GH3X_REG_WEAR_STATUS_BIT, GH3X_REG_WEAR_STATUS_BIT, uchWearStatus);
    GH3X_EnterLowPowerMode();
}

void GH3X_InitFunctionFrameInfo(const STFrameInfo * const pstFrameInfo)
{
    if (pstFrameInfo)
    {
        if(pstFrameInfo->punFrameCnt)
            (*(pstFrameInfo->punFrameCnt)) = 0;
        if(pstFrameInfo->pstGh3xData){
            pstFrameInfo->pstGh3xData->uchChnlIndex = 0;
            pstFrameInfo->pstGh3xData->unChnlBits = 0;
        }
        if(pstFrameInfo->pstFlagInfo)
            pstFrameInfo->pstFlagInfo->uchFlagBits = 0;
        if(pstFrameInfo->pstAlgoData)
            pstFrameInfo->pstAlgoData->unResultBits = 0;
    }
}

/**
 * @fn     GS8 GH3X_FunctionStart(const STFrameInfo * const pstFrameInfo)
 *
 * @brief  Start functions sample
 *
 * @attention   None
 *
 * @param[in]   pstFrameInfo    frame struct
 *
 * @return  error code
 */
GS8 GH3X_FunctionStart(const STFrameInfo * const pstFrameInfo)
{
    if(GH3X_PTR_NULL == pstFrameInfo)
    {
        return GH3X_RET_OK;
    }
    GS8 chRet = GH3X_RET_OK;
    GU32 unFunctionID;
    unFunctionID = 1 << pstFrameInfo->uchFuncID;
    pstFrameInfo->pstGh3xData->unChnlBits = 0;
    /* if all func off, start sampling */
    if (GH3X_NO_FUNCTION == gunFuncStartedBitmap)
    {
        EXAMPLE_LOG("GH3X_NO_FUNCTION----------------------------------------------------");
        GH3X_WriteRegBitField(0x780, 0, 1,0);
        chRet = GH3X_StartSampling();
        GH3X_DEBUG_LOG_PARAM("[FunctionStart] function id: 0x%X, start result: %d\r\n",(int)(1 << pstFrameInfo->uchFuncID), chRet);
        GH3X_RET_ERROR_CHECK_E(chRet, GH3X_RET_NO_INITED_ERROR);
    }
    GU8 uchPpgEn = GH3X_ReadRegBitField(0x0026, 0, 0);
    if(unFunctionID == GH3X_FUNCTION_ADT)
    {
        GU8 uchLeadEn = GH3X_ReadRegBitField(GH3X_REG_ADT_LEADON_CR_ADDR, GH3X_REG_LD_GSR_DET_EN_BIT, GH3X_REG_LD_ECG1_DET_EN_BIT);
        if(uchLeadEn != 0 && uchPpgEn == 0)
        {
            GH3X_WriteRegBitField(GH3X_REG_ADT_LEADON_CR_ADDR, GH3X_REG_LD_GSR_DET_EN_BIT, GH3X_REG_LD_ECG1_DET_EN_BIT, 0);
            GH3X_WriteRegBitField(GH3X_REG_PMU_REG0_ADDR, GH3X_REG_IA_DC_EN_BIT, GH3X_REG_IA_AC_EN_BIT,0x01);
            GH3x_WaitPllReady(3);
            GH3X_WriteRegBitField(0x200, 0, 0,0);
            GH3X_WriteReg(0x788, 0xC01);
            GH3X_WriteReg(0x100, 0x503C);
            GH3X_WriteReg(0x106, 0x1001);
            GH3X_WriteRegBitField(GH3X_REG_ADT_LEADON_CR_ADDR, GH3X_REG_LD_GSR_DET_EN_BIT, GH3X_REG_LD_ECG1_DET_EN_BIT, uchLeadEn);
        }
#if 0 == __HARD_ADT_ALGO_CHECK_EN__
        GU16 usWearOnModuleEnBak;
        usWearOnModuleEnBak = GH3X_ReadRegBitField(GH3X_REG_WEARON_CR_ADDR, 0, 0);
        GH3X_WriteRegBitField(GH3X_REG_WEARON_CR_ADDR, 0, 0, 0);  //disable wear module
        GH3X_WriteReg(GH3X_REG_INT_EVENT1_ADDR, GH3X_IRQ_MSK_WEAR_ON_BIT | GH3X_IRQ_MSK_WEAR_OFF_BIT);  //clean wear on/off event
        GH3X_WriteRegBitField(GH3X_REG_WEARON_CR_ADDR, GH3X_REG_WEAR_STATUS_BIT, GH3X_REG_WEAR_STATUS_BIT, 0);
        GH3X_WriteRegBitField(GH3X_REG_WEARON_CR_ADDR, 0, 0, usWearOnModuleEnBak);  //recover wear module
#endif
    }
    else if(unFunctionID == GH3X_FUNCTION_ECG && uchPpgEn == 0)
    {
        GH3X_WriteRegBitField(GH3X_REG_PMU_REG0_ADDR, GH3X_REG_IA_DC_EN_BIT, GH3X_REG_IA_AC_EN_BIT,0);
        GU8 uchLeadEn = GH3X_ReadRegBitField(GH3X_REG_ADT_LEADON_CR_ADDR, GH3X_REG_LD_GSR_DET_EN_BIT, GH3X_REG_LD_ECG1_DET_EN_BIT);
        if(uchLeadEn != 0)
        {
            GH3X_WriteReg(0x100, 0x50FA);
            GH3X_WriteReg(0x106, 0x1000);
            GH3X_WriteReg(0x788, 0x0300);
            GH3X_WriteRegBitField(GH3X_REG_ADT_LEADON_CR_ADDR, GH3X_REG_LD_GSR_DET_EN_BIT, GH3X_REG_LD_ECG1_DET_EN_BIT, 0);
            GH3X_WriteRegBitField(GH3X_REG_ADT_LEADON_CR_ADDR, GH3X_REG_LD_GSR_DET_EN_BIT, GH3X_REG_LD_ECG1_DET_EN_BIT, uchLeadEn);
        }
        GH3X_WriteRegBitField(0x200, 0, 0,1);
    }
    /* set started bit */
    gunFuncStartedBitmap |= unFunctionID;

    /* clear timestamp */
    GH3X_InitFunctionFrameInfo(pstFrameInfo);

    return chRet;
}

/**
 * @fn     GS8 GH3X_FunctionStop(const STFrameInfo * const pstFrameInfo)
 *
 * @brief  Stop functions sample
 *
 * @attention   None
 *
 * @param[in]   pstFrameInfo    frame struct
 *
 * @return  error code
 */
GS8 GH3X_FunctionStop(const STFrameInfo * const pstFrameInfo)
{
    GS8 chRet = GH3X_RET_OK;
    GU32 unFunctionID;
    if(0 == pstFrameInfo)
    {
        return chRet;
    }
    
    unFunctionID = 1 << pstFrameInfo->uchFuncID;
    
    /* clear started bit */
    gunFuncStartedBitmap &= (~unFunctionID);

    GU8 uchPpgEn = 0;
    GU8 uchLeadEn = 0;
    if(unFunctionID == GH3X_FUNCTION_ECG || unFunctionID == GH3X_FUNCTION_ADT)
    {
        uchPpgEn = GH3X_ReadRegBitField(0x0026, 0, 0);
        uchLeadEn = GH3X_ReadRegBitField(GH3X_REG_ADT_LEADON_CR_ADDR, GH3X_REG_LD_GSR_DET_EN_BIT, GH3X_REG_LD_ECG1_DET_EN_BIT);
    }
    if(unFunctionID == GH3X_FUNCTION_ECG  && uchPpgEn == 0 && uchLeadEn != 0 && GH3X_NO_FUNCTION != gunFuncStartedBitmap)
    {
        GH3X_WriteRegBitField(0x200, 0, 0,0);
        GH3X_WriteReg(0x788, 0xC01);
        GH3X_WriteReg(0x100, 0x503C);
        GH3X_WriteReg(0x106, 0x1001);
    }
    /* if all func off, stop sampling */
    if (GH3X_NO_FUNCTION == gunFuncStartedBitmap)
    {
        chRet = GH3X_StopSampling();
        //GH3X_WriteRegBitField(GH3X_REG_PMU_REG0_ADDR, GH3X_REG_IA_DC_EN_BIT, GH3X_REG_IA_AC_EN_BIT,0);
        GH3X_DEBUG_LOG_PARAM("[FunctionStop] function id: 0x%X, start result: %d\r\n",(int)(1 << pstFrameInfo->uchFuncID), chRet);
        //GH3X_RET_ERROR_CHECK(chRet);
        GH3X_WriteReg(0x100, 0x50FA);
        GH3X_WriteReg(0x106, 0x1000);
        GH3X_WriteReg(0x788, 0x0300);
        GH3X_WriteRegBitField(0x780, 0, 1,3);
        GH3X_WriteRegBitField(0x780, 8, 10,0);
        /* fixed fifo watermark thr */
        //GH3X_FIXED_FIFO_WATERMARK_THR();
    }
    if(unFunctionID == GH3X_FUNCTION_ADT)
    {
        if(uchLeadEn != 0 && uchPpgEn == 0)
        {
            GH3X_WriteReg(0x788, 0x0300);
            GH3X_WriteReg(0x100, 0x50FA);
            GH3X_WriteReg(0x106, 0x1000);
        }
        GH3X_WriteReg(GH3X_REG_INT_EVENT1_ADDR, GH3X_IRQ_MSK_WEAR_ON_BIT | GH3X_IRQ_MSK_WEAR_OFF_BIT);  //clean wear on/off event
    }
    return chRet;
}



/**
 * @fn     void GH3xSetFunctionChnlNum(const STFrameInfo * const pstFrameInfo, GU8 uchChnlNum)
 *
 * @brief  config Function ChnlNum
 *
 * @attention   None
 *
 * @param[in]   STFrameInfo    frame struct, uchChnlNum  channel in function
 *
 * @return  None
 */
void GH3xSetFunctionChnlNum(const STFrameInfo * const pstFrameInfo, GU8 uchChnlNum)
{
    GU8 uchChnlLimit;
    if(pstFrameInfo)
    {
        uchChnlLimit = pstFrameInfo->pstGh3xData->uchMaxChnlNum;
        if(uchChnlNum > uchChnlLimit)
        {
            uchChnlNum = uchChnlLimit;
        }
        pstFrameInfo ->pstGh3xData->uchChnlNum = uchChnlNum;
        GH3X_Memset(pstFrameInfo ->pstGh3xData->pusChnlMap,0,uchChnlLimit*sizeof(pstFrameInfo ->pstGh3xData->pusChnlMap[0]));
        GH3X_DEBUG_LOG_PARAM("[SetFunctionChnlNum] function id: 0x%X, ChnlNum: %d\r\n",(int)pstFrameInfo->uchFuncID, uchChnlNum);
    }
}


/**
 * @fn     void GH3xSetFunctionChnlMap(const STFrameInfo * const pstFrameInfo, GU8 uchChnlNum)
 *
 * @brief  config Function ChnlMap
 *
 * @attention   None
 *
 * @param[in]   STFrameInfo    frame struct, uchChnlId channel Id in function, uchChnlTag  tag in channel
 *
 * @return  None
 */
void GH3xSetFunctionChnlMap(const STFrameInfo * const pstFrameInfo, GU8 uchChnlId, GU16 uchChnlTag)
{
    if(pstFrameInfo)
    {
        if(uchChnlId < (pstFrameInfo->pstGh3xData->uchMaxChnlNum ))
        {
            pstFrameInfo ->pstGh3xData->pusChnlMap[uchChnlId] = uchChnlTag;
            
            GH3X_DEBUG_LOG_PARAM("[SetFunctionChnlMap] function id: 0x%X, uchChnlTag: %d\r\n",(int)pstFrameInfo->uchFuncID, uchChnlTag);
        }
        
    }
}




/**
 * @fn     GU32 GH3X_GetSoftEvent(void)
 * @brief  Get soft event
 * @param[in]   None
 * @param[out]  Soft event
 *
 * @return  soft event
 */
GU32 GH3X_GetSoftEvent(void)
{
    return gunSoftEvent;
}

/**
 * @fn     void GH3X_SetSoftEvent(GU32 unEvent)
 * @brief  set soft event
 * @param[in]   uchEvent
 * @param[out]  None
 * @return  None
 */
void GH3X_SetSoftEvent(GU32 unEvent)
{
    gunSoftEvent |= unEvent;
}
/**
 * @fn     void GH3X_ClearSoftEvent(GU32 unEvent)
 * @brief  clear soft event
 * @param[in]   uchEvent
 * @param[out]  None
 * @return  None
 */

void GH3X_ClearSoftEvent(GU32 unEvent)
{
    gunSoftEvent &= (~unEvent);
}


GS8 gh_read_efuse(GU8 efuse_start_byte_addr, GU8 efuse_data[], GU16 efuse_data_byte_len)
{
    GU16 read_data[4];
    efuse_data_byte_len = efuse_data_byte_len > 8 ? 8 : efuse_data_byte_len;
    GH3X_WriteRegBitField(0x802, 0, 6, efuse_start_byte_addr);
    GH3X_WriteRegBitField(0x802, 8, 11, (efuse_data_byte_len  - 1) & 0x07);
    GH3X_WriteRegBitField(0x816, 0, 0, 1);
    while(GH3X_ReadReg(0x816));
    GH3X_ReadRegs(0x818,read_data,4);
    GH3X_Memcpy(efuse_data, read_data, efuse_data_byte_len);
    return GH3X_RET_OK;
}

GS8 gh_read_tia_gain_error(void)
{
    GS8 efuse_data[5];
    gh_read_efuse(0x19, (GU8*)efuse_data, 5);
    GH3X_SAMPLE_LOG_PARAM("PPG.RX.Gain.Error : %d\r\n",efuse_data[0]);
    GH3X_SAMPLE_LOG_PARAM("PPG.RX0.Gain.Ratio.Error : %d\r\n",efuse_data[1]);
    GH3X_SAMPLE_LOG_PARAM("PPG.RX1.Gain.Ratio.Error : %d\r\n",efuse_data[2]);
    GH3X_SAMPLE_LOG_PARAM("PPG.RX2.Gain.Ratio.Error : %d\r\n",efuse_data[3]);
    GH3X_SAMPLE_LOG_PARAM("PPG.RX3.Gain.Ratio.Error : %d\r\n",efuse_data[4]);
    return GH3X_RET_OK;
}
GS8 gh_read_led_drv_gain_error(void)
{
    GS8 efuse_data[3];
    gh_read_efuse(0x18, (GU8*)efuse_data, 1);
    GH3X_SAMPLE_LOG_PARAM("LED.Driver0.Gain.Error: %d\r\n",efuse_data[0]);
    gh_read_efuse(0x29, (GU8*)efuse_data, 3);
    GH3X_SAMPLE_LOG_PARAM("LED.Driver1.Gain.Error: %d\r\n",efuse_data[0]);
    GH3X_SAMPLE_LOG_PARAM("LED.Driver2.Gain.Error: %d\r\n",efuse_data[1]);
    GH3X_SAMPLE_LOG_PARAM("LED.Driver3.Gain.Error: %d\r\n",efuse_data[2]);
    return GH3X_RET_OK;
}
GS8 gh_read_dc_cancel_error(void)
{
    GS8 efuse_data[2];
    gh_read_efuse(0x22, (GU8*)efuse_data, 2);
    GH3X_SAMPLE_LOG_PARAM("PPG.DC.Cancel.Error1 : %d\r\n",efuse_data[0]);
    GH3X_SAMPLE_LOG_PARAM("PPG.DC.Cancel.Error2 : %d\r\n",efuse_data[1]);
    return GH3X_RET_OK;
}
GS8 gh_read_bg_cancel_error(void)
{
    GS8 efuse_data[1];
    gh_read_efuse(0x28, (GU8*)efuse_data, 1);
    GH3X_SAMPLE_LOG_PARAM("PPG.BG.Cancel.Error: %d\r\n",efuse_data[0]);
    return GH3X_RET_OK;
}

GS8 gh_read_cp_uid(void)
{
    GU8 efuse_data[8];
    gh_read_efuse(0x0, (GU8*)efuse_data, 8);
    for(GU8 i = 0; i < 8; i++)
    {
        GH3X_SAMPLE_LOG_PARAM("uid %d,0x%02x\r\n",i,efuse_data[i]);
        GH3X_DelayMs(1);
    }
    return GH3X_RET_OK;
}
/**
 * @fn    float get_real_tia_gain(GU8 gain_code, GS8 rx_gain_error, GS8 rx_gain_ratio_error)
 * @brief  
 * @param[in]   gain_code : gain_code 0~8
 * @param[in]   rx_gain_error :get from efuse
 * @param[in]   rx_gain_ratio_error:get from efuse
 * @param[out]  None
 * @return  gain(KOhm)
 */
float get_real_tia_gain(GU8 gain_code, GS8 rx_gain_error, GS8 rx_gain_ratio_error)
{
    float ret = 7.8125f * (1 + (float)rx_gain_error / 512.0f);
    for(GU8 i = 0 ; i < gain_code; i++)
    {
        ret = ret * ((float)rx_gain_ratio_error/256.0f + 2);
    }
    return ret;
}
/**
 * @fn     float get_real_led_current(GS8 led_drv_gain_error, GU8 led_full_scale, GU8 led_drv_num)
 * @brief  
 * @param[in]   led_drv_gain_error :get from efuse
 * @param[in]   led_full_scale : full scale : 0 ~ 3
 * @param[in]   led_drv_code : led drv code :0 ~ 255
 * @param[in]   led_drv_num : 0 ~ 3
 * @param[out]  None
 * @return  current(mA)
 */
float get_real_led_current(GS8 led_drv_gain_error, GU8 led_full_scale, GU8 led_drv_code ,GU8 led_drv_num)
{
    float y[] = {0.06, 0.015, 0.015, 0.06};
    float Er = (float)led_drv_gain_error / 256.0f + 1;
    GU32 N = 1 << (3 - led_full_scale);
    float ret_mA = 225.0f / (Er * (255.0f * (float)N) / (float)led_drv_code + y[led_drv_num]);
    return ret_mA;
}
/**
 * @fn     float get_dc_cancel_current(GS8 DC_Cancel_Error1, GS8 DC_Cancel_Error2, GU8 dc_cancel_code)

 * @brief  
 * @param[in]   DC_Cancel_Error1 :get from efuse
 * @param[in]   DC_Cancel_Error2 :get from efuse
 * @param[in]   dc_cancel_code : 0 ~ 255
 * @param[in]   led_drv_code : led drv code :0 ~ 255
 * @param[out]  None
 * @return  current(uA)
 */
float get_dc_cancel_current(GS8 DC_Cancel_Error1, GS8 DC_Cancel_Error2, GU8 dc_cancel_code)
{
    float a = (float)DC_Cancel_Error1/256.0f + 1;
    float b = (float)DC_Cancel_Error2/64.0f;
    float ideal_val = dc_cancel_code / 255.0 * 36;// if VRF = 1.8V, FS = 36uA
    float ret_uA = a * ideal_val + b;
    return ret_uA;
}

GS8 gh_ia_filter(void)
{
    GS8 ret = GH3X_RET_LOW_TEMP_INVALID;
    //1. reset
#if __SUPPORT_HARD_RESET_CONFIG__
    GH3X_HardReset();
    GH3X_DelayMs(7);
    GU16 usEvent[7] = {0xFFFF,0xFFFF};
    GH3X_WriteRegs(0x060A, usEvent,2); //clear INT
    //check GID
    GU8 efuse_data[1];
    gh_read_efuse(0x0F, (GU8*)efuse_data, 1);
    GH3X_SAMPLE_LOG_PARAM("GID: %d\r\n",efuse_data[0]);
    if(0x3033 == GH3X_ReadReg(0x04) || 0x07 == efuse_data[0])
    {
        return GH3X_RET_OK;
    }

    //2. Lift PLL freequence
    GH3X_WriteRegBitField(0x0100, 0, 9,0x129);// pll = 297 * 32000 = 9,504,000
    GH3X_WriteRegBitField(0x780, 8, 10,7);//start up PLL
    GH3X_DelayMs(5);
    if (GH3X_RET_OK == GH3X_CommunicateConfirm())
    {
        ret = GH3X_RET_OK;
    }

    //3. reset  
    GH3X_HardReset();
    GH3X_DelayMs(7);
    // usEvent[0] = 0xFFFF;
    // usEvent[1] = 0xFFFF;
    // GH3X_WriteRegs(0x060A, usEvent,2); //clear INT
#endif
    return ret;
}

// GS8 gh_ia_filter(void)
// {
//     GS8 ret = GH3X_RET_LOW_TEMP_INVALID;
//     //1. reset
// #if __SUPPORT_HARD_RESET_CONFIG__
//     GH3X_HardReset();
//     GH3X_DelayMs(7);
//     GU16 usEvent[7] = {0xFFFF,0xFFFF};
//     GH3X_WriteRegs(0x060A, usEvent,2); //clear INT

//     //2. Lift PLL freequence
//     GH3X_WriteRegBitField(0x0100, 0, 9,0x129);// pll = 297 * 32000 = 9,504,000
//     GH3X_WriteRegBitField(0x780, 8, 10,7);//start up PLL
//     GH3X_DelayMs(5);
//     if (GH3X_RET_OK == GH3X_CommunicateConfirm())
//     {
//         ret = GH3X_RET_OK;
//     }

//     //3. reset  
//     GH3X_HardReset();
//     GH3X_DelayMs(7);
//     // usEvent[0] = 0xFFFF;
//     // usEvent[1] = 0xFFFF;
//     // GH3X_WriteRegs(0x060A, usEvent,2); //clear INT
// #endif
//     return ret;
// }



GS8* GhGetChipId(void)
{
    static GS8 chChipId[5];
    GH3X_Memset(chChipId,0,sizeof(chChipId));
    GU16 usChipIdWord[2];
    GH3X_ReadRegs(0x0004,usChipIdWord,2);
    chChipId[0] = GH3X_GET_LOW_BYTE_FROM_WORD(usChipIdWord[1]);
    chChipId[1] = GH3X_GET_HIGH_BYTE_FROM_WORD(usChipIdWord[0]);
    chChipId[2] = GH3X_GET_LOW_BYTE_FROM_WORD(usChipIdWord[0]);
    return chChipId;
}

//gunSlotCfgMask
void SetPPGSlotInfo(void)
{
    GU16* usSlotIdxArr = gstPPGSlotInfo.usSlotIndex;
    GH3X_Memset(usSlotIdxArr, 0x3F, GH3X_SLOT_NUM_MAX);
    GH3X_ReadRegs(GH3X_REG_SLOT_INDEX_CTRL0_ADDR, usSlotIdxArr, GH3X_SLOT_NUM_MAX/2);
    GU8* puchSlotIdx = (GU8*)usSlotIdxArr;
    gstPPGSlotInfo.unSlotCfgMask = 0;
    for(GU8 i = 0; i < GH3X_SLOT_NUM_MAX; i++)
    {
        if(puchSlotIdx[i] <32)
        {
            gstPPGSlotInfo.unSlotCfgMask |= (1 << puchSlotIdx[i]);
        }
    }
    gstPPGSlotInfo.uchFatestRate = GH3X_ReadRegBitField(0x000C,0, 7);
    gstPPGSlotInfo.puchDriverFs[0] = GH3X_ReadRegBitField(0x340,4, 5);
    gstPPGSlotInfo.puchDriverFs[1] = GH3X_ReadRegBitField(0x340,0, 1);
    gstPPGSlotInfo.puchDriverFs[2] = GH3X_ReadRegBitField(0x344,4, 5);
    gstPPGSlotInfo.puchDriverFs[3] = GH3X_ReadRegBitField(0x344,0, 1);

}
/**
 * @fn     void GH3X_SetLedCurrent(GU32 unslotCfgNum, double currentmA)
 * @brief  
 * @param[in]   unslotCfgNum : slot cfg num
 * @param[in]   currentmA : led current, unit : mA
 * @param[out]  None
 * @return  None
 */
void GH3X_SetLedCurrent(GU32 unslotCfgNum, double currentmA)
{
    GU16 slotLedMap = GH3X_ReadRegBitField(0x0c3e + 0x1c*unslotCfgNum,0, 7);
    GU16 oprLedDrv = 0;
    GU16 puchCurrentRegVals[2] = {0};
    GU8* puchCurrentRegVal =  (GU8*)puchCurrentRegVals;
    while(slotLedMap != 0 && oprLedDrv < 4)
    {
        if(slotLedMap & 0x01)
        {
            GU32 ledFs = 25;
            for(GU8 i = 0; i < gstPPGSlotInfo.puchDriverFs[oprLedDrv]; i++)
            {
                ledFs *= 2;
            }
            puchCurrentRegVal[oprLedDrv] = ((double)currentmA/(double)ledFs*255.0) + 0.5;
        }
        slotLedMap = slotLedMap >> 2;
        oprLedDrv++;
    }
    GH3X_WriteRegs(0xc3a + 0x1c*unslotCfgNum,puchCurrentRegVals, 2);
}
/**
 * @fn     void GH3X_SetSlotSampleRate(GU32 unslotCfgNum, GU32 unSampleRate)
 * @brief  
 * @param[in]   unslotCfgNum : slot cfg num
 * @param[in]   unSampleRate : sample rate, unit : Hz
 * @param[out]  None
 * @return  None
 */
void GH3X_SetSlotSampleRate(GU32 unslotCfgNum, GU32 unSampleRate)
{
    double fatestSampleRate = (double)GH3X_PPG_CLK/((double)gstPPGSlotInfo.uchFatestRate + 1);
    double slotDivider = (fatestSampleRate/(double)unSampleRate) - 1;
    GU32 unRegVal = (GU32)((double)slotDivider + 0.5);
    GH3X_WriteRegBitField(0x0C28 + 0x1C*unslotCfgNum,8, 15,unRegVal);
}
/**
 * @fn     void GH3X_SetSlotIntTime(GU32 unslotCfgNum, GU8 uchIntTime)
 * @brief  
 * @param[in]   unslotCfgNum : slot cfg num
 * @param[in]   uchIntTime : Int Time code,  write to register directly
 * @param[out]  None
 * @return  None
 */
void GH3X_SetSlotIntTime(GU32 unslotCfgNum, GU8 uchIntTime)
{
    uchIntTime &= 0xF;
    GH3X_WriteRegBitField(0x0C2E + 0x1C*unslotCfgNum,0, 3, uchIntTime);
}
/**
 * @fn     void GH3X_SetSlotGain(GU32 unslotCfgNum, GU8 uchAdcNo,GU8 uchGain)
 * @brief  
 * @param[in]   unslotCfgNum : slot cfg num
 * @param[in]   uchAdcNo : adc no, 0 ~ 3
 * @param[in]   uchGain : tia gain code, write to register directly
 * @param[out]  None
 * @return  None
 */
void GH3X_SetSlotGain(GU32 unslotCfgNum, GU8 uchAdcNo,GU8 uchGain)
{
    uchAdcNo &= 0x3;
    uchGain &= 0xF;
    GH3X_WriteRegBitField(0x0C30 + 0x1C*unslotCfgNum,0 << (4*uchAdcNo), 3 << (4*uchAdcNo), uchGain);
}

GU32 GH3X_CalSlotControl(GU16* usSlotIdxArr,GU8 uchSlotCfgNum, GU8 uchStart)
{
    GU8* puchSlotIdx = (GU8*)usSlotIdxArr;//slot cfgs which already opened
    GU8* puchConfigSlotIdx = (GU8*)gstPPGSlotInfo.usSlotIndex;//slot cfgs in config
    GU32 unSlotEnBit = 0;

    if(uchStart)
    {
        GU16 usNewSlotIndex[GH3X_SLOT_NUM_MAX/2];
        GU8* puchNewSlotIndex = (GU8*)usNewSlotIndex;
        GH3X_Memset(usNewSlotIndex,0x3F,sizeof(usNewSlotIndex));
        GU8 uchCurrentSlotIndex = 0;
        GU8 uchNewSlotIndex = 0;
        for(int i = 0; i < GH3X_SLOT_NUM_MAX;i++)
        {
            if(puchConfigSlotIdx[i] > 31)
            {
                continue;
            }
            if (puchSlotIdx[uchCurrentSlotIndex] == puchConfigSlotIdx[i] && uchSlotCfgNum != puchSlotIdx[uchCurrentSlotIndex])
            {
                puchNewSlotIndex[uchNewSlotIndex] = puchSlotIdx[uchCurrentSlotIndex];
                uchNewSlotIndex++;
                uchCurrentSlotIndex++;
                unSlotEnBit = (unSlotEnBit << 1) + 1;
            }
            else if(puchSlotIdx[uchCurrentSlotIndex] != puchConfigSlotIdx[i] && puchConfigSlotIdx[i] == uchSlotCfgNum)
            {
                puchNewSlotIndex[uchNewSlotIndex] = uchSlotCfgNum;
                uchNewSlotIndex++;
                unSlotEnBit = (unSlotEnBit << 1) + 1;
            }
            
        }
        GH3X_Memcpy(puchSlotIdx,puchNewSlotIndex,sizeof(usNewSlotIndex));
    }
    else
    {
        int i;
        int end = 0;
        while (end < GH3X_SLOT_NUM_MAX && puchSlotIdx[end] != 0x3F) {
            ++end;
            unSlotEnBit = (unSlotEnBit << 1) + 1;
        }
        for (i = 0; i < end; ++i) {
            if (puchSlotIdx[i] == uchSlotCfgNum) {
                break;
            }
        }
        if (i < end) {
            for (; i < end - 1; ++i) {
                puchSlotIdx[i] = puchSlotIdx[i + 1];
            }
            puchSlotIdx[i] = 0x3F;
            unSlotEnBit = (unSlotEnBit >> 1);
        }
    }
    return unSlotEnBit;
}


void GH3X_SlotControl(GU32 unSlotCfgNumBits, GU8 uchStart)
{
    GU16 usSlotIdxArr[GH3X_SLOT_NUM_MAX/2] = {0};
    GH3X_Memset(usSlotIdxArr, 0x3F, sizeof(usSlotIdxArr));
    GH3X_ReadRegs(GH3X_REG_SLOT_INDEX_CTRL0_ADDR, usSlotIdxArr, GH3X_SLOT_NUM_MAX/2);
    unSlotCfgNumBits &= gstPPGSlotInfo.unSlotCfgMask;
    GU32 unSlotEnBit = 0;
    GU8 uchSlotCfgNum = 0;
    while (unSlotCfgNumBits != 0)
    {
        if(unSlotCfgNumBits & 1)
        {
            unSlotEnBit = GH3X_CalSlotControl(usSlotIdxArr, uchSlotCfgNum, uchStart);
        }
        unSlotCfgNumBits = (unSlotCfgNumBits >> 1);
        uchSlotCfgNum++;
    }
    //GU8 uchSlotStart = GH3X_ReadRegBitField(8,0,0);
    GH3X_WriteRegBitField(0x0008,0, 0,0);
    GH3X_WriteReg(GH3X_REG_SLOT_EN0_ADDR,0);
    GH3X_WriteReg(GH3X_REG_SLOT_EN0_ADDR + 2,0);
    GH3X_WriteRegs(GH3X_REG_SLOT_INDEX_CTRL0_ADDR,usSlotIdxArr, GH3X_SLOT_NUM_MAX/2);
    usSlotIdxArr[0] = (unSlotEnBit & 0xFFFF);
    usSlotIdxArr[1] = ((unSlotEnBit >> 16) & 0xFFFF);
    GH3X_WriteRegs(GH3X_REG_SLOT_EN0_ADDR,usSlotIdxArr, 2);
    if (unSlotEnBit != 0)
    {
        GH3X_WriteRegBitField(0x0008,0, 0,1);
    }
}

void GhHardAgcDisable(void)
{
    for(int unslotCfgNum = 0; unslotCfgNum < GH3X_SLOT_NUM_MAX; unslotCfgNum++)
    {
        GH3X_WriteRegBitField(0x0C2E + 0x1C*unslotCfgNum,8, 11, 0);
    }
}

GU8 GH3X_ExistPace(const STFrameInfo * const pstFrameInfo)
{
    GU8 ret = 0;
    for(GU8 i = 0; i < pstFrameInfo->pstGh3xData->uchChnlNum; i++)
    {
        if(CHNLMAP_GET_DATATYPE(pstFrameInfo->pstGh3xData->pusChnlMap[i]) == DATATYPE_ECG &&
        CHNLMAP_GET_ECGTYPE(pstFrameInfo->pstGh3xData->pusChnlMap[i]) == ECGTYPE_PACE)
        {
            ret = 1;
            break;
        }
    }
    return ret;
}

GU8 GhBitsCount(GU16 x) {
    GU8 count;
    for (count = 0; x != 0; x &= x - 1) {
        count++;
    }
    return count;
}
#if __FIFO_MONITOR_EN__
void Gh3xFifoMonitorInfoDeInit(void)
{
    gstFifoMonitorInfo.uchInitFlag = 0;
}
void Gh3xFifoMonitorInfoInit(void)
{
    //return;
    //static STGh3xFifoMonitorInfo gstFifoMonitorInfo;
    GU8* puchConfigSlotIdx = (GU8*)gstPPGSlotInfo.usRunningSlotIndex;
    GU8 usEstiAllChnlSampleCntlimit = 0;
    GU8 lowest_fs = 0;
    GU8 lowest_slot_cfg = 0;
    GU8 lowest_adc = 0;
    GU8 lowest_dwsp = 0;
     
    for(int i = 0 ; i < GH3X_SLOT_NUM_MAX; i++)
    {
        if(puchConfigSlotIdx[i] < 32 && GH3X_ReadRegBitField(0xC28+0x1C*puchConfigSlotIdx[i], 4, 4))
        {
            //1.read sample rate
            GU16 sample_rate = GH3X_ReadRegBitField(0xC28+0x1C*puchConfigSlotIdx[i], 8, 15);
            //2. dwsp en
            GS16 dwsp_en = GH3X_ReadRegBitField(0xC28+0x1C*puchConfigSlotIdx[i], 6, 6);
            if(sample_rate > lowest_fs || (sample_rate == lowest_fs && dwsp_en))
            {
                lowest_dwsp = dwsp_en;
                lowest_fs = sample_rate;
                lowest_slot_cfg = puchConfigSlotIdx[i];
                GU16 rx_en = GH3X_ReadRegBitField(0xC28+0x1C*puchConfigSlotIdx[i], 0, 3);
                lowest_adc = 0;
                while(rx_en)
                {
                    if(rx_en & 0x01)
                    {
                        break;
                    }
                    rx_en = rx_en >> 1;
                    lowest_adc++;
                }
            }
        }
    }

    for(int i = 0 ; i < GH3X_SLOT_NUM_MAX; i++)
    {
        if(puchConfigSlotIdx[i] < 32 && GH3X_ReadRegBitField(0xC28+0x1C*puchConfigSlotIdx[i], 4, 4) && puchConfigSlotIdx[i] != lowest_slot_cfg)
        {
            //1.read sample rate
            GU16 sample_rate = GH3X_ReadRegBitField(0xC28+0x1C*puchConfigSlotIdx[i], 8, 15);
            //2. rx en
            GU16 rx_en = GH3X_ReadRegBitField(0xC28+0x1C*puchConfigSlotIdx[i], 0, 3);
            //3. dwsp en
            GS16 dwsp_en = GH3X_ReadRegBitField(0xC28+0x1C*puchConfigSlotIdx[i], 6, 6);
            
            GU8 this_slot_rxs =  GhBitsCount(rx_en);
            GS16 muiti = (lowest_fs + 1)/(sample_rate + 1) ;
            if(lowest_dwsp)
            {
                muiti *= 2;
            }
            if(dwsp_en)
            {
                muiti /= 2;
            }
            usEstiAllChnlSampleCntlimit += (muiti * GhBitsCount(rx_en));
        }
    }
    gstFifoMonitorInfo.uchLowestChnlSlotcfg = lowest_slot_cfg;
    gstFifoMonitorInfo.uchLowestChnlAdcNum = lowest_adc;
    gstFifoMonitorInfo.uchLowestChnlAppearedFlag = 0;
    gstFifoMonitorInfo.usNeedSkipFifoNum = 1 + GH3X_ReadRegBitField(0x60E, 0, 14);;
    gstFifoMonitorInfo.usAllChnlRealTimeSampleCnt = 0;
    gstFifoMonitorInfo.usEstiAllChnlSampleCntlimit = usEstiAllChnlSampleCntlimit;
    gstFifoMonitorInfo.uchLowestChnlAdcNum = lowest_adc;
    gstFifoMonitorInfo.uchInitFlag = 1;
  
}
#endif
/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
