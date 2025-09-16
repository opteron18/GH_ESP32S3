/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x_demo_version.h
 *
 * @brief   gh3x demo code version
 *
 * @version ref gh3x_demo_version.h
 *
 */

#ifndef _GH3X_DEMO_VERSION_H_
#define _GH3X_DEMO_VERSION_H_

#include "gh3x_demo_config.h"

#ifdef GOODIX_DEMO_PLATFORM
#include "slaver_log.h"
#endif     

#define   GH3X_DEMO_MAJOR_VERSION_NUMBER        0      /**< major version number */
#define   GH3X_DEMO_MINOR_VERSION_NUMBER        2      /**< minor version number */
#define   GH3X_DEMO_REVISION_VERSION_NUMBER     1      /**< revision version number */
#define   GH3X_DEMO_FIXED_VERSION_NUMBER        1      /**< fixed version number */

#define   GH3X_TO_STRING(x)       #x                      /**< number to char */
#define   GH3X_STR(x)             GH3X_TO_STRING(x)     /**< number to char */

#if (__DRIVER_LIB_MODE__ == __DRV_LIB_WITH_ALGO__)
#define   GH3X_ALGOCALL_EXTRA_VERSION_STRING      "_ALGO"
#else
#define   GH3X_ALGOCALL_EXTRA_VERSION_STRING      ""
#endif

#if (SLAVER_LOG_EN)
#define   GH3X_DEBUG_FLAG_STRING "_DEBUG"
#else
#define   GH3X_DEBUG_FLAG_STRING "" 
#endif

#define EVK_BOOT_LOADER_VERSION_ADDR   (0x00202400)

/// makeup version string
#define   GH3X_DEMO_VERSION_STRING    "GH(M)3X_EVK_"\
                                        "v"GH3X_STR(GH3X_DEMO_MAJOR_VERSION_NUMBER)\
                                        "."GH3X_STR(GH3X_DEMO_MINOR_VERSION_NUMBER)\
                                        "."GH3X_STR(GH3X_DEMO_REVISION_VERSION_NUMBER)\
                                        "."GH3X_STR(GH3X_DEMO_FIXED_VERSION_NUMBER)\
                                        GH3X_ALGOCALL_EXTRA_VERSION_STRING\
                                        GH3X_DEBUG_FLAG_STRING\
                                        "(build:"__DATE__"_"__TIME__")"
#endif
