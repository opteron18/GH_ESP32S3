#include <stdio.h>
#include "gh3x_drv.h"
#include "gh3x_demo_algo_config.h"

#if (__GOODIX_ALGO_CALL_MODE__)

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
void GH3X_HrAlgorithmResultReport(STAlgoData * pstAlgoResult, GU32 lubFrameId);

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
void GH3X_Spo2AlgorithmResultReport(STAlgoData * pstAlgoResult, GU32 lubFrameId);

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
void GH3X_HrvAlgorithmResultReport(STAlgoData * pstAlgoResult, GU32 lubFrameId);

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
void GH3X_EcgAlgorithmResultReport(STAlgoData * pstAlgoResult, GU32 lubFrameId);

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
void GH3X_SoftAdtGreenAlgorithmResultReport(STAlgoData * pstAlgoResult, GU32 lubFrameId);

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
void GH3X_SoftAdtIrAlgorithmResultReport(STAlgoData * pstAlgoResult, GU32 lubFrameId);

#endif // end else #if (__DRIVER_LIB_MODE__==__DRV_LIB_WITHOUT_ALGO__)

