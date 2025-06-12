#include <stdio.h>
#include "gh3x_demo_algo_call.h"
#include "gh3x_demo_algo_call_inner.h"
#include "goodix_hba.h"
#include "goodix_sys_hrv.h"

#if (__USE_GOODIX_HRV_ALGORITHM__)

GU32 gunHrvLastHrResult = 0;
GU8 guchLastGainValue = 0;
extern GU8 guchHrvInterEnable;
extern GS32 gnHrvParamsArr[5];
const void *goodix_hrv_config_get_arr(void);
int32_t goodix_hrv_config_get_size(void);
void goodix_hrv_config_get_version(char* ver, uint8_t ver_len);
void gh3x_set_hrv_hr(GU32 unHrvLastHrResult)
{
    gunHrvLastHrResult = unHrvLastHrResult;
}
//hrv
goodix_hrv_ret goodix_hrv_init_func(GU32 fs)
{
    goodix_hrv_ret stAlgoRet = GX_HRV_ALGO_OK;
    char chVer[HRV_INTERFACE_VERSION_LEN_MAX] = {0};
    goodix_hrv_config_get_version(chVer, HRV_INTERFACE_VERSION_LEN_MAX);
    GH3X_ALGO_LOG_PARAM("hrv algorithm version : %s\r\n", chVer);
    goodix_hrv_config stHrvConfig;
    GH3X_Memcpy(&stHrvConfig,goodix_hrv_config_get_arr(),goodix_hrv_config_get_size());
    stHrvConfig.fs = fs; 
		
    GH3X_ALGO_LOG_PARAM("[%s]:params = %d,%d,%d,%d,%d,%d,\r\n", __FUNCTION__,
                                                                stHrvConfig.need_ipl,
                                                                stHrvConfig.fs,
                                                                stHrvConfig.acc_thr[0],
                                                                stHrvConfig.acc_thr[1],
                                                                stHrvConfig.acc_thr[2],
                                                                stHrvConfig.acc_thr[3]);
    stAlgoRet = goodix_hrv_init(&stHrvConfig, sizeof(stHrvConfig), chVer);

    if (stAlgoRet != GX_HRV_ALGO_OK)
    {
        GH3X_ALGO_LOG_PARAM("hrv init errorcode : %x\r\n", (unsigned int)stAlgoRet);
    }
    return stAlgoRet;
}

goodix_hrv_ret goodix_hrv_deinit_func(void)
{
    goodix_hrv_ret stAlgoRet = GX_HRV_ALGO_OK;
    stAlgoRet = goodix_hrv_deinit();
    if (stAlgoRet != GX_HRV_ALGO_OK)
    {
        GH3X_ALGO_LOG_PARAM("hrv init errorcode : %x\r\n", (unsigned int)stAlgoRet);
    }
    return stAlgoRet;
}

GS8 GH3XHrvAlgoInit(const STFrameInfo * const pstFrameInfo)
{
    memset(&pstFrameInfo->pstAlgoData->uchFlag, 0, sizeof(GU8) + sizeof(GU8) + sizeof(GU32));

    GS8 chRet = GH3X_RET_GENERIC_ERROR;

    GH3X_ALGO_LOG_PARAM("%s\r\n", __FUNCTION__);
    if (goodix_hrv_init_func(pstFrameInfo ->pstGh3xData->usSampleRate) == GX_ALGO_HBA_SUCCESS)        
    {
        pstFrameInfo->pstAlgoData->unResultBits = 0x7F;
        chRet = GH3X_RET_OK;
    }
    else
    {
        GH3X_ALGO_LOG_PARAM("hrv init error!\r\n");
    }
    gpstGH3XAlgoFrameInfo[GH3X_FUNC_OFFSET_HR]->pstAlgoData->uchFlag = 0;
    gunHrvLastHrResult = 0;
    return chRet;
}



GS8 GH3XHrvAlgoDeinit(const STFrameInfo * const pstFrameInfo)
{
    GS8 chRet = GH3X_RET_GENERIC_ERROR;


    GH3X_ALGO_LOG_PARAM("%s\r\n", __FUNCTION__);
    if (goodix_hrv_deinit_func() == 0)
    {
        chRet = GH3X_RET_OK;
    }
    else
    {
        GH3X_ALGO_LOG_PARAM("hrv deinit error!\r\n");
    }
    return chRet;
}


GS8 GH3XHrvAlgoExe(const STFrameInfo * const pstFrameInfo)
{
    if(0 == pstFrameInfo )
    {
        return GH3X_RET_GENERIC_ERROR;
    }
    pstFrameInfo->pstAlgoData->uchFlag =  0;
    GS8 chAlgoRet = GH3X_RET_OK;
    GU8 uchGainValue = 0;
    GS32 ppg_rawdata[PPG_CHANNEL_NUM] = {0};
    GS32 cur_adj_flg[PPG_CHANNEL_NUM] = {0};
    GS32 gain_adj_flg[PPG_CHANNEL_NUM] = {0};
    goodix_hrv_result stResult = {0};
    goodix_hrv_input_rawdata stRawdataInput = {0};  
    stRawdataInput.ppg_rawdata = ppg_rawdata;
    stRawdataInput.cur_adj_flg = cur_adj_flg;
    stRawdataInput.gain_adj_flg = gain_adj_flg;
    stRawdataInput.bit_num = 24;
    stRawdataInput.acc_x = pstFrameInfo->pstGsData->pusData[0];
    stRawdataInput.acc_y = pstFrameInfo->pstGsData->pusData[1];
    stRawdataInput.acc_z = pstFrameInfo->pstGsData->pusData[2];
    stRawdataInput.frame_id = GH3X_GET_BYTE0_FROM_DWORD(*(pstFrameInfo->punFrameCnt));

    stRawdataInput.hr = gunHrvLastHrResult;

    for (GS32 nRawdatacnt = 0; nRawdatacnt < PPG_CHANNEL_NUM; nRawdatacnt++)
    {
        stRawdataInput.ppg_rawdata[nRawdatacnt] = pstFrameInfo->pstGh3xData->punRawdata[nRawdatacnt];
        uchGainValue = GH3X_GET_LOW_4BITS(pstFrameInfo->pstGh3xData->punAgcInfo[nRawdatacnt]);

        if (guchLastGainValue != uchGainValue) 
        {
            stRawdataInput.gain_adj_flg[nRawdatacnt] = 1;
        }
        guchLastGainValue = uchGainValue;
        stRawdataInput.cur_adj_flg[nRawdatacnt] = ((pstFrameInfo->pstFlagInfo->punFlag[0]) & 0x01);
        GH3X_ALGO_LOG_PARAM("[%s]ppg_rawdata = %d, gain_adj_flg = %d, cur_adj_flg = %d\r\n", __FUNCTION__, \
            (int)stRawdataInput.ppg_rawdata[nRawdatacnt], (int)stRawdataInput.gain_adj_flg[nRawdatacnt], \
            (int)stRawdataInput.cur_adj_flg[nRawdatacnt]);
    }

    goodix_hrv_ret ret = goodix_hrv_calc(&stRawdataInput, &stResult);
    if (GX_HRV_ALGO_OK == ret || GX_HRV_ALGO_UPDATE == ret)
    {
        if (GX_HRV_ALGO_UPDATE == ret)
        {
            pstFrameInfo->pstAlgoData->uchFlag = 1;
            pstFrameInfo->pstAlgoData->pnResults[0] = (GS32)stResult.rri[0];
            pstFrameInfo->pstAlgoData->pnResults[1] = (GS32)stResult.rri[1];
            pstFrameInfo->pstAlgoData->pnResults[2] = (GS32)stResult.rri[2];
            pstFrameInfo->pstAlgoData->pnResults[3] = (GS32)stResult.rri[3];
            pstFrameInfo->pstAlgoData->pnResults[4] = (GS32)stResult.rri_confidence;
            pstFrameInfo->pstAlgoData->pnResults[5] = (GS32)stResult.rri_valid_num;
            pstFrameInfo->pstAlgoData->unResultBits = 0x7F;
            pstFrameInfo->pstAlgoData->uchResultNum = GH3X_BitCount(pstFrameInfo->pstAlgoData->unResultBits);
            GH3X_HrvAlgorithmResultReport(pstFrameInfo->pstAlgoData, pstFrameInfo->punFrameCnt[0]);
        }
    }
    else
    {
        chAlgoRet = GH3X_RET_RESOURCE_ERROR;
        GH3X_ALGO_LOG_PARAM("GH3x2xHrAlgoExe error! error code = 0x%x\r\n", ret);
        GH3X_ALGO_LOG_PARAM("please feedback to GOODIX!\r\n");
    }

    return chAlgoRet;
}

#endif
