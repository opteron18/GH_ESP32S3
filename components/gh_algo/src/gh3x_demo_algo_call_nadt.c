/**
  ****************************************************************************************
  * @file    gh3x_demo_algo_call_nadt.c
  * @author  GHealth Driver Team
  * @brief   
  ****************************************************************************************
  * @attention  
  #####Copyright (c) 2022 GOODIX
   All rights reserved.
  
  ****************************************************************************************
  */

#include <stdio.h>
#include "gh3x_demo_algo_call.h"
#include "gh3x_demo_algo_call_inner.h"
#include "goodix_mem.h"
#include "goodix_nadt.h"

#if (__USE_GOODIX_SOFT_ADT_ALGORITHM__)

#define NADT_CONFIG_SAMPLE_RATE_TYPE               (4)
#define NADT_CONFIG_MULTI_SAMPLE_RATE_TYPE         (4)

#define SOFT_WEAR_INPUT_DATA_CHNL0_INDEX   (0)
#define SOFT_WEAR_INPUT_DATA_ACCX_INDEX    (1)
#define SOFT_WEAR_INPUT_DATA_ACCY_INDEX    (2)
#define SOFT_WEAR_INPUT_DATA_ACCZ_INDEX    (3)
#define SOFT_WEAR_INPUT_COLOR_INDEX        (4)
#define SOFT_WEAR_INPUT_DATA_ADT_INDEX     (5) //ir  adt data
#define SOFT_WEAR_INPUT_DATA_BG_INDEX      (6)
#define SOFT_WEAR_INPUT_DATA_CAP0_INDEX    (7)
#define SOFT_WEAR_INPUT_DATA_CAP1_INDEX    (8)
#define SOFT_WEAR_INPUT_DATA_CAP2_INDEX    (9)
#define SOFT_WEAR_INPUT_DATA_CAP3_INDEX    (10)


#define SOFT_WEAR_INPUT_DATA_INDEX_TOTAL (11)
#define SOFT_WEAR_OUTPUT_DATA_INDEX_TOTAL (8)

#define WATCH_NUM     (3)

#if (WATCH_NUM == 1)
static GS32 lWearCap1 = 16400;
static GS32 lUnwearCap1 = 12400;
static GS32 lWearCap2 = 0;
static GS32 lUnwearCap2 = 0;
#elif (WATCH_NUM == 2)
static GS32 lWearCap1 = 20000;
static GS32 lUnwearCap1 = 16000;
static GS32 lWearCap2 = 3700;
static GS32 lUnwearCap2 = 1200;
#elif (WATCH_NUM == 3)
static GS32 lWearCap1 = 42500;
static GS32 lUnwearCap1 = 39500;
static GS32 lWearCap2 = 42500;
static GS32 lUnwearCap2 = 39500;
#endif

static GU8  guchSoftAdtChannl0Color;   // 0: green   1: ir
static GU32 gunNadtIrDefaultTimeOutRecord = 0;
static GU32 gunNadtIrDefaultTimeOutSecondsThrd = __SOFT_ADT_IR_DETECT_TIMEOUT__;
static GU8 g_SoftWearQuality = 0;
NADT_ST_MID_INFO stMidInfo;
extern GU8 g_SoftAdtWearState;

void GH3X_SetIRDetectTimeThreshold(GU32 unIRDetectTimeThreshold)
{
    gunNadtIrDefaultTimeOutSecondsThrd = unIRDetectTimeThreshold;
}

void SoftWearControlInit(const STFrameInfo * const pstFrameInfo)
{
	g_SoftAdtWearState = GH3X_SOFT_ALGO_ADT_DEFAULT;
	GCHAR uchNadtVersion[100] = {0};

    GetNadtVersion((uint8_t *)uchNadtVersion);
    GH3X_ALGO_LOG_PARAM("[%s]nadt algorithm version : %s\r\n", __FUNCTION__, uchNadtVersion);
	
	
	goodix_nadt_config stNadtCfg ;
	GH3X_Memcpy(&stNadtCfg,goodix_nadt_config_get_arr(),goodix_nadt_config_get_size());
	
    stNadtCfg.sampleRateNADT = pstFrameInfo ->pstGh3xData->usSampleRate;

	GH3X_ALGO_LOG_PARAM("[%s]nadt param0 : %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,\r\n", __FUNCTION__, stNadtCfg.sampleRateNADT, \
        stNadtCfg.unwearDetectLevel, stNadtCfg.sleepStatus, stNadtCfg.checkTimeoutThr, stNadtCfg.unwearCheckCntThr, stNadtCfg.wearCheckCntThr, \
        stNadtCfg.adtWearOnThr, stNadtCfg.adtWearOffThr, stNadtCfg.accStdThr, stNadtCfg.liveDetectEnable, stNadtCfg.rawPVLenThr, \
        stNadtCfg.heartBeatThrLow, stNadtCfg.heartBeatThrHigh, stNadtCfg.difRrInterThr, stNadtCfg.wearAcfScoreThr);
    GH3X_ALGO_LOG_PARAM("[%s]nadt param1 : %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,\r\n", __FUNCTION__, stNadtCfg.baseLinePosRatioThr, \
        stNadtCfg.baseLineDiffThr, stNadtCfg.sigScaleThr, stNadtCfg.bgJumpDetectEnable, stNadtCfg.jumpBgDiffThr, stNadtCfg.bgPeakValleyNumThr, \
        stNadtCfg.bgPeriodDetectEnable, stNadtCfg.periodBgDiffThr, stNadtCfg.bgPeriodThr, stNadtCfg.bgPeriodDiffThr);
	
    const void *p_CfgInstance = (void *)&stNadtCfg;
    char chVer[NADT_INTERFACE_VERSION_LEN_MAX] = {0};

    goodix_nadt_config_get_version(chVer, NADT_INTERFACE_VERSION_LEN_MAX);
    const char *p_InterfaceVer = chVer;

    NADTProcInit(p_CfgInstance, sizeof(stNadtCfg), p_InterfaceVer);
    memset(&stMidInfo, 0, sizeof(NADT_ST_MID_INFO));
}


void GH3XCapNadtResultPro(const STFrameInfo * const pstFrameInfo,GS32  pnSoftWearOffDetResult[])
{
    if (pnSoftWearOffDetResult[3])            // 电容更新标记,更新佩戴检测阈值
    {
        pstFrameInfo->pstAlgoData->uchFlag = 1;
        lUnwearCap1 = pnSoftWearOffDetResult[4];
        lWearCap1 = pnSoftWearOffDetResult[5];
        lUnwearCap2 = pnSoftWearOffDetResult[6];
        lWearCap2 = pnSoftWearOffDetResult[7];
    }
    if(g_SoftWearQuality != pnSoftWearOffDetResult[2])
    {
        pstFrameInfo->pstAlgoData->uchFlag = 1;
        g_SoftWearQuality = pnSoftWearOffDetResult[2];
    }
    if(pstFrameInfo->punFrameCnt == 0)
    {
        pstFrameInfo->pstAlgoData->uchFlag = 1;
    }
    if(pstFrameInfo->pstAlgoData->uchFlag == 1)
    {
        pstFrameInfo->pstAlgoData->unResultBits = 0x7;
        pstFrameInfo->pstAlgoData->uchResultNum = GH3X_BitCount(pstFrameInfo->pstAlgoData->unResultBits);      
        pstFrameInfo->pstAlgoData->pnResults[0] = pnSoftWearOffDetResult[0]; // (bit0-bit1)：佩戴状态（0默认，1佩戴，2脱落，3非活体）； (bit2)：疑似脱落标记（0正常，1疑似脱落）；
        pstFrameInfo->pstAlgoData->pnResults[1] = pnSoftWearOffDetResult[1]; //活体检测置信度
        pstFrameInfo->pstAlgoData->pnResults[2] = pnSoftWearOffDetResult[2]; //佩戴质量（1紧佩戴，2松佩戴，3极度松佩戴，4脱落）；
        if(pstFrameInfo->punFrameCnt == 0)
        {
            pstFrameInfo->pstAlgoData->unResultBits = 0x7F;
            pstFrameInfo->pstAlgoData->pnResults[3] = lUnwearCap1; //脱落电容值1；
            pstFrameInfo->pstAlgoData->pnResults[4] = lWearCap1; //佩戴电容值1；
            pstFrameInfo->pstAlgoData->pnResults[5] = lUnwearCap2; //脱落电容值2；
            pstFrameInfo->pstAlgoData->pnResults[6] = lWearCap2; //佩戴电容值2；
        }
        else
        {
            pstFrameInfo->pstAlgoData->unResultBits = 0x7F;
            pstFrameInfo->pstAlgoData->pnResults[3] = pnSoftWearOffDetResult[4]; //脱落电容值1；
            pstFrameInfo->pstAlgoData->pnResults[4] = pnSoftWearOffDetResult[5]; //佩戴电容值1；
            pstFrameInfo->pstAlgoData->pnResults[5] = pnSoftWearOffDetResult[6]; //脱落电容值2；
            pstFrameInfo->pstAlgoData->pnResults[6] = pnSoftWearOffDetResult[7]; //佩戴电容值2；
        }
    }
}

GS8 GH3XSoftAdtAlgoExe(const STFrameInfo * const pstFrameInfo)
{
    GS32 nSoftWearOffInputBuf[SOFT_WEAR_INPUT_DATA_INDEX_TOTAL] = {0};
    GS32 pnSoftWearOffDetResult[SOFT_WEAR_OUTPUT_DATA_INDEX_TOTAL] = {0};

    if(pstFrameInfo)
    {
        pstFrameInfo->pstAlgoData->uchFlag = 0;
        GU32 unLedAdjFlag = 0;

        if(pstFrameInfo->pstFlagInfo->punFlag[0]&0x00000001)  //led current change
        {
            unLedAdjFlag = 1;
        }
        //if((0xFF != pstFrameInfo->puchFrameLastGain[0])&&((pstFrameInfo->pstGh3xData->punAgcInfo[0]&0x0000000F) != pstFrameInfo->puchFrameLastGain[0]))  //tia gain change
        //{
        //    unLedAdjFlag = 1;
        //}

        nSoftWearOffInputBuf[SOFT_WEAR_INPUT_DATA_ACCX_INDEX] = pstFrameInfo->pstGsData->pusData[0];
        nSoftWearOffInputBuf[SOFT_WEAR_INPUT_DATA_ACCY_INDEX] = pstFrameInfo->pstGsData->pusData[1];
        nSoftWearOffInputBuf[SOFT_WEAR_INPUT_DATA_ACCZ_INDEX] = pstFrameInfo->pstGsData->pusData[2];
        nSoftWearOffInputBuf[SOFT_WEAR_INPUT_DATA_CHNL0_INDEX] = pstFrameInfo->pstGh3xData->punRawdata[0];
        nSoftWearOffInputBuf[SOFT_WEAR_INPUT_DATA_CHNL0_INDEX] |= (unLedAdjFlag << 26);  //led adj flag
        nSoftWearOffInputBuf[SOFT_WEAR_INPUT_COLOR_INDEX]      =  guchSoftAdtChannl0Color;      //0:GREEN  1:IR
        nSoftWearOffInputBuf[SOFT_WEAR_INPUT_DATA_ADT_INDEX]   = pstFrameInfo->pstGh3xData->punRawdata[1];
        nSoftWearOffInputBuf[SOFT_WEAR_INPUT_DATA_BG_INDEX]    = pstFrameInfo->pstGh3xData->punRawdata[2];

        if (guchAlgoCapEnable == 1)
        {
            for (int k = 0; k < 4; k++)
            {
                //nSoftWearOffInputBuf[SOFT_WEAR_INPUT_DATA_CAP0_INDEX + k] = pstFrameInfo->pstFrameCapdata->unCapData[k];//  [PPGACCLEN + ALGOLEN + guchCompareNum + k]; //电容
            }
        }

        goodix_nadt_ret uchRet = NADTProc(nSoftWearOffInputBuf, pnSoftWearOffDetResult, &stMidInfo);
        GU32 unNadtIrDefaultTimeOutThrd = gunNadtIrDefaultTimeOutSecondsThrd * pstFrameInfo->pstGh3xData->usSampleRate;
        if (gunNadtIrDefaultTimeOutRecord >= unNadtIrDefaultTimeOutThrd && ((pnSoftWearOffDetResult[0] & 0x3) == 0))
        {
            pnSoftWearOffDetResult[0] = pnSoftWearOffDetResult[0] | 0x1;
            gunNadtIrDefaultTimeOutRecord = 0;
        }

        if ((pnSoftWearOffDetResult[0] & 0x2) == 0)
        {
            if (GH3X_FUNCTION_SOFT_ADT_IR == (1 << pstFrameInfo->uchFuncID))
            {
                gunNadtIrDefaultTimeOutRecord++;
            }
        }

        if (uchRet != GX_ALGO_NADT_SUCCESS)
        {
            GH3X_ALGO_LOG_PARAM("[GH3xSoftAdtAlgoExe]NADTProc: error! error code = 0x%x\r\n", uchRet);
        }

        if (guchAlgoCapEnable == 1)
        {
            GH3XCapNadtResultPro(pstFrameInfo, pnSoftWearOffDetResult);
        }
        else
        {
            pstFrameInfo->pstAlgoData->uchFlag = 1;
            pstFrameInfo->pstAlgoData->unResultBits = 0x3;
            pstFrameInfo->pstAlgoData->uchResultNum = GH3X_BitCount(pstFrameInfo->pstAlgoData->unResultBits);
            pstFrameInfo->pstAlgoData->pnResults[0] = pnSoftWearOffDetResult[0]; // (bit0-bit1)：佩戴状态（0默认，1佩戴，2脱落，3非活体）； (bit2)：疑似脱落标记（0正常，1疑似脱落）；
            pstFrameInfo->pstAlgoData->pnResults[1] = pnSoftWearOffDetResult[1]; //活体检测置信度
        }

        //report
        if (guchSoftAdtChannl0Color == 0)
        {
            GH3X_SoftAdtGreenAlgorithmResultReport(pstFrameInfo->pstAlgoData, GH3X_GET_BYTE0_FROM_DWORD(*(pstFrameInfo->punFrameCnt)));
        }
        else if(guchSoftAdtChannl0Color == 1)
        {
            GH3X_SoftAdtIrAlgorithmResultReport(pstFrameInfo->pstAlgoData, GH3X_GET_BYTE0_FROM_DWORD(*(pstFrameInfo->punFrameCnt)));
        }
    }
    return GH3X_RET_OK;
}


GS8 GH3XSoftAdtAlgoInit(const STFrameInfo * const pstFrameInfo)
{
    memset(&pstFrameInfo->pstAlgoData->uchFlag, 0, sizeof(GU8) + sizeof(GU8) + sizeof(GU32));

    if(pstFrameInfo)
    {
        g_SoftWearQuality  = 0;
        guchSoftAdtChannl0Color = 1 * (GH3X_FUNCTION_SOFT_ADT_IR == (1 << pstFrameInfo->uchFuncID));

        //GH3X_SAMPLE_ALGO_LOG_PARAM("[SoftAdt]CH0 slot = %d, rx = %d\r\n", GH3X_BYTE_RAWDATA_GET_SLOT_NUM(pstFrameInfo ->pchChnlMap[0]),GH3X_BYTE_RAWDATA_GET_ADC_NUM(pstFrameInfo ->pchChnlMap[0]));
        //GH3X_ALGO_LOG_PARAM("[SoftAdt]CH1 slot = %d, rx = %d\r\n", GH3X_BYTE_RAWDATA_GET_SLOT_NUM(pstFrameInfo ->pchChnlMap[1]),GH3X_BYTE_RAWDATA_GET_ADC_NUM(pstFrameInfo ->pchChnlMap[1]));
        //GH3X_ALGO_LOG_PARAM("[SoftAdt]CH2 slot = %d, rx = %d\r\n", GH3X_BYTE_RAWDATA_GET_SLOT_NUM(pstFrameInfo ->pchChnlMap[2]),GH3X_BYTE_RAWDATA_GET_ADC_NUM(pstFrameInfo ->pchChnlMap[2]));
        //GH3X_ALGO_LOG_PARAM("[SoftAdt]guchSoftAdtChannl0Color = %d\r\n", guchSoftAdtChannl0Color);

        SoftWearControlInit(pstFrameInfo);
    }
    return GH3X_RET_OK;
}


GS8 GH3XSoftAdtAlgoDeinit(const STFrameInfo * const pstFrameInfo)
{
    GH3X_ALGO_LOG_PARAM("[SoftAdt]%s\r\n", __FUNCTION__);
    goodix_nadt_ret ret = NADTProcDeinit();
    gunNadtIrDefaultTimeOutRecord = 0;
	if (GX_ALGO_NADT_SUCCESS != ret)
    {
        GH3X_ALGO_LOG_PARAM("[%s] fail! error code = 0x%x\r\n", __FUNCTION__, ret);
        return GH3X_RET_GENERIC_ERROR;
    }
    return GH3X_RET_OK;
}

#endif
