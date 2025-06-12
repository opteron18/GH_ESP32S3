/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x_demo_algo_call.h
 *
 * @brief   gh3x algo call
 *
 * @version ref gh3x_demo_algo_call.h
 *
 */
#ifndef _GH3X_DEMO_ALGO_CALL_H_
#define _GH3X_DEMO_ALGO_CALL_H_

#include "gh3x_drv.h"

extern GCHAR* GH3X_GetAlgoCallDemoVersion(void);
extern void GH3X_AlgoVersion(GU8 uchFunctionID, GCHAR version[150]);
extern GS8 GH3X_AlgoInit(GU32 unFunctionID);
extern GS8 GH3X_AlgoDeinit(GU32 unFunctionID);
extern GS8 GH3X_AlgoCalculate(GU32 unFunctionID);

extern void GH3X_InitAlgoConfigParameters(void);
extern void GH3X_AlgoSensorEnable(GU8 uchAlgoGsensorEnable,GU8 uchAlgoCapEnable,GU8 uchAlgoTempEnable);
void GH3X_AlgoCallConfigInit(const STFrameInfo * const  pSTFrameInfo[], GU8 uchAlgoCfgIndex);
extern void GH3X_SoftAdtParaInit(void);
extern void GH3X_WriteAlgConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue);
GS8 *GSRAlgoVersion(void) ;
GU8 GH3XBiaCheckParam(void);
void GH3X_AdtAlgorithmResultReport(STAlgoData * pstAlgoResult, GU32 lubFrameId);

/**
 * @fn     GS8 GH3X_HbAlgorithmOutputTimeConfig(GS32 nOutputTime)
 *
 * @brief  Hb algorithm output time config
 *
 * @attention   None
 *
 * @param[in]   nOutputTime          valid 7~GS32_MAX
 * @param[out]  None
 *
 * @return  error code
 * @retval  #GH3X_RET_OK                      return successfully
 * @retval  #GH3X_RET_ALGO_NO_SUPPORTED       return algorithm no supported error
 */
extern GS8 GH3X_HbAlgorithmOutputTimeConfig(GS32 nLatestOutputTime, GS32 nEarliestOutputTime);

/**
 * @fn     GS8 GH3X_HbAlgorithmScenarioConfig(GU8 uchScenario)
 *
 * @brief  Hb algorithm scenario config
 *
 * @attention   None
 *
 * @param[in]  uchScenario     @ref EMHbaScenesConfigType
 *                             others: fixed 0(default) and return GH3X_RET_PARAMETER_ERROR
 * @param[out]  None
 *
 * @return  error code
 * @retval  #GH3X_RET_OK                      return successfully
 * @retval  #GH3X_RET_PARAMETER_ERROR         return paramters error
 * @retval  #GH3X_RET_ALGO_NO_SUPPORTED       return algorithm no supported error
 */
extern GS8 GH3X_HbAlgorithmScenarioConfig(GU8 uchScenario);

extern GU8 GH3X_TimestampSyncGetFrameDataFlag(void);
extern void GH3X_TimestampSyncAccInit(void);
extern void GH3X_TimestampSyncPpgInit(GU32 unFunctionID);
extern void GH3X_TimestampSyncSetPpgIntFlag(GU8 uchPpgIntFlag);
extern void GH3X_TimestampSyncFillAccSyncBuffer(GU32 unTimeStamp,
                                           GS16 usAccX,
                                           GS16 usAccY,
                                           GS16 usAccZ);
extern void GH3X_TimestampSyncFillPpgSyncBuffer(GU32 unTimeStamp,
                                           const STFrameInfo * const pstFrameInfo);

#endif /* _GH3X_DEMO_ALGO_CALL_H_ */

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
