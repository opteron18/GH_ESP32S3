/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x_demo_algo_config.h
 *
 * @brief   gh3x algo config
 *
 * @version ref gh3x_demo_algo_config.h
 *
 */
#ifndef _GH3X_DEMO_ALGO_CONFIG_H_
#define _GH3X_DEMO_ALGO_CONFIG_H_

#ifdef GOODIX_DEMO_PLATFORM
//#include "Goodix_Platform_config.h"
#endif

#define GH3X_ALGO_FUNCTION_GROUP_0 (GH3X_FUNCTION_HR|GH3X_FUNCTION_HRV|GH3X_FUNCTION_SOFT_ADT_GREEN|GH3X_FUNCTION_SOFT_ADT_IR)
#define GH3X_ALGO_FUNCTION_GROUP_1 (GH3X_FUNCTION_SPO2)
#define GH3X_ALGO_FUNCTION_GROUP_2 (GH3X_FUNCTION_ECG)
#define GH3X_ALGO_FUNCTION_GROUP_3 (GH3X_NO_FUNCTION)
#define GH3X_ALGO_FUNCTION_GROUP_4 (GH3X_NO_FUNCTION)


/* function algo enable config */
#define __USE_GOODIX_HR_ALGORITHM__            (0)
#define __USE_GOODIX_HRV_ALGORITHM__           (0)
#define __USE_GOODIX_HSM_ALGORITHM__           (0)
#define __USE_GOODIX_SPO2_ALGORITHM__          (0)
#define __USE_GOODIX_ECG_ALGORITHM__           (0)
#define __USE_GOODIX_BT_ALGORITHM__            (0)
#define __USE_GOODIX_RESP_ALGORITHM__          (0)
#define __USE_GOODIX_AF_ALGORITHM__            (0)
#define __USE_GOODIX_BP_ALGORITHM__            (0)
#define __USE_GOODIX_SOFT_ADT_ALGORITHM__      (0)
#define __USE_GOODIX_BIA_ALGORITHM__           (0)
#define __USE_GOODIX_GSR_ALGORITHM__           (0)

/* algorithm log */
#define __ALGO_LOG_CONFIG__                    (0)

/* algorithm config max number */
#define __GOODIX_ALGO_CFG_LIST_MAX_NUM__       (1)

/*algorithm output value process strategy config*/
#define __GH3X_HR_OUTPUT_VALUE_STRATEGY_EN__ (1)

/* algoritm channel nunber config*/
#define __HR_ALGORITHM_SUPPORT_CHNL_NUM__      (4)  /* range 1~4 */
#define __SPO2_ALGORITHM_SUPPORT_CHNL_NUM__    (4)  /* range 1-4 */

#if (__USE_GOODIX_SOFT_ADT_ALGORITHM__)
#define __SOFT_ADT_IR_DETECT_TIMEOUT__         (30) /** (recomended value = 30 seconds) ir detect timeout **/
#endif


#if (\
    __USE_GOODIX_HR_ALGORITHM__      ||\
    __USE_GOODIX_HRV_ALGORITHM__     ||\
    __USE_GOODIX_HSM_ALGORITHM__     ||\
    __USE_GOODIX_SPO2_ALGORITHM__    ||\
    __USE_GOODIX_ECG_ALGORITHM__     ||\
    __USE_GOODIX_BT_ALGORITHM__      ||\
    __USE_GOODIX_RESP_ALGORITHM__    ||\
    __USE_GOODIX_AF_ALGORITHM__      ||\
    __USE_GOODIX_BP_ALGORITHM__      ||\
    __USE_GOODIX_SOFT_ADT_ALGORITHM__||\
    __USE_GOODIX_BIA_ALGORITHM__||\
    __USE_GOODIX_GSR_ALGORITHM__\
    )
#define __GOODIX_ALGO_CALL_MODE__           (1)

#else
#define __GOODIX_ALGO_CALL_MODE__           (0)
#endif


#endif /* _GH3X_DEMO_ALGO_CALL_H_ */

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
