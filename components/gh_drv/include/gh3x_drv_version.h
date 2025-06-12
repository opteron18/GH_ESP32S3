/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x_drv_version.h
 *
 * @brief   gh3x driver version and change log
 *
 * @par  history:
 * @verbatim
 * Date          Author       Version     Notes
 * 2022-07-12    zhengxiaoyu  v0.0.1.0    1.First version
 * 2022-08-29    zhengxiaoyu  v0.0.2.0    1.Add config check
 *                                        2.Add frame check
 *                                        3.Add function start&stop
 * @endverbatim  */

#ifndef _GH3X_DRV_VERSION_H_
#define _GH3X_DRV_VERSION_H_

   
#define   GH3X_DRV_MAJOR_VERSION_NUMBER        0      /**< major version number */
#define   GH3X_DRV_MINOR_VERSION_NUMBER        2      /**< minor version number */
#define   GH3X_DRV_REVISION_VERSION_NUMBER     1      /**< revision version number */
#define   GH3X_DRV_FIXED_VERSION_NUMBER        1      /**< fixed version number */

#define   GH3X_TO_STRING(x)       #x                      /**< number to char */
#define   GH3X_STR(x)             GH3X_TO_STRING(x)     /**< number to char */

/// makeup version string
#define   GH3X_VERSION_STRING     "v"GH3X_STR(GH3X_DRV_MAJOR_VERSION_NUMBER)\
                                    "."GH3X_STR(GH3X_DRV_MINOR_VERSION_NUMBER)\
                                    "."GH3X_STR(GH3X_DRV_REVISION_VERSION_NUMBER)\
                                    "."GH3X_STR(GH3X_DRV_FIXED_VERSION_NUMBER)\
                                    "(build:"__DATE__"_"__TIME__")"

#endif /* _GH3X_DRV_VERSION_H_ */

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
