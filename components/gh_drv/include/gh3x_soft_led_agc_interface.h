/**
  ****************************************************************************************
  * @file    gh3x_soft_led_agc_interface.h
  * @author  GHealth Driver Team
  * @brief   
  ****************************************************************************************
  * @attention  
  #####Copyright (c) 2022 GOODIX
   All rights reserved.
  
  ****************************************************************************************
  */


#ifndef _GH3X_DRV_SOFT_LED_AGC_INTERFACE_H_
#define _GH3X_DRV_SOFT_LED_AGC_INTERFACE_H_
 
 
#include "gh3x_drv.h"

/**
 * @fn  void GH3X_LedAgcReset(void)
 *
 * @brief   reset agc parameter
 *
 * @attention    None
 *
 * @param[in]    None
 * @param[out]   None
 *
 * @return  None
 */
void GH3X_LedAgcReset(void);

/**
 * @fn     void GH3X_LedAgcProcess(GU8* puchReadFifoBuffer, GU16 usFifoLen)
 * 
 * @brief  led agc process function
 *
 * @attention   None
 *
 * @param[in]   puchReadFifoBuffer         pointer to read fifo data buffer
 * @param[in]   usFifoLen           read fifo data buffer length
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_LedAgcProcess(GU8* puchReadFifoBuffer, GU16 usFifoLen);

/**
 * @fn     void GH3X_LedAgcPramWrite(GU16 usRegVal, GU16 usRegPosi)
 *
 * @brief  write AGC parameter
 *
 * @attention   None
 *
 * @param[in]   usRegVal         reg value
 * @param[in]   uchRegPosi         reg posi
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_LedAgcPramWrite(GU16 usRegVal, GU16 usRegPosi);


/**
 * @fn     static void GH3X_LedAgcInit(void)
 * 
 * @brief  init led agc
 *
 * @attention   just use to show function support
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_LedAgcInit(void);

/**
 * @fn     void GH3X_LedAgc_Close(void)
 * 
 * @brief  close agc
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_LedAgc_Close(void);

/**
 * @fn     void GH3X_LedAgc_Open(void)
 * 
 * @brief  open agc
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_LedAgc_Open(void);

#endif  // _GH3X_DRV_SOFT_LED_AGC_H_

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
