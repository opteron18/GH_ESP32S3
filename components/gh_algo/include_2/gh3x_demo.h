/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x_demo.h
 *
 * @brief   header file of gh3x driver lib demo
 *
 * @version GH(M)3X2X_DRV_LIB_DEMO_1.0
 *
 * @author  Gooidx Iot Team
 *
 */


#ifndef _GH3X_DEMO_H_
#define _GH3X_DEMO_H_

#include "gh3x_drv.h"
#include "gh_multi_sen_pro.h"


/**
 * @fn     int GH3X_DemoInit(void)
 *
 * @brief  Init GH3X_ module.
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
int GH3X_DemoInit(void);

/**
 * @fn     GU8 GH3X_DemoInterruptProcess(void)
 *
 * @brief  Interrupt process of GH3X_.
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_DemoInterruptProcess(void);

/**
 * @fn     void hal_gh3x_int_handler_call_back(void)
 * 
 * @brief  call back of gh3x int handler
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void hal_gh3x_int_handler_call_back(void);

/**
 * @fn     GS8 GH3X_DemoArrayCfgSwitch(GU8 uchArrayCfgIndex)
 *
 * @brief  array cfg switch (call by user)
 *
 * @attention  you should switch array cfg manually before calling GH3X_DemoStartSampling
 *
 * @param[in]   uchArrayCfgIndex    0: gh3x_reglist0    1: gh3x_reglist0 ...
 * @param[out]  None
 *
 * @return  GH3X_RET_OK:switch application mode success
 * @return  GH3X_RET_RESOURCE_ERROR:can't find corresponding reg config array
 */
GS8 GH3X_DemoArrayCfgSwitch(GU8 uchArrayCfgIndex);


/**
 * @fn     void GH3X_DemoStartSamplingWithCfgSwitch(GU32 unFuncMode, GU8 uchArrayCfgIndex)
 *
 * @brief  Start sampling of GH3X_
 *
 * @attention   None
 *
 * @param[in]   unFuncMode      function mode that will start
 *                              GH3X_FUNCTION_ADT/GH3X_FUNCTION_HR/GH3X_FUNCTION_SPO2/GH3X_FUNCTION_ECG and etc.
 * @param[in]   uchArrayCfgIndex      cfg id
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_DemoStartSamplingWithCfgSwitch(GU32 unFuncMode, GU8 uchArrayCfgIndex);

/**
 * @fn     void GH3X_DemoStartSampling()
 *
 * @brief  Start sampling of GH3X_
 *
 * @attention   None
 *
 * @param[in]   unFuncMode      function mode that will start
 *                              GH3X_FUNCTION_ADT/GH3X_FUNCTION_HR/GH3X_FUNCTION_SPO2/GH3X_FUNCTION_ECG and etc.
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_DemoStartSampling(GU32 unFuncMode);

/**
 * @fn     void GH3X_DemoStopSampling(GU32 unFuncMode)
 *
 * @brief  Stop sampling of GH3X_
 *
 * @attention   None
 *
 * @param[in]   unFuncMode      function mode that will start
 *                              GH3X_FUNCTION_ADT/GH3X_FUNCTION_HR/GH3X_FUNCTION_SPO2/GH3X_FUNCTION_ECG and etc.
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_DemoStopSampling(GU32 unFuncMode);

/**
 * @fn     void GH3X_DemoFunctionSampleRateSet(GU32 unFunctionID,  GU16 usSampleRate)
 *
 * @brief  
 *
 * @attention   None
 *
 * @param[in]   unFunctionID   such as: GH3X_FUNCTION_HR   or  GH3X_FUNCTION_HR|GH3X_FUNCTION_HRV
 * @param[in]   usSampleRate   only 25Hz times supported  (25/50/75...)
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_DemoFunctionSampleRateSet(GU32 unFunctionID,  GU16 usSampleRate);

/**
 * @fn      void GH3X_DemoFunctionChannelEnSet(GU32 unFunctionID,  GU32 unChnlEn)
 *
 * @brief  
 *
 * @attention   None
 *
 * @param[in]   unFunctionID   such as: GH3X_FUNCTION_HR   or  GH3X_FUNCTION_HR|GH3X_FUNCTION_HRV
 * @param[in]   unChnlEn   BitN = 0: channel N is disable        BitN = 1: channel N is enable
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_DemoFunctionChannelEnSet(GU32 unFunctionID,  GU32 unChnlEn);

void GhCommInit(void);

#endif /* _GH3X_DEMO_H_ */

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
