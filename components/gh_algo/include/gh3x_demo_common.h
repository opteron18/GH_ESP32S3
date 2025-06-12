/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x_demo_common.h
 *
 * @brief   gh3x algo common define
 *
 * @version ref gh3x_demo_common.h
 *
 */

#ifndef _GH3X_DEMO_COMMON_H_
#define _GH3X_DEMO_COMMON_H_


#include "gh3x_demo_algo_config.h"
#include "goodix_log.h"

/* extern log function */
extern void GH3X_AlgoLog(GCHAR *chLogString);
/// log support len
#define GH3X_LOG_ALGO_SUP_LEN     (128)

#define   GH3X_SAMPLE_ALGO_LOG_PARAM(...)         do {\
                                                        GCHAR chDebugStr[GH3X_LOG_ALGO_SUP_LEN] = {0};\
                                                        snprintf(chDebugStr, GH3X_LOG_ALGO_SUP_LEN, \
                                                                "[gh3x_algo]: "__VA_ARGS__);\
                                                        GH3X_AlgoLog(chDebugStr);\
                                                    } while (0)

#if (__ALGO_LOG_CONFIG__)
#define   GH3X_ALGO_LOG_PARAM(...)        do {\
                                                GCHAR chDebugStr[GH3X_LOG_ALGO_SUP_LEN] = {0};\
                                                snprintf(chDebugStr, GH3X_LOG_ALGO_SUP_LEN, \
                                                        "[gh3x_algo]: "__VA_ARGS__);\
                                                GH3X_AlgoLog(chDebugStr);\
                                            } while (0)


#else
#define   GH3X_ALGO_LOG_PARAM(...)
#endif


#endif /* _GH3X_DRV_COMMON_H_ */

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
