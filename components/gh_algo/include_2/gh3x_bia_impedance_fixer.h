/**
* @copyright (c) 2003 - 2024, Goodix Co., Ltd. All rights reserved.
*
* @file    gh3x_bia_cali_processor.h
*
* @brief   header file of gh3x bia impedance fixe process
*
* @version GH(M)3X2X_DRV_LIB_DEMO_1.0
*
* @author  Gooidx Iot Team
*
*/

#ifndef __GH3X_BIA_IMPEDANCE_FIXER_H__
#define __GH3X_BIA_IMPEDANCE_FIXER_H__

#include "gh3x_bia_common.h"
#include <stdio.h>
#include <math.h>

#define ZNewValueRxpZ1Index (0)
#define ZNewValueTxnZ2Index (1)
#define ZNewValueRxnZ3Index (2)
#define ZNewValueTxpZ4Index (3)
#define ZNewValueZbodyIndex (4)

typedef struct
{
    complex_double k1;
    complex_double k2;
    complex_double k3;
    complex_double k4;
    complex_double k5;
    complex_double k6;
    complex_double k7;
    complex_double k8;
    complex_double k9;
    complex_double k10;
    complex_double k11;
    complex_double k12;
    complex_double k13;
    complex_double k14;
    complex_double k15;
    complex_double k16;
    complex_double k17;
    complex_double k18;
}STBIAParameter;

typedef struct
{
    complex_double ZEL1A;
    complex_double ZEL2B;
    complex_double ZEL1B;
    complex_double ZEL2A;
    complex_double V1B2A;
} STBIAMeasure;

typedef struct
{
    STBIAParameter stBIAPara;
    complex_double stBIAW;  // 2*pi*fre
    STBIAMeasure stBIAMeasure;
}STGHxBIAInfo;


typedef enum
{
    BIA_OFFSET_IMP_FIXED_FAILED = 0,
    BIA_OFFSET_IMP_FIXED_OK = 1
}BID_OFFSET_IMP_FIXED_E;

BID_OFFSET_IMP_FIXED_E BiaImpedanceFixerZbodyFix(complex_double *pstImpedanceNetValue, GU16 uchFreq, complex_double *pstFixedZbody);
BID_OFFSET_IMP_FIXED_E BiaImpedanceFixerContactFixViaOpenParam(complex_double *pstImpedanceNetValue, complex_double stImpedanceOpenParam, GU16 uchFreq, complex_double *pstContactImpedance);


#endif

