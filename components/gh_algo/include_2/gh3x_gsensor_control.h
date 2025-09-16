/**
  ****************************************************************************************
  * @file    gh_gsensor_control.c
  * @author  GHealth Driver Team
  * @brief   
  ****************************************************************************************
  * @attention  
  #####Copyright (c) 2022 GOODIX
   All rights reserved.
  
  ****************************************************************************************
  */

#include "gh3x_drv.h"
#include "gh3x_demo_config.h"

/**
 * @fn     void GH3X_NormalizeGsensorSensitivity(GU16 * gsensor_buffer[], GU16 gsensor_point_num) 
 *
 * @brief  normalize gsensor sensitivity to  512LSB/g
 *
 * @attention   None
 *
 * @param[in]    gsensor_buffer          gsensor rawdata buffer
 * @param[in]   gsensor_point_num     gsensor rawdata point num
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_NormalizeGsensorSensitivity(GS16 gsensor_buffer[], GU16 gsensor_buffer_len);

