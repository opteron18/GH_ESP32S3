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
#include "gh3x_demo_inner.h"
#include "gh3x_gsensor_control.h"

#if (__GSENSOR_ENABLE__)
/**
 * @fn     void GH3X_NormalizeGsensorSensitivity(STGsensorRawdata gsensor_buffer[], GU16 gsensor_point_num) 
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
void GH3X_NormalizeGsensorSensitivity(GS16 gsensor_buffer[], GU16 gsensor_buffer_len)
{
    GU8 uchRightShift;
    uchRightShift = __GS_SENSITIVITY_CONFIG__;
    
    GU8 uchStep = 3 + 3 * __GS_GYRO_ENABLE__;
    for(GU16 uchCnt = 0; uchCnt < gsensor_buffer_len; uchCnt += uchStep)
    {
        gsensor_buffer[uchCnt] >>= uchRightShift;
        gsensor_buffer[uchCnt + 1] >>= uchRightShift;
        gsensor_buffer[uchCnt + 2] >>= uchRightShift;
    }
}



#endif
