#include <stdio.h>
#include "gh3x_demo_common.h"
#include "gh3x_demo_algo_call_inner.h"
#include "gh3x_demo_algo_call.h"
#include "gh3x_demo_algo_config.h"
#include "gh3x_demo_algo_hook.h"
#include "gh3x_demo_inner.h"

#if (__GOODIX_ALGO_CALL_MODE__)

/**
 * @fn     void GH3X_AlgoLog(char *logstring)
 * 
 * @brief  for debug version, log
 *
 * @attention   this function must define that use debug version lib
 *
 * @param[in]   logstring      pointer to log string
 * @param[out]  None
 *
 * @return  None
 */

void GH3X_AlgoLog(GCHAR *log_string)
{
    GOODIX_PLATFROM_LOG_ENTITY();
}

/**
 * @fn     void GH3X_AdtAlgorithmResultReport(STHbAlgoResult stHbAlgoRes[], GU16 pusAlgoResIndexArr[], usAlgoResCnt)
 *
 * @brief  This function will be called after calculate hb algorithm.
 *
 * @attention   None
 *
 * @param[in]   stHbAlgoRes             hb algorithm result array
 * @param[in]   pusAlgoResIndexArr      frame index of every algo result
 * @param[in]   usAlgoResCnt            number of algo result
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_AdtAlgorithmResultReport(STAlgoData * pstAlgoResult, GU32 lubFrameId)
{
#if (__USE_GOODIX_ADT_ALGORITHM__)
    if (pstAlgoResult->uchFlag)
    {
        if (pstAlgoResult->pnResults[0] == 1)
        {
            GH3X_ClearSoftEvent(GH3X_SOFT_EVENT_WEAR_OFF_ALGO_ADT);
            GH3X_SetSoftEvent(GH3X_SOFT_EVENT_WEAR_ON_ALGO_ADT);
            GH3X_ALGO_LOG_PARAM("[%s]:GH3X_SOFT_EVENT_WEAR_ON_ALGO_ADT\r\n", __FUNCTION__);

            /* code implement by user */
        }
        else if (pstAlgoResult->pnResults[0] == 2)
        {
            GH3X_ClearSoftEvent(GH3X_SOFT_EVENT_WEAR_ON_ALGO_ADT);
            GH3X_SetSoftEvent(GH3X_SOFT_EVENT_WEAR_OFF_ALGO_ADT);
            GH3X_ALGO_LOG_PARAM("[%s]:GH3X_SOFT_EVENT_WEAR_OFF_ALGO_ADT\r\n", __FUNCTION__);

            /* code implement by user */
        }
    }
#endif
}

/**
 * @fn     void GH3X_HrAlgorithmResultReport(STHbAlgoResult stHbAlgoRes[], GU16 pusAlgoResIndexArr[], usAlgoResCnt)
 *
 * @brief  This function will be called after calculate hb algorithm.
 *
 * @attention   None
 *
 * @param[in]   stHbAlgoRes             hb algorithm result array
 * @param[in]   pusAlgoResIndexArr      frame index of every algo result
 * @param[in]   usAlgoResCnt            number of algo result
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_HrAlgorithmResultReport(STAlgoData * pstAlgoResult, GU32 lubFrameId)
{
#if (__USE_GOODIX_HR_ALGORITHM__)
    /* code implement by user */
    GOODIX_PLATFROM_HR_RESULT_REPORT_ENTITY();
#endif
}

/**
 * @fn     void GH3X_Spo2AlgorithmResultReport(STAlgoData * pstAlgoResult)
 *
 *
 * @brief  This function will be called after calculate spo2 algorithm.
 *
 * @attention   None
 *
 * @param[in]   stSpo2AlgoRes           spo2 algorithm result array
 * @param[in]   pusAlgoResIndexArr      frame index of every algo result
 * @param[in]   usAlgoResCnt            number of algo result
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_Spo2AlgorithmResultReport(STAlgoData * pstAlgoResult, GU32 lubFrameId)
{
#if (__USE_GOODIX_SPO2_ALGORITHM__)

    /* code implement by user */
    GOODIX_PLATFROM_SPO2_RESULT_REPORT_ENTITY();
#endif
}

/**
 * @fn     void GH3X_HrvAlgorithmResultReport(STHrvAlgoResult stHrvAlgoRes[], GU16 pusAlgoResIndexArr[], usAlgoResCnt)
 *
 * @brief  This function will be called after calculate hrv algorithm.
 *
 * @attention   None
 *
 * @param[in]   stHrvAlgoRes            hrv algorithm result array
 * @param[in]   pusAlgoResIndexArr      frame index of every algo result
 * @param[in]   usAlgoResCnt            number of algo result
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_HrvAlgorithmResultReport(STAlgoData * pstAlgoResult, GU32 lubFrameId)
{
#if (__USE_GOODIX_HRV_ALGORITHM__)
    /* code implement by user */
    //SLAVER_LOG("snHrvOut0=%d,snHrvOut1=%d,snHrvOut2=%d,snHrvOut3=%d,snHrvNum=%d,snHrvConfi=%d\r\n",
    //          stHrvAlgoRes[0].snHrvOut0,stHrvAlgoRes[0].snHrvOut1,stHrvAlgoRes[0].snHrvOut2,stHrvAlgoRes[0].snHrvOut3,stHrvAlgoRes[0].snHrvNum,stHrvAlgoRes[0].snHrvNum);
//    GOODIX_PLATFROM_HRV_RESULT_REPORT_ENTITY();
#endif
}

/**
 * @fn     void GH3X_EcgAlgorithmResultReport(STEcgAlgoResult stEcgAlgoRes[], GU16 pusAlgoResIndexArr[], usAlgoResCnt)
 *
 * @brief  This function will be called after calculate ecg algorithm.
 *
 * @attention   None
 *
 * @param[in]   stEcgAlgoRes            ecg algorithm result array
 * @param[in]   pusAlgoResIndexArr      frame index of every algo result
 * @param[in]   usAlgoResCnt            number of algo result
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_EcgAlgorithmResultReport(STAlgoData * pstAlgoResult, GU32 lubFrameId)
{
#if (__USE_GOODIX_ECG_ALGORITHM__)
    /* code implement by user */
    GOODIX_PLATFROM_ECG_RESULT_REPORT_ENTITY();
#endif
}

GU8 g_SoftAdtWearState = GH3X_SOFT_ALGO_ADT_DEFAULT; // 0:default 1: wear on  1: wear off
/**
 * @fn     void GH3X_SoftAdtGreenAlgorithmResultReport(STAlgoData * pstAlgoResult, GU32 lubFrameId)
 *
 * @brief  This function will be called after calculate ecg algorithm.
 *
 * @attention   None
 *
 * @param[in]   stEcgAlgoRes            ecg algorithm result array
 * @param[in]   pusAlgoResIndexArr      frame index of every algo result
 * @param[in]   usAlgoResCnt            number of algo result
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_SoftAdtGreenAlgorithmResultReport(STAlgoData * pstAlgoResult, GU32 lubFrameId)
{
#if (__USE_GOODIX_SOFT_ADT_ALGORITHM__)
    GH3X_ALGO_LOG_PARAM("[%s]:result = %d,%d\r\n", __FUNCTION__, pstAlgoResult->pnResults[0], pstAlgoResult->pnResults[1]);
    //live object
    if (pstAlgoResult->pnResults[0] == 0x1 && g_SoftAdtWearState != GH3X_SOFT_ALGO_ADT_WEAR_ON)
    {
       GH3X_ALGO_LOG_PARAM("[%s]:get algo wear on event\r\n", __FUNCTION__);
       GH3X_ClearSoftEvent(GH3X_SOFT_EVENT_WEAR_OFF);
       GH3X_SetSoftEvent(GH3X_SOFT_EVENT_WEAR_ON);
       g_SoftAdtWearState = GH3X_SOFT_ALGO_ADT_WEAR_ON;
        /* code implement by user */
    }
    //non live object
    else if (pstAlgoResult->pnResults[0] & 0x2 && g_SoftAdtWearState != GH3X_SOFT_ALGO_ADT_WEAR_OFF)
    {
		GH3X_ALGO_LOG_PARAM("[%s]:get algo wear off event\r\n", __FUNCTION__);
		GH3X_ClearSoftEvent(GH3X_SOFT_EVENT_WEAR_ON);
        GH3X_SetSoftEvent(GH3X_SOFT_EVENT_WEAR_OFF);
        g_SoftAdtWearState = GH3X_SOFT_ALGO_ADT_WEAR_OFF;
        /* code implement by user */
    }
#endif
}

/**
 * @fn     void GH3X_SoftAdtIrAlgorithmResultReport(STAlgoData * pstAlgoResult, GU32 lubFrameId)
 *
 * @brief  This function will be called after calculate ecg algorithm.
 *
 * @attention   None
 *
 * @param[in]   stEcgAlgoRes            ecg algorithm result array
 * @param[in]   pusAlgoResIndexArr      frame index of every algo result
 * @param[in]   usAlgoResCnt            number of algo result
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_SoftAdtIrAlgorithmResultReport(STAlgoData * pstAlgoResult, GU32 lubFrameId)
{
#if (__USE_GOODIX_SOFT_ADT_ALGORITHM__)
    GH3X_ALGO_LOG_PARAM("[%s]:result = %d,%d\r\n", __FUNCTION__, pstAlgoResult->pnResults[0], pstAlgoResult->pnResults[1]);
    //live object
    if (pstAlgoResult->pnResults[0] == 0x1 && g_SoftAdtWearState != GH3X_SOFT_ALGO_ADT_WEAR_ON)
    {
		GH3X_ALGO_LOG_PARAM("[%s]:get algo wear on event\r\n", __FUNCTION__);
        //GH3XDemoStopSampling(GH3X_FUNCTION_SOFT_ADT_IR);
        //GH3XDemoStartSampling(GH3X_FUNCTION_SOFT_ADT_GREEN|GH3X_FUNCTION_HR);
		GH3X_ClearSoftEvent(GH3X_SOFT_EVENT_WEAR_OFF);
        GH3X_SetSoftEvent(GH3X_SOFT_EVENT_WEAR_ON);
        g_SoftAdtWearState = GH3X_SOFT_ALGO_ADT_WEAR_ON;
        /* code implement by user */
    }
    //non live object
    else if (pstAlgoResult->pnResults[0] & 0x2 && g_SoftAdtWearState != GH3X_SOFT_ALGO_ADT_WEAR_OFF)
    {
		GH3X_ALGO_LOG_PARAM("[%s]:get algo wear off event\r\n", __FUNCTION__);
		GH3X_ClearSoftEvent(GH3X_SOFT_EVENT_WEAR_ON);
        GH3X_SetSoftEvent(GH3X_SOFT_EVENT_WEAR_OFF);
        g_SoftAdtWearState = GH3X_SOFT_ALGO_ADT_WEAR_OFF;
        /* code implement by user */
    }
#endif
}

#endif // end else #if (__DRIVER_LIB_MODE__==__DRV_LIB_WITHOUT_ALGO__)

