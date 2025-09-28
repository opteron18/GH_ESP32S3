#include <stdio.h>
#include "gh3x_demo_algo_call.h"
#include "gh3x_demo_algo_call_inner.h"
#include "gh3x_demo_algo_config.h"
#include "goodix_hba.h"

#if (__USE_GOODIX_HR_ALGORITHM__)


void GH3XSetHbaMode(GS32 nHbaScenario)
{
    gunHrParamsArr[1] = nHbaScenario;
}
void goodix_hba_config_get_version(char* ver, uint8_t ver_len);
const void *goodix_hba_config_get_arr(void);
int32_t  goodix_hba_config_get_size(void);

goodix_hba_ret goodix_hba_init_func(GU32 fs)
{
    goodix_hba_config stHbCfg;
    goodix_hba_ret stAlgoRet = GX_ALGO_HBA_SUCCESS;
    GH3X_Memcpy(&stHbCfg,goodix_hba_config_get_arr(),goodix_hba_config_get_size());

    char chVer[HBA_INTERFACE_VERSION_LEN_MAX] = {0};
    goodix_hba_config_get_version(chVer, HBA_INTERFACE_VERSION_LEN_MAX); 
    
    GH3X_HbaAlgoChnlMapDefultSet();
    
    GH3X_ALGO_LOG_PARAM("hba algorithm version : %s\r\n", chVer);
    GH3X_ALGO_LOG_PARAM("hba algorithm legal chnl num : %d\r\n", gstHbaAlgoChnlMap.uchNum);

    if(gstHbaAlgoChnlMap.uchNum > GH3X_GetHrAlgoSupportChnl())
    {
        GH3X_ALGO_LOG_PARAM("ERROR: hr algorithm legal chnl num is too big, check cfg !!! \r\n");
        return GX_ALGO_HBA_RWONG_INPUT;//while(1);
    }

    stHbCfg.mode = (hba_test_mode)gunHrParamsArr[0];
    stHbCfg.scence = (hba_scenes_e)gunHrParamsArr[1];
    stHbCfg.fs = fs;
    stHbCfg.valid_channel_num = gstHbaAlgoChnlMap.uchNum;
    stHbCfg.back_track_len = gunHrParamsArr[2];
    stHbCfg.hba_latest_output_time = gunHrParamsArr[3];
    stHbCfg.hba_earliest_output_time = gunHrParamsArr[4];
	
    GH3XSleepFlagSet((stHbCfg.scence == HBA_SCENES_SLEEP) ? 1 : 0);
    GH3X_ALGO_LOG_PARAM("[%s]:params = %u,%u,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld\r\n", __FUNCTION__,
                                    stHbCfg.mode,
                                    stHbCfg.scence,
                                    stHbCfg.fs,
                                    stHbCfg.valid_channel_num,
                                    stHbCfg.back_track_len,
                                    stHbCfg.hba_latest_output_time,
                                    stHbCfg.hba_earliest_output_time,
                                    stHbCfg.hba_lowerest_confidence,
                                    stHbCfg.hba_out_step_second,
                                    stHbCfg.hba_convergence_rate,
                                    stHbCfg.senseless_mode_step,
                                    stHbCfg.senseless_mode_duration);

    stAlgoRet = goodix_hba_init(&stHbCfg,sizeof(stHbCfg),(const char *)chVer);
    return stAlgoRet;
}

goodix_hba_ret goodix_hba_deinit_func(void)
{
    goodix_hba_deinit();
    return GX_ALGO_HBA_SUCCESS;
}


GS8 GH3XHrAlgoInit(const STFrameInfo * const pstFrameInfo)
{
    memset(&pstFrameInfo->pstAlgoData->uchFlag, 0, sizeof(GU8) + sizeof(GU8) + sizeof(GU32));

    GS8 chRet = GH3X_RET_GENERIC_ERROR;

    GH3X_ALGO_LOG_PARAM("%s\r\n", __FUNCTION__);
    if (goodix_hba_init_func(pstFrameInfo ->pstGh3xData->usSampleRate) == GX_ALGO_HBA_SUCCESS)        
    {
        
        chRet = GH3X_RET_OK;
        GH3X_ALGO_LOG_PARAM("[%s]GH3X_HbaInit success!!\r\n", __FUNCTION__);
    }
    else
    {
        GH3X_ALGO_LOG_PARAM("hba init error!\r\n");
    }
    pstFrameInfo->pstAlgoData->uchFlag = 0;
    pstFrameInfo->pstAlgoData->pnResults[0] = 0;
    return chRet;
}



GS8 GH3XHrAlgoDeinit(const STFrameInfo * const pstFrameInfo)
{
    GS8 chRet = GH3X_RET_GENERIC_ERROR;
    GH3X_ALGO_LOG_PARAM("%s\r\n", __FUNCTION__);
    if (goodix_hba_deinit_func() == GX_ALGO_HBA_SUCCESS)
    {
        chRet = GH3X_RET_OK;
    }
    else
    {
        GH3X_ALGO_LOG_PARAM("hba deinit error!\r\n");
    }
    return chRet;
}




//call algo 2-level interface

GS8 GH3XHrAlgoExe(const STFrameInfo * const pstFrameInfo)
{
    if(0 == pstFrameInfo )
    {
        return GH3X_RET_GENERIC_ERROR;
    }
    int32_t ppg_rawdata[3 * PPG_CHANNEL_NUM] = {0};
    int32_t cur_adj_flg[3 * PPG_CHANNEL_NUM] = {0};
    int32_t gain_adj_flg[3 * PPG_CHANNEL_NUM] = {0};
    int32_t enable_flg[3 * PPG_CHANNEL_NUM] = {0};
    goodix_hba_input_rawdata stRawdata = {0};
    stRawdata.ppg_rawdata = ppg_rawdata;
    stRawdata.cur_adj_flg = cur_adj_flg;
    stRawdata.gain_adj_flg = gain_adj_flg;
    stRawdata.enable_flg = enable_flg;

    pstFrameInfo->pstAlgoData->uchFlag = 0;
    goodix_hba_result stResult = {0};
    goodix_hba_ret emAlgoRet = GX_ALGO_HBA_SUCCESS;
    
    stRawdata.frameid = GH3X_GET_BYTE0_FROM_DWORD(*(pstFrameInfo->punFrameCnt));
    // stRawdata.acc_x = pstFrameInfo->pstGsData->pusData[0];
    // stRawdata.acc_y = pstFrameInfo->pstGsData->pusData[1];
    // stRawdata.acc_z = pstFrameInfo->pstGsData->pusData[2];
	GH3X_ALGO_LOG_PARAM("[%s]acc_x = %d,acc_y = %d,acc_z = %d\r\n",__FUNCTION__,\
                (int)stRawdata.acc_x,\
                (int)stRawdata.acc_y,\
                (int)stRawdata.acc_z);
    stRawdata.sleep_flg = GH3XSleepFlagGet();

    GU8 uchChMapCnt = 0;
    GU8 uchRawdataIndex = 0;
	GU8 uchTotalChannelNum = 0;
    GS8 chAlgoRet = 0;

    for (uchChMapCnt = 0; uchChMapCnt < 3 * PPG_CHANNEL_NUM; ++uchChMapCnt)
    {
        if (gstHbaAlgoChnlMap.uchAlgoChnlMap[uchChMapCnt + 4 * (uchChMapCnt / 4)] < 0xff && ((uchChMapCnt % 4) < gstHbaAlgoChnlMap.uchNum))
        {
            uchRawdataIndex = gstHbaAlgoChnlMap.uchAlgoChnlMap[uchChMapCnt + 4 * (uchChMapCnt / 4)];

            //make gain_adj_flg and cur_adj_flg to 0 for algo 1.0.5.0(alpha).
            stRawdata.gain_adj_flg[uchChMapCnt] = 0;// (pstFrameInfo->pstGh3xData->punAgcInfo[uchRawdataIndex]) & 0xF;
            stRawdata.cur_adj_flg[uchChMapCnt] = 0;//((pstFrameInfo->pstFlagInfo->punFlag[0] >> uchChMapCnt) & 0x01);
            stRawdata.enable_flg[uchChMapCnt]  = 1;
            uchTotalChannelNum++;
            stRawdata.ppg_rawdata[uchChMapCnt] = pstFrameInfo->pstGh3xData->punRawdata[uchRawdataIndex];
            GH3X_ALGO_LOG_PARAM("[%s]ppg_rawdata = %d,gain_adj_flg = %d,cur_adj_flg = %d\r\n",__FUNCTION__,\
                (int)stRawdata.ppg_rawdata[uchChMapCnt],\
                (int)stRawdata.gain_adj_flg[uchChMapCnt],\
                (int)stRawdata.cur_adj_flg[uchChMapCnt]);
        }
    }
    stRawdata.total_channel_num = uchTotalChannelNum;

    /* call algorithm, update result */
    emAlgoRet = goodix_hba_update(&stRawdata, &stResult);
    if (emAlgoRet == GX_ALGO_HBA_SUCCESS)
    {
        pstFrameInfo->pstAlgoData->uchFlag = (GU8)stResult.hba_out_flag;
        if (stResult.hba_out_flag == 1)
        {
#if (__USE_GOODIX_HRV_ALGORITHM__)
            void gh3x_set_hrv_hr(GU32 unHrvLastHrResult);
            gh3x_set_hrv_hr((GS32)stResult.hba_out);
#endif
            pstFrameInfo->pstAlgoData->pnResults[0] = (GS32)stResult.hba_out;
            pstFrameInfo->pstAlgoData->pnResults[1] = (GS32)stResult.valid_score;
            pstFrameInfo->pstAlgoData->pnResults[2] = GH3X_ALGORITHM_GF32_2_GS32(stResult.hba_snr);
            pstFrameInfo->pstAlgoData->pnResults[3] = (GS32)stResult.valid_level;
            pstFrameInfo->pstAlgoData->pnResults[4] = (GS32)stResult.hba_acc_info;
            pstFrameInfo->pstAlgoData->pnResults[5] = (GS32)stResult.hba_acc_scence;

            GH3X_ALGO_LOG_PARAM("[%s]hr = %d,UpdateFlag = %d\r\n",__FUNCTION__,\
                (int)pstFrameInfo->pstAlgoData->pnResults[0],(int)pstFrameInfo->pstAlgoData->uchFlag);

            pstFrameInfo->pstAlgoData->unResultBits = 0x3F;
            pstFrameInfo->pstAlgoData->uchResultNum = GH3X_BitCount(pstFrameInfo->pstAlgoData->unResultBits);
#if __GH3X_HR_OUTPUT_VALUE_STRATEGY_EN__
            GH3XHrOutputValueStrategyPro(pstFrameInfo->pstAlgoData,pstFrameInfo->punFrameCnt[0]);
#endif
            GH3X_HrAlgorithmResultReport(pstFrameInfo->pstAlgoData,pstFrameInfo->punFrameCnt[0]);
            
        }
    }
    else
    {
        chAlgoRet = GH3X_RET_RESOURCE_ERROR;
        GH3X_ALGO_LOG_PARAM("HrAlgoExe error! error code = 0x%x please feedback to GOODIX!\r\n", chAlgoRet);
    }
    return chAlgoRet;
}

#endif

