#include <stdio.h>
#include "gh3x_demo_algo_memory.h"
#include "gh3x_demo_algo_call.h"
#include "gh3x_demo_algo_config.h"
#include "gh3x_demo_algo_call_inner.h"
#include "gh3x_demo_algo_call_version.h"
#include "goodix_log.h"
#include "goodix_mem.h"
#include "goodix_type.h"
#include "goodix_hba.h"
#include "goodix_ecg.h"
//#include "goodix_af.h"
//#include "goodix_hsm.h"
//#include "goodix_resp.h"
#include "goodix_sys_hrv.h"
#include "goodix_spo2.h"
#include "goodix_nadt.h"
//#include "goodix_algo_bt.h"
#include "goodix_sys_bia.h"

#if (__GOODIX_ALGO_CALL_MODE__)
extern const STInitConfig gstGoodixAlgoCfgListArr[__GOODIX_ALGO_CFG_LIST_MAX_NUM__];
STFrameInfo ** gpstGH3XAlgoFrameInfo = GH3X_PTR_NULL;
static GU32 gunAlgoFuncStartedBitmap = 0;
static GU8 guchAlgInitFlag[GH3X_FUNC_OFFSET_MAX] = {0};
static const pfnAlgorithmCallFunc gpfnAlgorithmCallFunc[GH3X_FUNC_OFFSET_MAX][GH3X_ALGO_FUNCTION_NUM_MAX] = {
//adt
#if __USE_GOODIX_ADT_ALGORITHM__
    { GH3XAdtAlgoInit, GH3XAdtAlgoExe, GH3XAdtAlgoDeinit },
#else
    { 0, 0, 0 },
#endif
//hr
#if __USE_GOODIX_HR_ALGORITHM__
    { GH3XHrAlgoInit, GH3XHrAlgoExe, GH3XHrAlgoDeinit },
#else
    { 0, 0, 0 },
#endif
//hrv
#if __USE_GOODIX_HRV_ALGORITHM__
    { GH3XHrvAlgoInit, GH3XHrvAlgoExe, GH3XHrvAlgoDeinit },
#else
    { 0, 0, 0 },
#endif
//hsm
    { 0, 0, 0 },
//fpbp
//pwa
    { 0, 0, 0 },
    { 0, 0, 0 },
//spo2
#if __USE_GOODIX_SPO2_ALGORITHM__
    { GH3XSpo2AlgoInit, GH3XSpo2AlgoExe, GH3XSpo2AlgoDeinit },
#else
    { 0, 0, 0 },
#endif
//ecg
#if __USE_GOODIX_ECG_ALGORITHM__
    { GH3XEcgAlgoInit, GH3XEcgAlgoExe, GH3XEcgAlgoDeinit },
#else
    { 0, 0, 0 },
#endif
//pwtt
    { 0, 0, 0 },
//soft adt green
#if __USE_GOODIX_SOFT_ADT_ALGORITHM__
    { GH3XSoftAdtAlgoInit, GH3XSoftAdtAlgoExe, GH3XSoftAdtAlgoDeinit },
#else
    { 0, 0, 0 },
#endif
//bt
    { 0, 0, 0 },
//resp
    { 0, 0, 0 },
//af
    { 0, 0, 0 },
//test
    { 0, 0, 0 },
    { 0, 0, 0 },
//soft adt ir
#if __USE_GOODIX_SOFT_ADT_ALGORITHM__
    { GH3XSoftAdtAlgoInit, GH3XSoftAdtAlgoExe, GH3XSoftAdtAlgoDeinit },
#else
    { 0, 0, 0 },
#endif
//bia
#if __USE_GOODIX_BIA_ALGORITHM__
    { GH3XBiaAlgoInit, GH3XBiaAlgoExe, GH3XBiaAlgoDeinit },
#else
    { 0, 0, 0 },
#endif
//GSR
#if __USE_GOODIX_GSR_ALGORITHM__
    { GH3XGSRAlgoInit, GH3XGSRAlgoExe, GH3XGSRAlgoDeinit },
#else
    { 0, 0, 0 },
#endif
};

GCHAR* GH3X_GetAlgoCallDemoVersion(void)
{
    return GOODIX_ALGO_CALL_VERSION_STRING;
}

void GH3X_RegisterFuncInfoStruct(const STFrameInfo * const  pSTFrameInfo[])
{
    gpstGH3XAlgoFrameInfo = (STFrameInfo **)pSTFrameInfo;
}

void GH3X_AlgoCallConfigInit(const STFrameInfo * const  pSTFrameInfo[], GU8 uchAlgoCfgIndex)
{
    if (pSTFrameInfo != GH3X_PTR_NULL)
    {
        gpstGH3XAlgoFrameInfo = (STFrameInfo **)pSTFrameInfo;
    }
    GH3X_AlgoMemConfig(GH3X_ALGORITHMS_MEMORY_SIZE_FINAL);
    GH3X_ALGO_LOG_PARAM("Algorithm mem size = %d\r\n",(int)GH3X_ALGORITHMS_MEMORY_SIZE_FINAL);
}

void GH3X_AlgoVersion(GU8 uchFunctionID, GCHAR version[150])
{
    if (version == 0)
    {
        GH3X_ALGO_LOG_PARAM("[%s]:version buffer is NULL!!!\r\n", __FUNCTION__);
        return;
    }
    GCHAR chNoVersion[10] = "no_ver";
    switch (uchFunctionID)
    {
#if __USE_GOODIX_HR_ALGORITHM__
    case GH3X_FUNC_OFFSET_HR:
        goodix_hba_version((uint8_t*)version);
        break;
#endif
#if __USE_GOODIX_HRV_ALGORITHM__
    case GH3X_FUNC_OFFSET_HRV:
        GH3X_Memcpy(version, goodix_hrv_version(), strlen((const GCHAR *)goodix_hrv_version()) + 1);
        break;
#endif
#if __USE_GOODIX_SPO2_ALGORITHM__
    case GH3X_FUNC_OFFSET_SPO2:
        goodix_spo2_version((uint8_t*)version);
        break;
#endif
#if __USE_GOODIX_ECG_ALGORITHM__
    case GH3X_FUNC_OFFSET_ECG:
        goodix_ecg_version((uint8_t*)version);
        break;
#endif

#if __USE_GOODIX_SOFT_ADT_ALGORITHM__
    case GH3X_FUNC_OFFSET_SOFT_ADT_GREEN:
    case GH3X_FUNC_OFFSET_SOFT_ADT_IR:
        GetNadtVersion((uint8_t *)version);
        break;
#endif
#if __USE_GOODIX_BP_ALGORITHM__
    case GH3X_FUNC_OFFSET_PWA:
    case GH3X_FUNC_OFFSET_FPBP:
        //GH3X_Memcpy(version, GoodixBpAlgoVersion(), strlen((const GCHAR *)GoodixBpAlgoVersion()) + 1);
        break;
#endif
#if __USE_GOODIX_BIA_ALGORITHM__
    case GH3X_FUNC_OFFSET_BIA:
        GH3X_Memcpy(version, BIAAlgoVersion(), strlen((const GCHAR *)BIAAlgoVersion()) + 1);
        break;
#endif
#if __USE_GOODIX_GSR_ALGORITHM__
    case GH3X_FUNC_OFFSET_GSR:
        GH3X_Memcpy(version, GSRAlgoVersion(), strlen((const GCHAR *)GSRAlgoVersion()) + 1);
        break;
#endif
    default:
        GH3X_Memcpy(version, (GCHAR *)chNoVersion,\
        strlen((const GCHAR *)chNoVersion) + 1);
        break;
    }
    GH3X_ALGO_LOG_PARAM("version = %s\n", version);
}

GS8 GH3X_AlgoInit(GU32 unFunctionID)
{
    GU8 chRet = GH3X_RET_OK;

    /* set started bit */
    gunAlgoFuncStartedBitmap |= unFunctionID;

    /* init mem pool */
    if (GH3X_GetAlgoMemStatus() != ALGO_MEM_INIT_OK)
    {
        return -1;
    }

    for(GU8 uchFunCnt = 0; uchFunCnt < GH3X_FUNC_OFFSET_MAX; uchFunCnt ++)
    {
        if (((1<<uchFunCnt) & unFunctionID) == (1<<uchFunCnt))
        {
            /* int algo */
            const STFrameInfo * const pstFrameInfo = gpstGH3XAlgoFrameInfo[uchFunCnt];
            if (1 == guchAlgInitFlag[uchFunCnt])
            {
                GH3X_ALGO_LOG_PARAM("[%s]:Function(function id = %d) already init!\r\n", __FUNCTION__, (int)uchFunCnt);
                GH3X_LOG_RECORD("[%s]:Function(function id = %d) already init!\r\n", __FUNCTION__, (int)uchFunCnt);
            }
            else
            {
                if (gpfnAlgorithmCallFunc[uchFunCnt][GH3X_ALGO_INIT_FUNCTION] != GH3X_PTR_NULL)
                {
                    chRet = gpfnAlgorithmCallFunc[uchFunCnt][GH3X_ALGO_INIT_FUNCTION](pstFrameInfo);
                    if (chRet != GH3X_RET_OK)
                    {
                        GH3X_ALGO_LOG_PARAM("[%s]:Function(function id = %d) init fail! errcode = %d\r\n", __FUNCTION__, (int)uchFunCnt, (int)chRet);
                        GH3X_LOG_RECORD("[%s]:Function(function id = %d) init fail! errcode = %d\r\n", __FUNCTION__, (int)uchFunCnt, (int)chRet);
                    }
                    else
                    {
                        GH3X_ALGO_LOG_PARAM("[%s]:Function(function id = %d) init success!\r\n", __FUNCTION__, (int)uchFunCnt);
                        GH3X_LOG_RECORD("[%s]:Function(function id = %d) init success!\r\n", __FUNCTION__, (int)uchFunCnt);
                        guchAlgInitFlag[uchFunCnt] = 1;
                    }
                }
                else
                {
                    GH3X_ALGO_LOG_PARAM("[%s]:Function(function id = %d) init func null!\r\n", __FUNCTION__, (int)uchFunCnt);
                }
                GH3XOutputValueStrategyInit(unFunctionID);
            }
            GH3X_RET_ERROR_CHECK(chRet);
        }
    }
    return chRet;
}

GS8 GH3X_AlgoDeinit(GU32 unFunctionID)
{
    GS32 chRet = GH3X_RET_OK;

    /* clear started bit */
    gunAlgoFuncStartedBitmap &= (~unFunctionID);

    /* deint algo */
    for(GU8 uchFunCnt = 0; uchFunCnt < GH3X_FUNC_OFFSET_MAX; uchFunCnt ++)
    {
        if (((1<<uchFunCnt) & unFunctionID) == (1<<uchFunCnt))
        {
            /* deint algo */
            const STFrameInfo * const pstFrameInfo = gpstGH3XAlgoFrameInfo[uchFunCnt];
            if (0 == guchAlgInitFlag[uchFunCnt])
            {
                GH3X_ALGO_LOG_PARAM("[%s]:Function(function id = %d) didn't init!\r\n", __FUNCTION__, (int)uchFunCnt);
                GH3X_LOG_RECORD("[%s]:Function(function id = %d) didn't init!\r\n", __FUNCTION__, (int)uchFunCnt);
            }
            else
            {
                if (gpfnAlgorithmCallFunc[uchFunCnt][GH3X_ALGO_DEINIT_FUNCTION] != GH3X_PTR_NULL)
                {
                    chRet = gpfnAlgorithmCallFunc[uchFunCnt][GH3X_ALGO_DEINIT_FUNCTION](pstFrameInfo);
                    if (chRet != GH3X_RET_OK)
                    {
                        GH3X_ALGO_LOG_PARAM("[%s]:Function(function id = %d) deinit fail! errcode = %d\r\n", __FUNCTION__, (int)uchFunCnt, (int)chRet);
                        GH3X_LOG_RECORD("[%s]:Function(function id = %d) deinit fail! errcode = %d\r\n", __FUNCTION__, (int)uchFunCnt, (int)chRet);
                    }
                    else
                    {
                        GH3X_ALGO_LOG_PARAM("[%s]:Function(function id = %d) deinit success!\r\n", __FUNCTION__, (int)uchFunCnt);
                        GH3X_LOG_RECORD("[%s]:Function(function id = %d) deinit success!\r\n", __FUNCTION__, (int)uchFunCnt);
                        guchAlgInitFlag[uchFunCnt] = 0;
                    }
                }
                else
                {
                    GH3X_ALGO_LOG_PARAM("[%s]:Function(function id = %d) deinit func null!\r\n", __FUNCTION__, (int)uchFunCnt);
                    GH3X_LOG_RECORD("[%s]:Function(function id = %d) deinit func null!\r\n", __FUNCTION__, (int)uchFunCnt);
                }
            }
            GH3X_RET_ERROR_CHECK(chRet);
        }
    }
    return chRet;
}

GS8 GH3X_AlgoCalculate(GU32 unFunctionID)
{
    if ((gunAlgoFuncStartedBitmap & unFunctionID) == 0)
    {
        GH3X_AlgoInit(unFunctionID);//return GH3X_RET_OK;
    }

    GS32 chRet = GH3X_RET_GENERIC_ERROR;

    for(GU8 uchFunCnt = 0; uchFunCnt < GH3X_FUNC_OFFSET_MAX; uchFunCnt ++)
    {
        if (((1<<uchFunCnt) & unFunctionID) == (1<<uchFunCnt))
        {
            /* calc algo */
            if (GH3X_CHECK_BIT_SET(gunAlgoFuncStartedBitmap, unFunctionID))
            {
                const STFrameInfo * const pstFrameInfo = gpstGH3XAlgoFrameInfo[uchFunCnt];
                if(GH3XSleepFlagGet())
                {
                    pstFrameInfo->pstFlagInfo->punFlag[2] |= ((GU32)1)<<3;
                }
                if (0 == guchAlgInitFlag[uchFunCnt])
                {
                    //GH3X_ALGO_LOG_PARAM("Function(funcition id = %d) didn't init!\r\n",(int)uchFunCnt);
                }
                else
                {
                    if (gpfnAlgorithmCallFunc[uchFunCnt][GH3X_ALGO_CALCULATE_FUNCTION] != GH3X_PTR_NULL)
                    {
                        chRet = gpfnAlgorithmCallFunc[uchFunCnt][GH3X_ALGO_CALCULATE_FUNCTION](pstFrameInfo);
                    }
                }
            }
        }
        else
        {
            chRet = GH3X_RET_OK;
        }
    }
    return chRet;
}

#endif

