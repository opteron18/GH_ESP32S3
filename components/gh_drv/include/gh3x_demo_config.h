/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x_demo_config.h
 *
 * @brief   gh3x driver lib demo configuration file
 *
 * @author  Gooidx Iot Team
 *
 */
#ifndef _GH3X_DEMO_CONFIG_H_
#define _GH3X_DEMO_CONFIG_H_
#include "gh3x_drv.h"
#include "stdio.h"


/* bsp config */
/**< interface config __GH3X_INTERFACE_I2C__ /__GH3X_INTERFACE_SPI__ */
#define __GH3X_INTERFACE__                          (__GH3X_INTERFACE_SPI__)
#if (__GH3X_INTERFACE__ == __GH3X_INTERFACE_SPI__)
    #define __GH3X_SPI_TYPE__                       (__GH3X_SPI_TYPE_SOFTWARE_CS__)
#endif
#if (__GH3X_INTERFACE__ == __GH3X_INTERFACE_I2C__)
    #define __GH3X_I2C_DEVICE_ID__                  (GH3X_I2C_ID_SEL_1L0L)
#endif
#define __SUPPORT_HARD_RESET_CONFIG__           (1)                                         /**< support hard reset config */
#define __INTERRUPT_PROCESS_MODE__              (__NORMAL_INT_PROCESS_MODE__)               /**< 2:polling+int 1:use polling 0:use interrupt */

/* gsensor set */
#define __GSENSOR_ENABLE__                      (1)                                         /*Gsensor enable*/
#if (__GSENSOR_ENABLE__)
    #define __GS_SENSITIVITY_CONFIG__               (GSENSOR_SENSITIVITY_512_COUNTS_PER_G)  /**< gsensor sensitivity config */
    #define __GS_GYRO_ENABLE__                      (0)                                     /*Gyro enable*/
#else
    #define __GS_GYRO_ENABLE__                      (0)        /*Gyro enable*/
#endif
#if (__GH3X_INTERFACE__ == __GH3X_INTERFACE_I2C__)
    #define __FIFO_MONITOR_EN__                     (0)
#else
    #define __FIFO_MONITOR_EN__                     (0)
#endif
/* app config */
#define __DRIVER_LIB_MODE__                     (__DRV_LIB_WITH_ALGO__)

/*sofe agc*/
#define __SUPPORT_SOFT_AGC_CONFIG__             (1)    /**< support soft agc config */
#define GH3X_NEW_AGC_SLOT_NUM_LIMIT         (16)
#define GH3X_NEW_AGC_SUB_CHNL_NUM_LIMIT     ((GH3X_NEW_AGC_SLOT_NUM_LIMIT)*3)
#define __GH3X_AGC_GENERAL_TRIG_H_THD__  ((8388608 + 8388608*0.85f))
#define __GH3X_AGC_GENERAL_TRIG_L_THD__  ((8388608 + 8388608*0.15f))
#define __GH3X_AGC_GENERAL_IDEAL_VALUE__ ((8388608 + 8388608*0.56f))
#define __GH3X_AGC_SPO2_TRIG_H_THD__  ((8388608 + 8388608*0.9f))
#define __GH3X_AGC_SPO2_TRIG_L_THD__  ((8388608 + 8388608*0.5f))
#define __GH3X_AGC_SPO2_IDEAL_VALUE__  ((8388608 + 8388608*0.7f))
#define __GH3X2X_AGC_CONVERGENCE_LEVEL__  (8)


/* config about mult-sensor event process*/
#define __GH_MULTI_SENSOR_EVENT_PRO_CONIG__              (0)        /**0: no support multi-sensor event process  1: support multi-sensor event process*/
#define __GH_MULTI_SENSOR_TIMER_PERIOD_MS__              (1000)     /** period of multi-sensor timer, unit: ms*/

/* soft adt */
#define __SUPPORT_HARD_ADT_CONFIG__             (1)    /**< support hard adt config */
#define __GSENSOR_MOVE_THRESHOLD__              (65)   /**< DIFF of sqrt(x^2+y^2+z^2) */
#define __GSENSOR_MOVE_CNT_THRESHOLD__          (1)    /**< (recomended value = 1) more than how many times of movement can be judged as effective moveing*/
#define __GSENSOR_NOT_MOVE_CNT_THRESHOLD__      (150)  /**< (recommended value = 1.5 * sample rate of g-sensor ) more than how many times of movement can be judged as effective non-moveing*/
#define __GSENSOR_MOVE_TIME_OUT_THRESHOLD_MS__  (5000)      /**range 1~5000, the time of not move > __GSENSOR_MOVE_TIME_OUT_THRESHOLD_MS__, event : GH_MULTI_SENSOR_EVENT_GS_MOVE_TIME_OUT will be generated*/
#if (__GSENSOR_ENABLE__)
    #define __GSENSOR_MOVE_DET_ENABLE__             (1)    /**<  GSENSOR MOVE DET ENABLE  */
#else
    #define __GSENSOR_MOVE_DET_ENABLE__             (0)    /**<  GSENSOR MOVE DET ENABLE  */
#endif

/* function enable */
#define __FUNC_TYPE_ADT_ENABLE__                (1)    /**< adt function enable */
#define __FUNC_TYPE_HR_ENABLE__                 (1)    /**< hr function enable */
#define __FUNC_TYPE_HRV_ENABLE__                (0)    /**< hrv function enable */
#define __FUNC_TYPE_HSM_ENABLE__                (0)    /**< hsm function enable */
#define __FUNC_TYPE_FPBP_ENABLE__               (0)    /**< fpbp function enable */
#define __FUNC_TYPE_PWA_ENABLE__                (0)    /**< pwa function enable */
#define __FUNC_TYPE_SPO2_ENABLE__               (1)    /**< spo2 function enable */
#define __FUNC_TYPE_ECG_ENABLE__                (0)    /**< ecg function enable */
#define __FUNC_TYPE_PWTT_ENABLE__               (0)    /**< pwtt function enable */
#define __FUNC_TYPE_SOFT_ADT_GREEN_ENABLE__     (0)    /**< soft_adt_green function enable */
#define __FUNC_TYPE_BT_ENABLE__                 (0)    /**< bt function enable */
#define __FUNC_TYPE_RESP_ENABLE__               (0)    /**< resp function enable */
#define __FUNC_TYPE_AF_ENABLE__                 (0)    /**< af function enable */
#define __FUNC_TYPE_TEST1_ENABLE__              (0)    /**< test1 function enable */
#define __FUNC_TYPE_TEST2_ENABLE__              (0)    /**< test2 function enable */
#define __FUNC_TYPE_SOFT_ADT_IR_ENABLE__        (0)    /**< soft_adt_ir function enable */
#define __FUNC_TYPE_BIA_ENABLE__                (0)    /**< bia function enable */
#define __FUNC_TYPE_GSR_ENABLE__                (0)    /**< gsr function enable */

#define __HARD_ADT_ALGO_CHECK_EN__          (0)   //0: do not use algo to replace hard adt module in chip  1: use algo to replace hard adt mudule in chip

 
/*debug config*/
#define __SUPPORT_GHEALTH_PROTOCOL__            (1)         /**< driver lib support protocol analyze */

#define __EXAMPLE_LOG_TYPE__                    (__EXAMPLE_LOG_METHOD_0__) /**< example log config */
#define __EXAMPLE_LOG_LEVEL__                   (__EXAMPLE_LOG_LV_2__) /**< example log config */
#define   EXAMPLE_LOG_MAX_LEN                   (128)

/*config list*/
#define __GH3X_CFG_LIST_MAX_NUM__               (8)         /**< max num of config list(1 ~ 8) */

/* gh3x data buffer size config,this is relat   fifo water mark config */
#define __GH3X_RAWDATA_BUFFER_SIZE__            (240 * 5)   /**< rawdata buffer size in byte */

/* gsensor data buffer size config */
#define __GSENSOR_DATA_BUFFER_SIZE__            (240 * (3+3*__GS_GYRO_ENABLE__))   /**< max num of gsensor data */

#define __INT_PROCESS_MAX_LOOP_CNT__                        (5)       /**< max int process loop cnt */

/* function channel nunber config*/
#define GH3X_ADT_CHNL_NUM                       (8)        /**< adt chnl num */
#define GH3X_HR_CHNL_NUM                        (32)        /**< hr chnl num */
#define GH3X_HRV_CHNL_NUM                       (32)        /**< hrv chnl num */
#define GH3X_HSM_CHNL_NUM                       (32)        /**< hsm chnl num */
#define GH3X_FPBP_CHNL_NUM                      (32)        /**< fpbp chnl num */
#define GH3X_PWA_CHNL_NUM                       (32)        /**< pwa chnl num */
#define GH3X_SPO2_CHNL_NUM                      (32)        /**< spo2 chnl num */
#define GH3X_ECG_CHNL_NUM                       (32)        /**< ecg chnl num */
#define GH3X_PWTT_CHNL_NUM                      (32)        /**< pwtt chnl num */
#define GH3X_SOFT_ADT_GREEN_CHNL_NUM            (32)        /**< soft_adt_green chnl num */
#define GH3X_BT_CHNL_NUM                        (32)        /**< bt chnl num */
#define GH3X_RESP_CHNL_NUM                      (32)        /**< resp chnl num */
#define GH3X_AF_CHNL_NUM                        (32)        /**< af chnl num */
#define GH3X_TEST1_CHNL_NUM                     (32)        /**< test1 chnl num */
#define GH3X_TEST2_CHNL_NUM                     (32)        /**< test2 chnl num */
#define GH3X_SOFT_ADT_IR_CHNL_NUM               (32)        /**< soft_adt_ir chnl num */
#define GH3X_BIA_CHNL_NUM                       (32)        /**< bia chnl num */
#define GH3X_GSR_CHNL_NUM                       (32)        /**< bia chnl num */
#define GH3X_LEAD_CHNL_NUM                      (32)        /**< lead chnl num */

/***********************************  DO NOT MODIFY FOLLOWING CODE *******************************/
/* log def*/
#if (__EXAMPLE_LOG_LEVEL__ > __EXAMPLE_LOG_LV_0__)
#define   EXAMPLE_LOG(...)                do{\
                                            if(gpSnprintfUser)\
                                            {\
                                                char gh3x_example_log[EXAMPLE_LOG_MAX_LEN] = {0};\
                                                gpSnprintfUser(gh3x_example_log, EXAMPLE_LOG_MAX_LEN, __VA_ARGS__);\
                                                GH3X_Log((GCHAR *)gh3x_example_log);\
                                            }\
                                            if(gpPrintfUser)gpPrintfUser(__VA_ARGS__);\
                                        } while(0)
#if (__EXAMPLE_LOG_LEVEL__ == __EXAMPLE_LOG_LV_2__)
#define   GH3X_DEMO_LOG_PARAM_DEBUG(...)  do{\
                                            if(gpSnprintfUser)\
                                            {\
                                                char gh3x_example_log[EXAMPLE_LOG_MAX_LEN] = {0};\
                                                gpSnprintfUser(gh3x_example_log, EXAMPLE_LOG_MAX_LEN, __VA_ARGS__);\
                                                GH3X_Log((GCHAR *)gh3x_example_log);\
                                            }\
                                            if(gpPrintfUser)gpPrintfUser(__VA_ARGS__);\
                                        } while(0)
#else
#define   GH3X_DEMO_LOG_PARAM_DEBUG(...)
#endif
#else
#define   EXAMPLE_LOG(...) 
#define   GH3X_DEMO_LOG_PARAM_DEBUG(...)
#endif

#ifdef GH3X_MAIN_FUNC_CHNL_NUM
#undef GH3X_MAIN_FUNC_CHNL_NUM
#endif
#define GH3X_MAIN_FUNC_CHNL_NUM 1
#if GH3X_ADT_CHNL_NUM > GH3X_MAIN_FUNC_CHNL_NUM
#undef GH3X_MAIN_FUNC_CHNL_NUM
#define GH3X_MAIN_FUNC_CHNL_NUM GH3X_ADT_CHNL_NUM
#endif
#if GH3X_HR_CHNL_NUM > GH3X_MAIN_FUNC_CHNL_NUM
#undef GH3X_MAIN_FUNC_CHNL_NUM
#define GH3X_MAIN_FUNC_CHNL_NUM GH3X_HR_CHNL_NUM
#endif
#if GH3X_HRV_CHNL_NUM > GH3X_MAIN_FUNC_CHNL_NUM
#undef GH3X_MAIN_FUNC_CHNL_NUM
#define GH3X_MAIN_FUNC_CHNL_NUM GH3X_HRV_CHNL_NUM
#endif
#if GH3X_HSM_CHNL_NUM > GH3X_MAIN_FUNC_CHNL_NUM
#undef GH3X_MAIN_FUNC_CHNL_NUM
#define GH3X_MAIN_FUNC_CHNL_NUM GH3X_HSM_CHNL_NUM
#endif
#if GH3X_FPBP_CHNL_NUM > GH3X_MAIN_FUNC_CHNL_NUM
#undef GH3X_MAIN_FUNC_CHNL_NUM
#define GH3X_MAIN_FUNC_CHNL_NUM GH3X_FPBP_CHNL_NUM
#endif
#if GH3X_PWA_CHNL_NUM > GH3X_MAIN_FUNC_CHNL_NUM
#undef GH3X_MAIN_FUNC_CHNL_NUM
#define GH3X_MAIN_FUNC_CHNL_NUM GH3X_PWA_CHNL_NUM
#endif
#if GH3X_SPO2_CHNL_NUM > GH3X_MAIN_FUNC_CHNL_NUM
#undef GH3X_MAIN_FUNC_CHNL_NUM
#define GH3X_MAIN_FUNC_CHNL_NUM GH3X_SPO2_CHNL_NUM
#endif
#if GH3X_ECG_CHNL_NUM > GH3X_MAIN_FUNC_CHNL_NUM
#undef GH3X_MAIN_FUNC_CHNL_NUM
#define GH3X_MAIN_FUNC_CHNL_NUM GH3X_ECG_CHNL_NUM
#endif
#if GH3X_PWTT_CHNL_NUM > GH3X_MAIN_FUNC_CHNL_NUM
#undef GH3X_MAIN_FUNC_CHNL_NUM
#define GH3X_MAIN_FUNC_CHNL_NUM GH3X_PWTT_CHNL_NUM
#endif
#if GH3X_SOFT_ADT_GREEN_CHNL_NUM > GH3X_MAIN_FUNC_CHNL_NUM
#undef GH3X_MAIN_FUNC_CHNL_NUM
#define GH3X_MAIN_FUNC_CHNL_NUM GH3X_SOFT_ADT_GREEN_CHNL_NUM
#endif
#if GH3X_BT_CHNL_NUM > GH3X_MAIN_FUNC_CHNL_NUM
#undef GH3X_MAIN_FUNC_CHNL_NUM
#define GH3X_MAIN_FUNC_CHNL_NUM GH3X_BT_CHNL_NUM
#endif
#if GH3X_RESP_CHNL_NUM > GH3X_MAIN_FUNC_CHNL_NUM
#undef GH3X_MAIN_FUNC_CHNL_NUM
#define GH3X_MAIN_FUNC_CHNL_NUM GH3X_RESP_CHNL_NUM
#endif
#if GH3X_AF_CHNL_NUM > GH3X_MAIN_FUNC_CHNL_NUM
#undef GH3X_MAIN_FUNC_CHNL_NUM
#define GH3X_MAIN_FUNC_CHNL_NUM GH3X_AF_CHNL_NUM
#endif
#if GH3X_TEST1_CHNL_NUM > GH3X_MAIN_FUNC_CHNL_NUM
#undef GH3X_MAIN_FUNC_CHNL_NUM
#define GH3X_MAIN_FUNC_CHNL_NUM GH3X_TEST1_CHNL_NUM
#endif
#if GH3X_TEST2_CHNL_NUM > GH3X_MAIN_FUNC_CHNL_NUM
#undef GH3X_MAIN_FUNC_CHNL_NUM
#define GH3X_MAIN_FUNC_CHNL_NUM GH3X_TEST2_CHNL_NUM
#endif
#if GH3X_SOFT_ADT_IR_CHNL_NUM > GH3X_MAIN_FUNC_CHNL_NUM
#undef GH3X_MAIN_FUNC_CHNL_NUM
#define GH3X_MAIN_FUNC_CHNL_NUM GH3X_SOFT_ADT_IR_CHNL_NUM
#endif
#if GH3X_BIA_CHNL_NUM > GH3X_MAIN_FUNC_CHNL_NUM
#undef GH3X_MAIN_FUNC_CHNL_NUM
#define GH3X_MAIN_FUNC_CHNL_NUM GH3X_BIA_CHNL_NUM
#endif
#if GH3X_PACE_CHNL_NUM > GH3X_MAIN_FUNC_CHNL_NUM
#undef GH3X_MAIN_FUNC_CHNL_NUM
#define GH3X_MAIN_FUNC_CHNL_NUM GH3X_PACE_CHNL_NUM
#endif
#if GH3X_ECG20K_CHNL_NUM > GH3X_MAIN_FUNC_CHNL_NUM
#undef GH3X_MAIN_FUNC_CHNL_NUM
#define GH3X_MAIN_FUNC_CHNL_NUM GH3X_ECG20K_CHNL_NUM
#endif
#if GH3X_LEAD_CHNL_NUM > GH3X_MAIN_FUNC_CHNL_NUM
#undef GH3X_MAIN_FUNC_CHNL_NUM
#define GH3X_MAIN_FUNC_CHNL_NUM GH3X_LEAD_CHNL_NUM
#endif
#if GH3X_PRESSURE_CHNL_NUM > GH3X_MAIN_FUNC_CHNL_NUM
#undef GH3X_MAIN_FUNC_CHNL_NUM
#define GH3X_MAIN_FUNC_CHNL_NUM GH3X_PRESSURE_CHNL_NUM
#endif

/// number of max channel map id
#define  GH3X_CHANNEL_MAX_NUM             (GH3X_MAIN_FUNC_CHNL_NUM)

/// number of gsensor max channel
#define  GSENSOR_FRAME_MAX_NUM            (3 + 3 * __GS_GYRO_ENABLE__)

#if(__GH3X_INTERFACE__ != __GH3X_INTERFACE_SPI__)
#define __GH3X_SPI_TYPE__                 (__GH3X_SPI_TYPE_SOFTWARE_CS__)
#endif
#if(__GH3X_INTERFACE__ != __GH3X_INTERFACE_I2C__)
#define __GH3X_I2C_DEVICE_ID__            (GH3X_I2C_ID_SEL_1L0L)
#endif

#ifdef GOODIX_DEMO_PLATFORM
#include "Goodix_Platform_config.h"
#endif

#endif /* _GH3X_DEMO_CONFIG_H_ */

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
