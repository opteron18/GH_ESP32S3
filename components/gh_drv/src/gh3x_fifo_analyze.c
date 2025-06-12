/**
  ****************************************************************************************
  * @file    gh3x_fifo_analyze.c
  * @author  GHealth Driver Team
  * @brief   
  ****************************************************************************************
  * @attention  
  #####Copyright (c) 2022 GOODIX
   All rights reserved.
  
  ****************************************************************************************
  */

#include "gh3x_drv.h"
#include "string.h"
#include "gh3x_fifo_analyze.h"
#include "gh3x_demo_protocol.h" 
#include "gh3x_demo_config.h" 
#include "gh3x_soft_led_agc.h"
#include "gh_gsr.h"

extern void gh3x_algorithm_get_io_data_hook_func(const STFrameInfo * const pstFrameInfo);
extern GU16 g_usBiaMeasureMode;
static void GhGotOneFrameData(const STFrameInfo * const pstFrameInfo,GU32* punFrameCnt,
                              GU16* pusFrameNum,GS16 *psGsFifo, float fGsStep, float* pfGsFifoCnt);
static void GhFillGhRawdata(const STFrameInfo * const pstFrameInfo,GU8 uchChnlIndex,char *puchRawdataFifo,int nSearchStartPosi);

GU32 gunFifoFullCnt = 0;
extern const STFrameInfo * const gpstFrameInfo[GH3X_FUNC_OFFSET_MAX];
static GU8 guchFifoFullFlag = 0;
GS32 GH3X_u24toint24(GU32 unU24Val);

void GH3X_SetFifoFullFlag(GU8 uchFlag)
{
    if (uchFlag)
    {
        gunFifoFullCnt++;
        GH3X_SAMPLE_LOG_PARAM("[%s]:Fifo full!!!\n",__FUNCTION__);
    }
    guchFifoFullFlag = uchFlag;
}

float GH3X_CalSensorStep(GU16 usDataNum, GU16 usFrameNum)
{
    float fStep = 0;
    if ((usFrameNum > 1) && (usDataNum > 0)) // calc gs index
    {
        fStep = ((GF32)(usDataNum)) / (GF32)(usFrameNum);
    }
    return fStep;
}

GU8 GhPopCount(GU64 x) {
    GU8 count;
    for (count = 0; x != 0; x &= x - 1) {
        count++;
    }
    return count;
}

void GH3X_SetFlag(GU8 index, GU16 tag, GU32 *pflag, GU8 *puchBits)
{
    if(pflag == 0 || puchBits == 0)
    {
        return;
    }
    GU32 pflagtemp[GH3X_FRAME_FLAG_NUM] = {0};
    pflagtemp[7] |= guchFifoFullFlag & 0x1;
    switch (CHNLMAP_GET_DATATYPE(tag))
    {
    case DATATYPE_PPG:
        pflagtemp[0] |= (TAG_GET_PPGADJFLAG(tag) << index);
        pflagtemp[1] |= (TAG_GET_PPGSAFLAG(tag) << index);
        break;
    case DATATYPE_PPG_BG:
        pflagtemp[2] |= TAG_GET_BGECGWEARON(tag);
        break;
    case DATATYPE_BG_CANCEL:
        break;
    case DATATYPE_LED_DRV:
        break;
    case DATATYPE_ECG:
        pflagtemp[2] |= TAG_GET_ECGLEAD(tag) << index;
        pflagtemp[2] |= TAG_GET_ECGFASTFLAG(tag) << (index + 4);
        //pflagtemp[2] |= TAG_GET_ECGMODE(tag) << (CHNLMAP_GET_ECGADCNO(tag) + 6);
        switch(CHNLMAP_GET_ECGTYPE(tag))
        {
        case 1:
            break;
        case 2://pace
            pflagtemp[3] = 1;
            break;
        default:
            break;
        }
        break;
    case DATATYPE_BIA:
        pflagtemp[2] |= TAG_GET_BIALEAD(tag);
        pflagtemp[3] = CHNLMAP_GET_BIASLOTNUM(tag);
        pflagtemp[4] = TAG_GET_BIANUM(tag);
        break;
    case DATATYPE_GSR:
        switch (CHNLMAP_GET_GSRTYPE(tag))
        {
        case GSR_DATATYPE_GSR:
#if __FUNC_TYPE_GSR_ENABLE__
            pflagtemp[1] |= get_gsr_current();
#endif
            break;
        case GSR_DATATYPE_PRESSURE:
        case GSR_DATATYPE_TEMPERATURE:
            pflagtemp[1] |= TAG_GET_GSRSAFLAG(tag) << (index);
            break;
        }
        break;
    default:
        break;
    }
    //compare
    for(int i = 0; i < GH3X_FRAME_FLAG_NUM; i++)
    {
        if (pflagtemp[i] != pflag[i])
        {
            pflag[i] = pflagtemp[i];
            *puchBits |= (1 << i);
        }
    }
}

void GH3X_SetGain(GU8 index, GU16 tag, GU32* pgain)
{
    if(pgain == 0)
    {
        return;
    }
    switch (CHNLMAP_GET_DATATYPE(tag))
    {
        case DATATYPE_PPG:
        case DATATYPE_PPG_BG:
            pgain[index] = TAG_GET_PPGGAIN(tag);
            #if __SUPPORT_SOFT_AGC_CONFIG__
            pgain[index] |= (((g_puchDrvCurrentPreSoftAgc[CHNLMAP_GET_CFGMUX(tag)])<<8)&0xFF00);
            #endif
            break;
        default:
            break;
    }
}

GU8 GH3X_TagCheck(GU16 usReadTag, GU16 usChnlMap)
{
    if (CHNLMAP_GET_DATATYPE(usReadTag) == CHNLMAP_GET_DATATYPE(usChnlMap))
    {
        switch(CHNLMAP_GET_DATATYPE(usReadTag))
        {
        case DATATYPE_PPG:
        case DATATYPE_PPG_BG:
            return (CHNLMAP_GET_CFGMUX(usReadTag) == CHNLMAP_GET_CFGMUX(usChnlMap) &&
                    CHNLMAP_GET_ADCNO(usReadTag) == CHNLMAP_GET_ADCNO(usChnlMap));
        case DATATYPE_BG_CANCEL:
        case DATATYPE_LED_DRV:
            return (CHNLMAP_GET_CFGMUX(usReadTag) == CHNLMAP_GET_CFGMUX(usChnlMap));
        case DATATYPE_ECG:
            return (CHNLMAP_GET_ECGADCNO(usReadTag) == CHNLMAP_GET_ECGADCNO(usChnlMap) &&
                    CHNLMAP_GET_ECGTYPE(usReadTag) == CHNLMAP_GET_ECGTYPE(usChnlMap));
        case DATATYPE_BIA:
            return ( CHNLMAP_GET_BIAIQFLAG(usReadTag) == CHNLMAP_GET_BIAIQFLAG(usChnlMap) );
        case DATATYPE_GSR:
            return (CHNLMAP_GET_GSRTYPE(usReadTag) == CHNLMAP_GET_GSRTYPE(usChnlMap) &&
                    CHNLMAP_GET_GSRFLAG(usReadTag) == CHNLMAP_GET_GSRFLAG(usChnlMap));
        default:
            break;
        }
    }
    else
    {
        return 0;
    }
    return 0;
}

GU8 GH3X_TagCompare(GU16 usReadTag, GU16 *pusChnlMap, GU8 uchChnlNum, GU8 lastIdx)
{
    GU8 cnt = 0;
    lastIdx = (lastIdx + 1) % uchChnlNum;
    while (cnt < uchChnlNum)
    {
        if (GH3X_TagCheck(usReadTag, pusChnlMap[lastIdx]))
        {
            return lastIdx;
        }
        lastIdx = (lastIdx + 1) % uchChnlNum;
        cnt++;
    }
    return 0xFF;
}

void GH3X_FillGsensorData(GS16 *psGsFifo, float *pfGsFifoCnt, float fGsStep, 
                          STGsensorData *pstGsData)
{
    if(psGsFifo == 0 || pstGsData->uchEnableFlag == 0)
    {
        return;
    }
    GU8 uchStep = 3 + 3 * __GS_GYRO_ENABLE__;
    for(int i = 0 ; i < uchStep; i++)
    {
        if((*pfGsFifoCnt) != (int)(*pfGsFifoCnt))
        {
            pstGsData->pusData[i] = psGsFifo[((int)*pfGsFifoCnt)*uchStep + i] * (1.0f-*pfGsFifoCnt + (int)(*pfGsFifoCnt))
                                    + psGsFifo[((int)(*pfGsFifoCnt) + 1)*uchStep + i] * (*pfGsFifoCnt - (int)(*pfGsFifoCnt));
        }
        else
        {
            pstGsData->pusData[i] = psGsFifo[((int)*pfGsFifoCnt)*uchStep + i];
        }
    }
    //memcpy(pstGsData->pusData, &psGsFifo[((int)*pfGsFifoCnt)*uchStep], uchStep * sizeof(GS16));
    *pfGsFifoCnt += (float) fGsStep;
}

void GH3X_FrameDataHandle(const STFrameInfo * const pstFrameInfo) //data struct
{
    //algothrim
    gh3x_algorithm_get_io_data_hook_func(pstFrameInfo);
    //send data
#if __SUPPORT_GHEALTH_PROTOCOL__
    GH3X_UprotocolSendFrameDataBytes(pstFrameInfo);
#endif
}

GS16 GH3X_CalResampleCnt(const STFrameInfo * const pstFrameInfo)
{
    if(pstFrameInfo -> pstReSampleInfo == 0 ||
        pstFrameInfo -> pstReSampleInfo ->sRawdataFrameNum == pstFrameInfo -> pstReSampleInfo ->sResampleFrameNum ||
        pstFrameInfo -> pstReSampleInfo ->fRawdataFs == 0)
    {
        return 1;
    }
    GS16 sResampleCnt = 0;
    GS16 sResampleFrameNum = pstFrameInfo -> pstReSampleInfo ->sResampleFrameNum;
    GS16 sRawdataFrameNum = pstFrameInfo -> pstReSampleInfo ->sRawdataFrameNum;
    
    //up sample
    if(sResampleFrameNum > sRawdataFrameNum && sRawdataFrameNum > 0)
    {
        GS16 sNormalCnt = (GS16)((GF32)sResampleFrameNum / (GF32)sRawdataFrameNum + 0.5f);
        sResampleCnt = (sResampleFrameNum) > sNormalCnt ? sNormalCnt : (sResampleFrameNum);
        pstFrameInfo -> pstReSampleInfo ->sRawdataFrameNum--;
        pstFrameInfo -> pstReSampleInfo ->sResampleFrameNum -= sResampleCnt;	
    }
    else if(sResampleFrameNum < sRawdataFrameNum && sResampleFrameNum > 0)//down sample
    {
        GF32 fRawdataStep = (GF32)sRawdataFrameNum / (GF32)sResampleFrameNum;
        GS16 sRawdataFrameCnt = pstFrameInfo -> pstReSampleInfo ->sRawdataFrameCnt;
        GS16 sResampleFrameCnt = pstFrameInfo -> pstReSampleInfo ->sResampleFrameCnt;
        if(sRawdataFrameCnt == (GS16)(0.5f + fRawdataStep * (sResampleFrameCnt + 1)))
        {
            sResampleCnt = 1;
            pstFrameInfo -> pstReSampleInfo ->sResampleFrameCnt += sResampleCnt;
        }
        pstFrameInfo -> pstReSampleInfo ->sRawdataFrameCnt += 1;
    }
    return sResampleCnt;
}

void GhBiaFifoFrameHandle(GU32 *punFlag,GU8 *puchFlagBits, GU16* pusFrameNum,GU32* punFrameCnt)
{
    if (punFlag != GH3X_PTR_NULL)
    {
        punFlag[2] &= (~0x2);
        if(0<g_usBiaMeasureMode)  //// not demo mode
        {
            *puchFlagBits = 0x1A;   ////upload flag1,3,4   																
        }
        else
        {
            *puchFlagBits |= 0x3f;   ////upload flag0-5
        }
        if(*punFrameCnt >= *pusFrameNum)
        {
            *punFrameCnt = 0;
            punFlag[0] = 1;//last frame bia data flag
        }
        else
        {
            punFlag[0] = 0;
        }
    }

}
void GhClearRawdataBuf(GU32 *punFlag,GU8 *puchFlagBits, GU32 *punRawdata, GS16 sResampleCnt,GU8 uchChnlNum)
{
    if (punFlag != GH3X_PTR_NULL && sResampleCnt > 0)
    {
        punFlag[6] &= (~0x1);
        punFlag[2] &= (~0x2);
    }
    if(sResampleCnt == 0)
    {
        if (punFlag != GH3X_PTR_NULL)
        {
            *puchFlagBits = 0;
        }
        memset(punRawdata, 0, uchChnlNum * sizeof(GU32));
    }
}

void GH3X_FifoFrameHandle(char *puchRawdataFifo, int nRawdataFifoLen, //gh3x rawdata
                          GS16 *psGsFifo, float fGsStep, //gsensor
                          GU16 usFrameNum, const STFrameInfo * const pstFrameInfo) //data struct
{
    float fGsFifoCnt = (float)0;
    GU8 uchChnlIndex = pstFrameInfo->pstGh3xData->uchChnlIndex;
    GU32 unFrameCnt = 0;
    if (pstFrameInfo->pstGh3xData->uchChnlNum == 0)
    {
        return;
    }
    GU8 uchExistPace = GH3X_ExistPace(pstFrameInfo);
    for (int nSearchStartPosi = 0; nSearchStartPosi < nRawdataFifoLen; nSearchStartPosi += GH3X_FIFO_RAWDATA_SIZE)
    {
        GU16 usTagTemp = GH3X_MAKEUP_WORD(puchRawdataFifo[nSearchStartPosi], puchRawdataFifo[nSearchStartPosi + 1]);
        uchChnlIndex = GH3X_TagCompare(usTagTemp, pstFrameInfo->pstGh3xData->pusChnlMap, pstFrameInfo->pstGh3xData->uchChnlNum,uchChnlIndex);
        if(uchChnlIndex == 0xFF){
            continue;
        }
        if(uchExistPace)
        {
            if (pstFrameInfo->pstGh3xData->unChnlBits & ((GU64)1 << uchChnlIndex))
            {
                GhGotOneFrameData(pstFrameInfo, &unFrameCnt,
                                 &usFrameNum,psGsFifo, fGsStep, &fGsFifoCnt);
            }
            GhFillGhRawdata(pstFrameInfo, uchChnlIndex, puchRawdataFifo, nSearchStartPosi);
        }
        else
        {
            GhFillGhRawdata(pstFrameInfo, uchChnlIndex, puchRawdataFifo, nSearchStartPosi);
            if (GhPopCount(pstFrameInfo->pstGh3xData->unChnlBits) == pstFrameInfo->pstGh3xData->uchChnlNum)
            {
                GhGotOneFrameData(pstFrameInfo, &unFrameCnt,
                                 &usFrameNum,psGsFifo, fGsStep, &fGsFifoCnt);
            }
        }
    }
    pstFrameInfo->pstGh3xData->uchChnlIndex = uchChnlIndex;
}

static void GhGotOneFrameData(const STFrameInfo * const pstFrameInfo,GU32* punFrameCnt,
                              GU16* pusFrameNum,GS16 *psGsFifo, float fGsStep, float* pfGsFifoCnt)
{
    pstFrameInfo->pstGh3xData->unChnlBits = 0;
    //calculate resample cnt
    GS16 sResampleCnt = GH3X_CalResampleCnt(pstFrameInfo);
    if(sResampleCnt != 0 && pstFrameInfo -> pstReSampleInfo != 0)
    {
        pstFrameInfo->pstFlagInfo->punFlag[6] |= 0x1;
        pstFrameInfo->pstFlagInfo->uchFlagBits |= (1 << 6);
    }
    //this frame will be resample to several frame
    while(sResampleCnt > 0)
    {
        if ((*(pstFrameInfo->punFrameCnt)) == 0)
        {
            guchFifoFullFlag = 0;
            pstFrameInfo->pstFlagInfo->punFlag[2] |= 0x2;
            pstFrameInfo->pstFlagInfo->uchFlagBits |= (1 << 2);
        }
        GU8 uchIsEdgeFrame = ((*punFrameCnt) == 0) ? 1 : 0;
        uchIsEdgeFrame = ((*punFrameCnt) == (*pusFrameNum) - 1) ? 2 : uchIsEdgeFrame;
        pstFrameInfo->pstFlagInfo->punFlag[6] |= (uchIsEdgeFrame << 8);
        pstFrameInfo->pstFlagInfo->uchFlagBits |= (1 << 6);

        sResampleCnt--;
        (*punFrameCnt)++;
        if(pstFrameInfo->uchFuncID == GH3X_FUNC_OFFSET_BIA)
        {
            GhBiaFifoFrameHandle(pstFrameInfo->pstFlagInfo->punFlag, &pstFrameInfo->pstFlagInfo->uchFlagBits,pusFrameNum,punFrameCnt);
        }
        //gsensor
        GH3X_FillGsensorData(psGsFifo, pfGsFifoCnt, fGsStep, pstFrameInfo->pstGsData);
        //get one frame rawdata
        GH3X_FrameDataHandle(pstFrameInfo);
        //clear buffer
        GhClearRawdataBuf(pstFrameInfo->pstFlagInfo->punFlag, &pstFrameInfo->pstFlagInfo->uchFlagBits, 
                            pstFrameInfo->pstGh3xData->punRawdata, sResampleCnt,pstFrameInfo->pstGh3xData->uchChnlNum);
        (*(pstFrameInfo->punFrameCnt))++;
    }
}

static void GhFillGhRawdata(const STFrameInfo * const pstFrameInfo,GU8 uchChnlIndex,char *puchRawdataFifo,int nSearchStartPosi)
{
    pstFrameInfo->pstGh3xData->unChnlBits |= ((GU64)1 << uchChnlIndex);

    GU32 unRawdata = 0;
    GU16 usTag = 0;
    //get teg and rawdata
    usTag = GH3X_MAKEUP_WORD(puchRawdataFifo[nSearchStartPosi], puchRawdataFifo[nSearchStartPosi + 1]);
    unRawdata = GH3X_MAKEUP_DWORD(puchRawdataFifo[nSearchStartPosi + 1] *(CHNLMAP_GET_DATATYPE(usTag) == DATATYPE_BG_CANCEL || CHNLMAP_GET_DATATYPE(usTag) == DATATYPE_LED_DRV),
                                puchRawdataFifo[nSearchStartPosi + 2] * (CHNLMAP_GET_DATATYPE(usTag) != DATATYPE_ECG || CHNLMAP_GET_ECGTYPE(usTag) != 1),
                                puchRawdataFifo[nSearchStartPosi + 3],
                                puchRawdataFifo[nSearchStartPosi + 4]);
    //Negative number handling
    if ((CHNLMAP_GET_DATATYPE(usTag) == DATATYPE_GSR ||\
            CHNLMAP_GET_DATATYPE(usTag) == DATATYPE_BIA ||\
            ((CHNLMAP_GET_ECGTYPE(usTag) != 1) && (CHNLMAP_GET_ECGTYPE(usTag) != 2) && (CHNLMAP_GET_DATATYPE(usTag) == DATATYPE_ECG))))
    {
        unRawdata = GH3X_u24toint24(unRawdata);
    }
    //store incomplete rawdata
    //flag
    GH3X_SetFlag(uchChnlIndex, usTag, pstFrameInfo->pstFlagInfo->punFlag, &pstFrameInfo->pstFlagInfo->uchFlagBits);
    //gain
    GH3X_SetGain(uchChnlIndex, usTag, pstFrameInfo->pstGh3xData->punAgcInfo);
    if (pstFrameInfo->pstGh3xData != GH3X_PTR_NULL)
    {
        pstFrameInfo->pstGh3xData->uchAgcNum = uchChnlIndex + 1;
    }
    //rawdata
    pstFrameInfo->pstGh3xData->punRawdata[uchChnlIndex] = unRawdata;
}


void GH3X_InitResample(const STFrameInfo * const pstFrameInfo)
{
    if(pstFrameInfo ->pstReSampleInfo != 0)
    {
        pstFrameInfo ->pstReSampleInfo ->fResampleFrameRest = 0;
    }
}

GU16 GH3X_GetFrameNum(GU8 *puchRawdataFifo, int nRawDataByteLen, const STFrameInfo * const pstFrameInfo)
{
    GU16 usFrameNum = 0;
    GU8 uchChnlNum = pstFrameInfo->pstGh3xData->uchChnlNum;
    GU8 uchChnlIndex = pstFrameInfo->pstGh3xData->uchChnlIndex;
    GU16 *pusChnlMap = pstFrameInfo->pstGh3xData->pusChnlMap;
    GU64 unChnlBits = pstFrameInfo->pstGh3xData->unChnlBits;
    //set incomplete flag
    GU8 uchExistPace = GH3X_ExistPace(pstFrameInfo);
    for (GU16 nSearchStartPosi = 0; nSearchStartPosi < nRawDataByteLen; nSearchStartPosi += GH3X_FIFO_RAWDATA_SIZE)
    {
        GU16 usTagTemp = GH3X_MAKEUP_WORD(puchRawdataFifo[nSearchStartPosi], 
                                          puchRawdataFifo[nSearchStartPosi + 1]);
        uchChnlIndex = GH3X_TagCompare(usTagTemp, pusChnlMap, uchChnlNum,uchChnlIndex);
        if(uchChnlIndex != 0xFF)
        {
            if(uchExistPace)
            {
                if (unChnlBits & ((GU64)1 << uchChnlIndex))
                {
                    usFrameNum++;
                    unChnlBits = 0;
                }
                unChnlBits |= ((GU64)1 << uchChnlIndex);
            }
            else
            {
                unChnlBits |= ((GU64)1 << uchChnlIndex);
                if(GhPopCount(unChnlBits) == uchChnlNum)
                {
                    usFrameNum++;
                    unChnlBits = 0;
                }
            }
        }
    }
    if(pstFrameInfo ->pstReSampleInfo != 0 &&
        pstFrameInfo -> pstReSampleInfo -> fRawdataFs != 0 &&
        pstFrameInfo ->pstGh3xData ->usSampleRate != (GS16)(0.5f + pstFrameInfo -> pstReSampleInfo -> fRawdataFs))
    {
        pstFrameInfo ->pstReSampleInfo ->sRawdataFrameNum = usFrameNum;
        GF32 fResampleFrameNum = (GF32)(usFrameNum * pstFrameInfo ->pstGh3xData ->usSampleRate)/(GF32)pstFrameInfo -> pstReSampleInfo -> fRawdataFs;
        pstFrameInfo ->pstReSampleInfo ->sResampleFrameNum = (fResampleFrameNum + pstFrameInfo ->pstReSampleInfo ->fResampleFrameRest + 0.5f);
        pstFrameInfo ->pstReSampleInfo ->fResampleFrameRest = fResampleFrameNum - pstFrameInfo ->pstReSampleInfo ->sResampleFrameNum;
        pstFrameInfo ->pstReSampleInfo ->sRawdataFrameCnt = 0;
        pstFrameInfo ->pstReSampleInfo ->sResampleFrameCnt = 0;
        usFrameNum = pstFrameInfo ->pstReSampleInfo ->sResampleFrameNum;
        pstFrameInfo ->pstReSampleInfo ->sAlgoCallCnt = pstFrameInfo ->pstReSampleInfo ->sResampleFrameNum;
        pstFrameInfo ->pstReSampleInfo ->sAlgoCallCnt = usFrameNum;
    }
    return usFrameNum;
}

void GH3X_FunctionProcess(STSensorRawdata* pstSensorRawdata)
{
    if (pstSensorRawdata == GH3X_PTR_NULL || 
            *pstSensorRawdata->pusGhRawdataCnt == 0 ||
            *pstSensorRawdata->pusGhRawdataCnt == 0)
    {
        return;
    }
    char *puchRawdataFifo = (char *)pstSensorRawdata->puchGhRawdataArr;
    int nRawdataFifoLen = *pstSensorRawdata->pusGhRawdataCnt;
    GS16 *psGsFifo = pstSensorRawdata->psGsRawdataArr;
    int nGsFifoLen = (psGsFifo == GH3X_PTR_NULL) ? 0 : *pstSensorRawdata->pusGsRawdataCnt;
 
    for(GU8 uchFunCnt = 0; uchFunCnt < GH3X_FUNC_OFFSET_MAX; uchFunCnt++)
    {
        if((gpstFrameInfo[uchFunCnt] != GH3X_PTR_NULL) &&
            (GH3X_GetDemoFuncMode() & (((GU32)1)<< uchFunCnt)))
        {
            GU16 usFrameNum = GH3X_GetFrameNum((GU8*)puchRawdataFifo, nRawdataFifoLen, gpstFrameInfo[uchFunCnt]);
            GH3X_FifoFrameHandle(puchRawdataFifo, nRawdataFifoLen,
                                psGsFifo, GH3X_CalSensorStep(nGsFifoLen / (3 + 3 * __GS_GYRO_ENABLE__), usFrameNum),
                                usFrameNum, gpstFrameInfo[uchFunCnt]);
        }
    }
}

void GH3X_InitFifoAnalyzeBuffer(void)
{

}


#define COMPRESS_BUF_LEN       32

GS32 gnEcgDataBuf[COMPRESS_BUF_LEN];

GS32 GH3X_u24toint24(GU32 unU24Val)
{
    GU32 ret = unU24Val;
    if(unU24Val & 1 << 23)
    {
        ret = unU24Val|(0xFF000000);
    }
    return (GS32)ret;
}
GS32 zigzag_decode(GU32 x)
{
    return (GS32)((x>>1) ^ ((~(x & 1)) + 1));
}

GU8 guchUploadArr[500];
void GH_UploadCompressTest(GU8 uchTag,GS32 gnEcgDcBuf[],GU16 usDataLen)
{
    for(int i = 0; i < usDataLen; i++)
    {
        GU32 un24 = gnEcgDcBuf[i];
        guchUploadArr[5*i+4] = uchTag;
        guchUploadArr[5*i+3] = 0;
        guchUploadArr[5*i+2] = ((un24 >> 16)& 0xFF);
        guchUploadArr[5*i+1] = ((un24 >> 8)& 0xFF);
        guchUploadArr[5*i+0] = ((un24 >> 0) & 0xFF);
    }
#if __SUPPORT_GHEALTH_PROTOCOL__
    GH3X_UprotocolSendFifoData(guchUploadArr, usDataLen*5,0);
#endif
}

#define GH_COMPRESS_LEN_BIT                (0x7F)
#define GH_COMPRESS_VAL_BIT                (0x7F)
#define GH_COMPRESS_VAR_LEN_BIT            (0x80)
 

GU8 GH3X_UncompressEcgData(GU8* puchCompressData, GU16 usDataByteLen,GS32* pnEcgDataBuf)
{
    //static GU8 uchCurrentState = 0;// 0:wait rawdata  1 :wait diff
    static GU8 uchCompressByte= 0;
    static GU8 uchTag= 0;
    static GU32 idx = 0;
    static GU32 unDiffVal = 0;
    if(uchCompressByte == 0)
    {
        uchCompressByte = ((puchCompressData[3]) & GH_COMPRESS_LEN_BIT);
        uchTag = puchCompressData[4];
        GU32 unFirstData = (puchCompressData[2] << 16) + (puchCompressData[1] << 8) + (puchCompressData[0] << 0);
        idx = 0;
        pnEcgDataBuf[idx++] = GH3X_u24toint24(unFirstData);
        unDiffVal = 0;
        return 0;
    }
    GU8* puchByte = puchCompressData;
    for(GU8 i = 0; i < usDataByteLen; i++)
    {
        GS32 nDiffIdx = i/GH3X_FIFO_RAWDATA_SIZE*GH3X_FIFO_RAWDATA_SIZE + (GH3X_FIFO_RAWDATA_SIZE - 1)-(i%GH3X_FIFO_RAWDATA_SIZE);
        if(puchByte[nDiffIdx] & GH_COMPRESS_VAR_LEN_BIT)
        {
            unDiffVal = (unDiffVal << 7) + (puchByte[nDiffIdx] & GH_COMPRESS_VAL_BIT);
        }
        else
        {
            unDiffVal = (unDiffVal << 7) + (puchByte[nDiffIdx] & GH_COMPRESS_VAL_BIT);
            pnEcgDataBuf[idx] = pnEcgDataBuf[idx-1] + zigzag_decode(unDiffVal);
            idx++;
            unDiffVal = 0;
        }
        uchCompressByte--;
        if(uchCompressByte == 0)
        {
            GH_UploadCompressTest(uchTag,pnEcgDataBuf,idx);
            return 1;
        }
    }
    return 0;
}   

 

void GH3X_DemoEcgUncompress(GU8* puchFifoData, GU16 usDataByteLen,GU32 unFifoFull)
{
    static GU16 usCompressByteLen = 0;
    if(usDataByteLen == 0)
    {
        return;
    }
    if(unFifoFull != 0)
    {
        while(1);
    }
    static GS32* punEcgBuf = 0;
    for(GU16 i = 0; i < usDataByteLen; i+= GH3X_FIFO_RAWDATA_SIZE)
    {
        if(usCompressByteLen == 0)
        {
            GU16 usTag = (puchFifoData[i+4] << 8) + puchFifoData[i+3];
            if((CHNLMAP_GET_DATATYPE(usTag) == DATATYPE_ECG) &&
                (CHNLMAP_GET_ECGTYPE(usTag) == 0  || CHNLMAP_GET_ECGTYPE(usTag) == 3) &&
               ((usTag & GH_COMPRESS_LEN_BIT) != 0))
            
            {
                switch(CHNLMAP_GET_ECGADCNO(usTag))
                {
                    case 1:
                        punEcgBuf = gnEcgDataBuf;
                    break;
                    case 2:
                        punEcgBuf = gnEcgDataBuf;
                    break;
                    case 3:
                        punEcgBuf = gnEcgDataBuf;
                    break;
                    default:
                    break;
                }
                if(punEcgBuf == 0)
                {
                    while(1);
                    //continue;
                }
                usCompressByteLen = (usTag & GH_COMPRESS_LEN_BIT);
                 
                GH3X_UncompressEcgData(puchFifoData + i, GH3X_FIFO_RAWDATA_SIZE,punEcgBuf);
            }
        }
        else
        {
            if(punEcgBuf == 0)
            {
                while(1);
            }
            if(1 == GH3X_UncompressEcgData(puchFifoData + i, GH3X_FIFO_RAWDATA_SIZE,punEcgBuf))
            {
                punEcgBuf = 0;
                usCompressByteLen = 0;
            }
        }
    }
}
