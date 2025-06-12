
#include <stdio.h>
#include "gh3x_demo_algo_call.h"
#include "gh3x_demo_algo_call_inner.h"
#include "gh3x_demo_algo_config.h"
#include "goodix_ecg.h"

#if (__USE_GOODIX_ECG_ALGORITHM__)

#define GH3X_SINGLE_CLASSIFICATION_TIME 32
#define PACE_RECORD_LEN       (255)
#define PACE_MIN_INTERVAL       (50)
GU32 gunPaceTimestampRecord[PACE_RECORD_LEN + 1] = {0};
GU32 gunPaceWidthAndHigh[PACE_RECORD_LEN + 1] = {0};
GU32 gunPaceTimestampPre = 0;
//GU32 gunPaceTimestampLater = 0;
GU8 guchPaceTimestampRecordReadCnt = 0;
GU8 guchPaceTimestampRecordWriteCnt = 0;
GU8 guchPaceTimestampRecordCnt = 0;
goodix_ecg_init_result g_stEcgInitResult;

void  GH3X_ResetPaceBuffer(void)
{
    guchPaceTimestampRecordReadCnt = 0;
    guchPaceTimestampRecordWriteCnt = 0;
    guchPaceTimestampRecordCnt = 0;
    gunPaceTimestampPre = 0;
    memset(gunPaceTimestampRecord, 0, (PACE_RECORD_LEN + 1)*sizeof(GU32));
    memset(gunPaceWidthAndHigh, 0, (PACE_RECORD_LEN + 1)*sizeof(GU32));
    
}
const void *goodix_ecg_config_get_arr(void);
int32_t goodix_ecg_config_get_size(void);
void goodix_ecg_config_get_version(char* ver, uint8_t ver_len);
goodix_ecg_config stEcgCfg;
goodix_ecg_ret goodix_ecg_init_func(GU32 fs)
{
    char chVer[ECG_INTERFACE_VERSION_LEN_MAX] = {0};
    goodix_ecg_config_get_version(chVer, ECG_INTERFACE_VERSION_LEN_MAX);
    GH3X_ALGO_LOG_PARAM("ecg algorithm version : %s\r\n", chVer);
     
    GH3X_Memset(&stEcgCfg,0,sizeof(stEcgCfg));
    GH3X_Memcpy(&stEcgCfg,goodix_ecg_config_get_arr(),goodix_ecg_config_get_size());

    stEcgCfg.fs = fs;
//  stEcgCfg.fs = 500;  
    stEcgCfg.valid_channel_num = (uint32_t)gnEcgParamsArr[0];
    stEcgCfg.qrs_flag =  (uint32_t)gnEcgParamsArr[1];
    stEcgCfg.mode_type = (uint32_t)gnEcgParamsArr[2];
    stEcgCfg.ic_type = 1;
    GH3X_ALGO_LOG_PARAM("[%s]:params = %d,%d,%d,%d,%d,\r\n", __FUNCTION__, stEcgCfg.fs, stEcgCfg.valid_channel_num, stEcgCfg.qrs_flag,stEcgCfg.mode_type,stEcgCfg.ic_type);
    goodix_ecg_ret chRet = goodix_ecg_init(&stEcgCfg,&g_stEcgInitResult,sizeof(stEcgCfg), chVer);
    guchClassificationFlag = 0;
    gunEcgTimeSeconds = 0;
    GH3X_ResetPaceBuffer();
    return chRet;
}

goodix_ecg_ret goodix_ecg_deinit_func(void)
{
    goodix_ecg_deinit();
    return GX_ALGO_ECG_SUCCESS;
}

GS8 GH3XEcgAlgoInit(const STFrameInfo * const pstFrameInfo)
{
    memset(&pstFrameInfo->pstAlgoData->uchFlag, 0, sizeof(GU8) + sizeof(GU8) + sizeof(GU32));

    GS8 chRet = GH3X_RET_GENERIC_ERROR;
    GU16 usSampleRate = pstFrameInfo ->pstGh3xData->usSampleRate;    
    GH3X_ALGO_LOG_PARAM("%s\r\n", __FUNCTION__);
	
	goodix_ecg_ret ret = goodix_ecg_init_func(usSampleRate);
    if (ret == GX_ALGO_ECG_SUCCESS)
    {
        chRet = GH3X_RET_OK;
    }
    else
    {
        GH3X_ALGO_LOG_PARAM("ecg init error! ret = 0x%x\r\n", ret);
    }
    gunEcgFrameCnt  = 0;
    GH3X_SetNewDataFlag(GH3X_FUNC_OFFSET_ECG, GH3X_IS_NEW_RAWDATA);

    return chRet;
}


GS8 GH3XEcgAlgoDeinit(const STFrameInfo * const pstFrameInfo)
{
    GS8 chRet = GH3X_RET_GENERIC_ERROR;

    GH3X_ALGO_LOG_PARAM("%s\r\n", __FUNCTION__);
	goodix_ecg_ret ret = goodix_ecg_deinit_func();
    if (ret == GX_ALGO_ECG_SUCCESS)
    {
        chRet = GH3X_RET_OK;
    }
    else
    {
        GH3X_ALGO_LOG_PARAM("ecg deinit error! ret = 0x%x\r\n", ret);
    }
    return chRet;
}

GS8 GhPaceDetDelayProcess(const STFrameInfo * const pstFrameInfo)
{
    if(0 == pstFrameInfo)
    {
        return GH3X_RET_GENERIC_ERROR;
    }

    if(pstFrameInfo->pstFlagInfo->punFlag[3])
    {
        pstFrameInfo->pstFlagInfo->punFlag[3] = 0;
        if(*pstFrameInfo->punFrameCnt > 1000 && guchPaceTimestampRecordCnt < PACE_RECORD_LEN)
        {
            gunPaceTimestampRecord[guchPaceTimestampRecordWriteCnt] = *pstFrameInfo->punFrameCnt + g_stEcgInitResult.out_delay;
            gunPaceWidthAndHigh[guchPaceTimestampRecordWriteCnt] = pstFrameInfo->pstGh3xData->punRawdata[1];
            
            guchPaceTimestampRecordWriteCnt++;
            guchPaceTimestampRecordCnt ++;
    
        }
    }
    if(guchPaceTimestampRecordCnt >= 200)
    {
        GH3X_ResetPaceBuffer();
    }
    if(guchPaceTimestampRecordCnt > 0 && *pstFrameInfo->punFrameCnt == gunPaceTimestampRecord[guchPaceTimestampRecordReadCnt])
    {
        //check whether the time interval 
        GU8 uchCheckRes = 0;
        GU32 unTimeInterval = 0;
        if(guchPaceTimestampRecordCnt > 1)
        {
            GU8 guchPaceTimestampRecordTempReadCnt = guchPaceTimestampRecordReadCnt + 1;
            unTimeInterval = gunPaceTimestampRecord[guchPaceTimestampRecordTempReadCnt] - gunPaceTimestampRecord[guchPaceTimestampRecordReadCnt];
            if(unTimeInterval >= PACE_MIN_INTERVAL)
            {
                uchCheckRes |= 0x01;
            }
        }
        else
        {
            uchCheckRes |= 0x01;
        }
        if(gunPaceTimestampPre == 0)
        {
            uchCheckRes |= 0x02;
        }
        else
        {
            unTimeInterval = gunPaceTimestampRecord[guchPaceTimestampRecordReadCnt] - gunPaceTimestampPre;
            if(unTimeInterval >= PACE_MIN_INTERVAL)
            {
                uchCheckRes |= 0x02;
            }
        }
        gunPaceTimestampPre = gunPaceTimestampRecord[guchPaceTimestampRecordReadCnt];
        if(uchCheckRes == 3)
        {
            pstFrameInfo->pstFlagInfo->punFlag[3] = 1;
            pstFrameInfo->pstFlagInfo->punFlag[4] = ((gunPaceWidthAndHigh[guchPaceTimestampRecordReadCnt] >> 18) & 0x3F) * 50; //width
            GU32 paceH_rawdata = gunPaceWidthAndHigh[guchPaceTimestampRecordReadCnt] & 0x3FFFF;//原始数据读出的18bit无符号数
            GS32  paceH_signed_data;
            if(paceH_rawdata & 0x20000)
            {
                paceH_rawdata = (paceH_rawdata | 0xFFFC0000);
            }
            paceH_signed_data = (GS32)paceH_rawdata;
            double paceH = paceH_signed_data/(double)65536;
            int gain = 1;
            double paceH_mV = paceH/ gain * 1.8 * 16 / 15 * 1000;
            //double paceH_mV = paceH/ gain * 1.8 * 1 * 1000;
            int upload_h = paceH_mV * 1000;//uV
            //upload_h = (upload_h < 0) ? (-upload_h) : upload_h;
            pstFrameInfo->pstFlagInfo->punFlag[5] = upload_h;

            pstFrameInfo->pstFlagInfo->uchFlagBits |= (1<<3);
            pstFrameInfo->pstFlagInfo->uchFlagBits |= (1<<4);
            pstFrameInfo->pstFlagInfo->uchFlagBits |= (1<<5);
        }
        guchPaceTimestampRecordReadCnt++;
        guchPaceTimestampRecordCnt--;
    }
    return GX_ALGO_ECG_SUCCESS;
}

GS8 GH3XEcgAlgoExe(const STFrameInfo * const pstFrameInfo)
{
    if(0 == pstFrameInfo)
    {
        return GH3X_RET_GENERIC_ERROR;
    }

    GhPaceDetDelayProcess(pstFrameInfo);
    pstFrameInfo->pstAlgoData->uchFlag = 0;
    goodix_ecg_input_rawdata stRawdata = {0};
    goodix_ecg_result stResult;
    GS8 chAlgoRet = 0;

    GF32 fEcgOutData[ECG_CHANNEL_NUM] = {0};
    goodix_stage unEcgStage[ECG_CHANNEL_NUM] = {stage_noneffect_out, stage_noneffect_out};
    GF32 fEcgSnr[ECG_CHANNEL_NUM] = {0};
    GF32 fEcgPowerLineOut[ECG_CHANNEL_NUM] = {0};
    GF32 fEcgBlwOut[ECG_CHANNEL_NUM] = {0};
    GF32 fEcgBpmCh[ECG_CHANNEL_NUM] = {0};
    GS32 ecg_rawdataarr[ECG_CHANNEL_NUM] = {0};
    stResult.ecg_out = fEcgOutData;
    stResult.ecg_stage = unEcgStage;
    stResult.ecg_snr = fEcgSnr;
    stResult.ecg_powerline_out = fEcgPowerLineOut;
    stResult.ecg_blw_out = fEcgBlwOut;
    stResult.ecg_bpm_ch = fEcgBpmCh;
    stRawdata.ecg_rawdata = (GS32 *)&ecg_rawdataarr; //(GS32 *)&ecg_rawdataarr;
    //GU8 uchChnlNum = pstFrameInfo->pstFunctionInfo->uchChnlNum; 
    stRawdata.frameid = GH3X_GET_BYTE0_FROM_DWORD(*(pstFrameInfo->punFrameCnt));
    stRawdata.acc_x = pstFrameInfo->pstGsData->pusData[0];
    stRawdata.acc_y = pstFrameInfo->pstGsData->pusData[1];
    stRawdata.acc_z = pstFrameInfo->pstGsData->pusData[2];
    stRawdata.ecg_rawdata[0] = pstFrameInfo->pstGh3xData->punRawdata[0];
    stRawdata.ecg_rawdata[1] = pstFrameInfo->pstGh3xData->punRawdata[1];

    stRawdata.ecg_qc_flag = GH3X_CHECK_BIT_IS_SET(pstFrameInfo->pstFlagInfo->punFlag[2], 0x10);
     
    if((stRawdata.ecg_qc_flag) && stEcgCfg.mode_type == 0)
    {
        return chAlgoRet;
    }
    goodix_ecg_ret stRet = GX_ALGO_ECG_SUCCESS;
    stRet = goodix_ecg_update(&stRawdata, &stResult);
 
    if (stRet == GX_ALGO_ECG_SUCCESS)
    {
        pstFrameInfo->pstAlgoData->uchFlag = 1;
        pstFrameInfo->pstAlgoData->pnResults[0] = GH3X_Round(stResult.ecg_out[0] * ECG_OUT_FACTOR);
        pstFrameInfo->pstAlgoData->pnResults[2] = GH3X_Round(stResult.ecg_out[1] * ECG_OUT_FACTOR);

        GH3X_ALGO_LOG_PARAM("ecg out0 = %.3f\r\n",stResult.ecg_out[0]);
        GH3X_ALGO_LOG_PARAM("ecg out1 = %.3f\r\n",stResult.ecg_out[1]);
        GH3X_ALGO_LOG_PARAM("bpm result = %d\r\n",(GS32)(stResult.ecg_bpm + 0.5f));

        pstFrameInfo->pstAlgoData->pnResults[1] = (GS32)(stResult.ecg_bpm + 0.5f);
        pstFrameInfo->pstAlgoData->unResultBits = 0x1 |\
                                                 ((0x1 << 1) * (pstFrameInfo->pstAlgoData->pnResults[1] != 0)) |\
                                                 ((0x1 << 2) * (gnEcgParamsArr[1] == 2));
        pstFrameInfo->pstAlgoData->uchResultNum = GH3X_BitCount(pstFrameInfo->pstAlgoData->unResultBits);

        GH3X_EcgAlgorithmResultReport(pstFrameInfo->pstAlgoData,pstFrameInfo->punFrameCnt[0]);
    }
    else
    {
        GH3X_ALGO_LOG_PARAM("no enough ram for algorithm error!\r\n");
        GH3X_ALGO_LOG_PARAM("please feedback to GOODIX!\r\n");
        chAlgoRet = GH3X_RET_RESOURCE_ERROR;
    }
    return chAlgoRet;
}

GU8 GH3XEcgAlgoClassification(void)
{
    goodix_ecg_classification_update((goodix_classification_flag*)&guchClassificationFlag);
    return guchClassificationFlag;
}


#endif
