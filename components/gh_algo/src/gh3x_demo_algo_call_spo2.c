#include <stdio.h>
#include "gh3x_demo_algo_call.h"
#include "gh3x_demo_algo_call_inner.h"
#include "gh3x_demo_algo_config.h"
#include "goodix_log.h"
#include "goodix_spo2.h"

#if (__USE_GOODIX_SPO2_ALGORITHM__)
#define CHIP_PPG_CHL_NUM        (4)
const void *goodix_spo2_config_get_instance(void);
int32_t  goodix_spo2_config_get_size(void);
void goodix_spo2_config_get_version(char* ver,uint8_t ver_len);

goodix_spo2_ret goodix_spo2_init_func(GU32 fs)
{
    goodix_spo2_ret stAlgoRet = GX_ALGO_SPO2_SUCCESS;

    GH3X_Spo2AlgoChnlMapDefultSet();

    char chVer[SPO2_INTERFACE_VERSION_LEN_MAX] = {0};
    goodix_spo2_config_get_version(chVer, SPO2_INTERFACE_VERSION_LEN_MAX);
    GH3X_ALGO_LOG_PARAM("spo2 algorithm version : %s\r\n", chVer);
    GH3X_ALGO_LOG_PARAM("spo2 algorithm legal chnl num : %d\r\n", gstSpo2AlgoChnlMap.uchNum);

    if(gstSpo2AlgoChnlMap.uchNum > GH3X_GetSpo2AlgoSupportChnl())
    {
        GH3X_ALGO_LOG_PARAM("ERROR: spo2 algorithm legal chnl num is too big, check cfg !!! \r\n");
        return GX_ALGO_SPO2_RWONG_INPUT;//while(1);
    }
 
    goodix_spo2_config stSpo2Cfg ;
    GH3X_Memcpy(&stSpo2Cfg,goodix_spo2_config_get_instance(),goodix_spo2_config_get_size());
    stSpo2Cfg.valid_chl_num = gstSpo2AlgoChnlMap.uchNum;
    stSpo2Cfg.raw_fs = fs;
  
    GH3X_SAMPLE_ALGO_LOG_PARAM("[%s]:params = %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,\r\n", __FUNCTION__,
                                                                            (unsigned int)stSpo2Cfg.valid_chl_num,
                                                                            (unsigned int)stSpo2Cfg.raw_fs,
                                                                            (unsigned int)stSpo2Cfg.hb_en_flg,
                                                                            (unsigned int)stSpo2Cfg.wear_mode,
                                                                            (unsigned int)stSpo2Cfg.acc_thr_max,
                                                                            (unsigned int)stSpo2Cfg.acc_thr_min,
                                                                            (unsigned int)stSpo2Cfg.acc_thr_scale,
                                                                            (unsigned int)stSpo2Cfg.acc_thr_num,
                                                                            (unsigned int)stSpo2Cfg.ctr_en_flg,
                                                                            (unsigned int)stSpo2Cfg.ctr_red_thr);

    stAlgoRet = goodix_spo2_init(&stSpo2Cfg,sizeof(stSpo2Cfg),chVer);
    return stAlgoRet;
}

goodix_spo2_ret goodix_spo2_deinit_func(void)
{
    goodix_spo2_deinit();
    return GX_ALGO_SPO2_SUCCESS;
}

GS8 GH3XSpo2AlgoInit(const STFrameInfo * const pstFrameInfo)
{
	GH3X_ALGO_LOG_PARAM("%s\r\n", __FUNCTION__);
	GS8 chRet = GH3X_RET_GENERIC_ERROR;
    memset(&pstFrameInfo->pstAlgoData->uchFlag, 0, sizeof(GU8) + sizeof(GU8) + sizeof(GU32));

    if (goodix_spo2_init_func(pstFrameInfo ->pstGh3xData->usSampleRate) == GX_ALGO_SPO2_SUCCESS)
    {
        chRet = GH3X_RET_OK;
    }
    else
    {
        GH3X_ALGO_LOG_PARAM("spo2 init error! error code:0x%x\r\n", chRet);
    }
    return chRet;
}


GS8 GH3XSpo2AlgoDeinit(const STFrameInfo * const pstFrameInfo)
{
    GS8 chRet = GH3X_RET_GENERIC_ERROR;

    GH3X_ALGO_LOG_PARAM("%s\r\n", __FUNCTION__);
    if (goodix_spo2_deinit_func() == GX_ALGO_SPO2_SUCCESS)
    {
        chRet = GH3X_RET_OK;
    }
    else
    {
        GH3X_ALGO_LOG_PARAM("spo2 deinit error! error code:0x%x\r\n", chRet);
    }
    return chRet;
}


GS8 GH3XSpo2AlgoExe(const STFrameInfo * const pstFrameInfo)
{
    if(0 == pstFrameInfo )
    {
        return GH3X_RET_GENERIC_ERROR;
    }
    int32_t ppg_rawdata[3 * CHIP_PPG_CHL_NUM] = {0};
    GS8 cur_adj_flg[3 * CHIP_PPG_CHL_NUM] = {0};
    GS8 gain_adj_flg[3 * CHIP_PPG_CHL_NUM] = {0};
    GS8 enable_flg[3 * CHIP_PPG_CHL_NUM] = {0};
    int32_t ch_agc_drv0[3 * CHIP_PPG_CHL_NUM] = {0};
    int32_t ch_agc_drv1[3 * CHIP_PPG_CHL_NUM] = {0};
    GS8 ch_agc_gain[3 * CHIP_PPG_CHL_NUM] = {0};
    goodix_spo2_input_rawdata stRawdata = {0};
    stRawdata.ppg_rawdata = ppg_rawdata;
    stRawdata.cur_adj_flg = cur_adj_flg;
    stRawdata.gain_adj_flg = gain_adj_flg;
    stRawdata.enable_flg = enable_flg;
    stRawdata.ch_agc_drv0 = ch_agc_drv0;
    stRawdata.ch_agc_drv1 = ch_agc_drv1;
    stRawdata.ch_agc_gain = ch_agc_gain;

    pstFrameInfo->pstAlgoData->uchFlag =  0;
    pstFrameInfo->pstAlgoData->uchResultNum = 0;
    goodix_spo2_result stResult;
    goodix_spo2_ret emAlgoRet = GX_ALGO_SPO2_SUCCESS;
    
    //GU8 uchChnlNum = pstFrameInfo->pstFunctionInfo->uchChnlNum; 
    stRawdata.frameid = GH3X_GET_BYTE0_FROM_DWORD(*(pstFrameInfo->punFrameCnt));
    stRawdata.bit_num = 24;
    stRawdata.acc_x = pstFrameInfo->pstGsData->pusData[0];
    stRawdata.acc_y = pstFrameInfo->pstGsData->pusData[1];
    stRawdata.acc_z = pstFrameInfo->pstGsData->pusData[2];
    stRawdata.wear_on_flag = 1;
    
    /* call algorithm, update result */
    memset(&stResult, 0, sizeof(goodix_spo2_result));


    GU8 uchChMapCnt = 0;
    //GU8 uchGainValue = GH3X_GAIN_VALUE_INVALID;
    
    stRawdata.wear_on_flag = 1;

    for (uchChMapCnt = 0; uchChMapCnt < CHIP_PPG_CHL_NUM * 3; ++uchChMapCnt)
    {
        if (gstSpo2AlgoChnlMap.uchAlgoChnlMap[uchChMapCnt + 4 * (uchChMapCnt / 4)] < 0xff && ((uchChMapCnt % 4) < gstSpo2AlgoChnlMap.uchNum))
        {
            GU8 uchRawdataIndex = gstSpo2AlgoChnlMap.uchAlgoChnlMap[uchChMapCnt + 4 * (uchChMapCnt / 4)];

            stRawdata.ch_agc_drv0[uchChMapCnt]  = ((pstFrameInfo->pstGh3xData->punAgcInfo[uchRawdataIndex]>>8) & 0xFF);
            stRawdata.ch_agc_drv1[uchChMapCnt]  = ((pstFrameInfo->pstGh3xData->punAgcInfo[uchRawdataIndex]>>16) & 0xFF);
            stRawdata.ch_agc_gain[uchChMapCnt]  = (pstFrameInfo->pstGh3xData->punAgcInfo[uchRawdataIndex] & 0xF);

            stRawdata.gain_adj_flg[uchChMapCnt] = 1;

            stRawdata.cur_adj_flg[uchChMapCnt] = (((pstFrameInfo->pstFlagInfo->punFlag[0]) >> uchRawdataIndex) & 0x01);
            stRawdata.enable_flg[uchChMapCnt]  = 1;
            
            stRawdata.ppg_rawdata[uchChMapCnt] = pstFrameInfo->pstGh3xData->punRawdata[uchRawdataIndex];
            GH3X_ALGO_LOG_PARAM("[%s]ppg_rawdata[%d] = %d,gain_adj_flg = %d,cur_adj_flg = %d\r\n",__FUNCTION__,\
                (int)uchChMapCnt,\
                (int)stRawdata.ppg_rawdata[uchChMapCnt],\
                (int)stRawdata.gain_adj_flg[uchChMapCnt],\
                (int)stRawdata.cur_adj_flg[uchChMapCnt]);
        }
    }
    stRawdata.ch_num = CHIP_PPG_CHL_NUM;
	
    emAlgoRet = goodix_spo2_calc(&stRawdata, &stResult);
    if (emAlgoRet == GX_ALGO_SPO2_SUCCESS || emAlgoRet == GX_ALGO_SPO2_FRAME_UNCOMPLETE || emAlgoRet == GX_ALGO_SPO2_WIN_UNCOMPLETE)
    {
        pstFrameInfo->pstAlgoData->uchFlag = (GU8)stResult.final_calc_flg;
        if (stResult.final_calc_flg == 1)
        {
            pstFrameInfo->pstAlgoData->pnResults[0] = (GS32)(GH3X_Round((float)stResult.final_spo2/10000));
            pstFrameInfo->pstAlgoData->pnResults[1] = (GS32)stResult.final_r_val;
            pstFrameInfo->pstAlgoData->pnResults[2] = (GS32)stResult.final_confi_coeff;
            pstFrameInfo->pstAlgoData->pnResults[3] = (GS32)stResult.final_valid_level;
            pstFrameInfo->pstAlgoData->pnResults[4] = (GS32)stResult.final_hb_mean;
            pstFrameInfo->pstAlgoData->pnResults[5] = (GS32)stResult.final_invalidFlg;
            pstFrameInfo->pstAlgoData->unResultBits = 0x7F;
            pstFrameInfo->pstAlgoData->uchResultNum = GH3X_BitCount(pstFrameInfo->pstAlgoData->unResultBits);
            GH3X_Spo2AlgorithmResultReport(pstFrameInfo->pstAlgoData,pstFrameInfo->punFrameCnt[0]);
            GH3X_ALGO_LOG_PARAM("[%s]spo2 = %d, r_val = %d\r\n",__FUNCTION__,pstFrameInfo->pstAlgoData->pnResults[0],pstFrameInfo->pstAlgoData->pnResults[1]);
        }
    }
    else
    {
        GH3X_ALGO_LOG_PARAM("error code = 0x%x, please feedback to GOODIX!\r\n", (int)emAlgoRet);
    }
    return (GS8)emAlgoRet;
}

#endif
