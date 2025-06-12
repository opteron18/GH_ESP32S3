#include <stdio.h>
#include "gh3x_demo_algo_call.h"
#include "gh3x_demo_algo_call_inner.h"
#include "gh3x_demo_algo_config.h"

#if (__USE_GOODIX_ADT_ALGORITHM__)

#define __HARD_ADT_WAER_ON_THRD__               8600000
#define __HARD_ADT_WAER_OFF_THRD__              8478608
#define __HARD_ADT_WAER_ON_DIFF_THRD__          100000
#define __HARD_ADT_WAER_BIG_DIFF__              30000
#define GH3X_MOVINF_AVA_WINDOW_SIZE             6

static EMWearDetectType g_emAlgoWearStatus = WEAR_DETECT_WEAR_ON;
static GU32 gpunMovingAvaFilterBuf[GH3X_MOVINF_AVA_WINDOW_SIZE];
static GU8  guchGH3XMovingAvaFilterPointCnt;



void GH3XMovingAvaFilterInit(void)
{
    guchGH3XMovingAvaFilterPointCnt = 0;
}

GU8 GH3XMovingAvaFilter(GU32 *Rawdata, GU8 uchBypass)
{
    GU32 unSum = 0;

    if(uchBypass)
    {
        return 1;
    }
    else
    {
        if(guchGH3XMovingAvaFilterPointCnt >= GH3X_MOVINF_AVA_WINDOW_SIZE)
        {
            for(GU8 uchCnt = 1; uchCnt < GH3X_MOVINF_AVA_WINDOW_SIZE; uchCnt ++)
            {
                gpunMovingAvaFilterBuf[uchCnt - 1] = gpunMovingAvaFilterBuf[uchCnt];
            }
            guchGH3XMovingAvaFilterPointCnt = GH3X_MOVINF_AVA_WINDOW_SIZE - 1;
        }
        gpunMovingAvaFilterBuf[guchGH3XMovingAvaFilterPointCnt] = Rawdata[0];
        guchGH3XMovingAvaFilterPointCnt ++;
            
        if(guchGH3XMovingAvaFilterPointCnt > 1)
        {
            for(GU8 uchCnt = 0; uchCnt < guchGH3XMovingAvaFilterPointCnt - 1; uchCnt ++)
            {
                unSum += gpunMovingAvaFilterBuf[uchCnt];
            }
            Rawdata[0] = unSum/(guchGH3XMovingAvaFilterPointCnt - 1);
        }
        return 1;
    }
}

#define ADT_CHECK_ALGO_VERSION "GX_HARDADT_SOFTCHECK_v0.1"

GU8* GH3XAdtVersion (void)
{
    return (GU8*)ADT_CHECK_ALGO_VERSION;
}

GS8  GH3XAdtAlgoInit(const STFrameInfo * const pstFrameInfo)
{
    memset(&pstFrameInfo->pstAlgoData->uchFlag, 0, sizeof(GU8) + sizeof(GU8) + sizeof(GU32));
    GH3XMovingAvaFilterInit();
    g_emAlgoWearStatus = WEAR_DETECT_WEAR_ON;
    return GH3X_RET_OK;
}

GS8 GH3XAdtAlgoDeinit(const STFrameInfo * const pstFrameInfo)
{
    GH3XMovingAvaFilterInit();
    g_emAlgoWearStatus = WEAR_DETECT_WEAR_ON;
    return GH3X_RET_OK;
}

GS8 GH3XAdtAlgoExe(const STFrameInfo * const pstFrameInfo)
{
    if (pstFrameInfo == GH3X_PTR_NULL)
    {
        return GH3X_RET_GENERIC_ERROR;
    }
    pstFrameInfo->pstAlgoData->uchFlag = 0;

    GU32 unCurrentFrameRawdata = 0;
    GS32 snDiffRawdata = 0;
    GU32 unRawDataBeforeFilter = 0;

    unCurrentFrameRawdata = pstFrameInfo->pstGh3xData->punRawdata[0];
    unRawDataBeforeFilter = pstFrameInfo->pstGh3xData->punRawdata[0];
    GH3X_ALGO_LOG_PARAM("[GH3XAdtAlgoExe]:punFrameRawdata = %d\r\n", unCurrentFrameRawdata);
    GH3XMovingAvaFilter(&unCurrentFrameRawdata, 0);
    GH3X_ALGO_LOG_PARAM("[GH3XAdtAlgoExe]:punFrameRawdata after filter = %d\r\n", unCurrentFrameRawdata);
    GH3X_ALGO_LOG_PARAM("[GH3XAdtAlgoExe]:start check...\r\n");

    snDiffRawdata = (GS32)unRawDataBeforeFilter - (GS32)unCurrentFrameRawdata;
    pstFrameInfo->pstAlgoData->pnResults[0] = ADT_ALGO_RESULT_DEFULT;

    if(WEAR_DETECT_WEAR_ON == g_emAlgoWearStatus)
    {
        if((unRawDataBeforeFilter > __HARD_ADT_WAER_ON_THRD__)&&((snDiffRawdata > (GS32)__HARD_ADT_WAER_ON_DIFF_THRD__)))
        {
            g_emAlgoWearStatus = WEAR_DETECT_WEAR_OFF;
            GH3X_ALGO_LOG_PARAM("[GH3XAdtAlgoExe]:wear on event, Rawdata = %d, unRawdataDiff = %d\r\n",unCurrentFrameRawdata,snDiffRawdata);
            pstFrameInfo->pstAlgoData->uchFlag = 1;
            pstFrameInfo->pstAlgoData->pnResults[0] = ADT_ALGO_RESULT_WAER_ON;
        }
        else
        {
            if(unRawDataBeforeFilter > __HARD_ADT_WAER_ON_THRD__)
            {
                GH3X_ALGO_LOG_PARAM("[GH3XAdtAlgoExe]:wear on condition is not match ! r = %d, r_diff = %d\r\n",unCurrentFrameRawdata,snDiffRawdata);
            }
        }
        if(snDiffRawdata > __HARD_ADT_WAER_BIG_DIFF__)
        {
            //pstFrameInfo->pstAlgoData->uchFlag = 1;
            //pstFrameInfo->pstAlgoData->pnResults[0] = ADT_ALGO_RESULT_DIFF_BIG;
            GH3X_ALGO_LOG_PARAM("[GH3XAdtAlgoExe]:ppg 5Hz diff big event, Rawdata = %d, unRawdataDiff = %d\r\n",unCurrentFrameRawdata,snDiffRawdata);
        }
    }
    else if(WEAR_DETECT_WEAR_OFF == g_emAlgoWearStatus)
    {
        if(unRawDataBeforeFilter < __HARD_ADT_WAER_OFF_THRD__)
        {
            g_emAlgoWearStatus = WEAR_DETECT_WEAR_ON;
            GH3X_ALGO_LOG_PARAM("[GH3XAdtAlgoExe]:wear off event, Rawdata = %d, unRawdataDiff = %d\r\n",unCurrentFrameRawdata,snDiffRawdata);
            pstFrameInfo->pstAlgoData->uchFlag = 1;
            pstFrameInfo->pstAlgoData->pnResults[0] = ADT_ALGO_RESULT_WAER_OFF;
        }
    }

    pstFrameInfo->pstAlgoData->unResultBits = 0x7;
    pstFrameInfo->pstAlgoData->uchResultNum = GH3X_BitCount(pstFrameInfo->pstAlgoData->unResultBits);
    pstFrameInfo->pstAlgoData->pnResults[1] = snDiffRawdata;
    pstFrameInfo->pstAlgoData->pnResults[2] = pstFrameInfo->punFrameCnt[0];
    GH3X_AdtAlgorithmResultReport(pstFrameInfo->pstAlgoData, pstFrameInfo->punFrameCnt[0]);

    return GH3X_RET_OK;
}

#endif

