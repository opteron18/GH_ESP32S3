/**
  ****************************************************************************************
  * @file    gh3x_bia_common.h
  * @author  GHealth Driver Team
  * @brief   
  #####Copyright (c) 2022 GOODIX
   All rights reserved.
  ****************************************************************************************
  */
#ifndef _GH3X_BIA_COMMON_H_
#define _GH3X_BIA_COMMON_H_

#include "gh3x_drv.h"
#include "stdint.h"

#define BIA_FRQ_NUM_LIMIT      3
#define BIA_IMPEDANCE_NET_NUM_LIMIT   5
#define BIA_FRAME_NUM_LIMIT      7
#define BIA_FRQ_NUM_MAX      8

#define  __BIA_IMPEDANCE_FIXER__  1

typedef struct {
	double real;
	double imag;
} complex_double;


typedef struct {
	int32_t real;
	int32_t imag;
} complex_S32;


typedef enum{
    BIA_STATE_INIT,
    BIA_SAMPLE_STATE_OFFSET,
    BIA_SAMPLE_STATE_CALI,
    BIA_SAMPLE_STATE_NET_IMEDANCE,
}EM_BIA_SAMPLE_STATE;

//typedef enum{
//    ZE1,
//    ZE2,
//    ZE3,
//    ZE4,
//    ZBODY,
//}EM_BIA_ELECTRODE;

#endif

