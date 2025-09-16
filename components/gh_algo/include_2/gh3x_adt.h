/**
  ****************************************************************************************
  * @file    gh3x_soft_adt.h
  * @author  GHealth Driver Team
  * @brief   
  ****************************************************************************************
  * @attention  
  #####Copyright (c) 2022 GOODIX
   All rights reserved.
  
  ****************************************************************************************
  */

#ifndef _GH3X_DEMO_ADT_H_
#define _GH3X_DEMO_ADT_H_


#include "gh3x_drv.h"

/// wear on event en set
#define  WEAR_ON_EVENT_EN_CONFIG(en)        do {\
                                                GH3X_WriteRegBitField(GH3X_INT_CR2_REG_ADDR, \
                                                    GH3X_WEAR_ON_EVENT_EN_LSB, GH3X_WEAR_ON_EVENT_EN_MSB, (en)); \
                                            } while (0)

/// wear off event en set
#define  WEAR_OFF_EVENT_EN_CONFIG(en)        do {\
                                                GH3X_WriteRegBitField(GH3X_INT_CR2_REG_ADDR, \
                                                    GH3X_WEAR_OFF_EVENT_EN_LSB, GH3X_WEAR_OFF_EVENT_EN_MSB, (en)); \
                                            } while (0)



#define SOFT_WEAR_INPUT_DATA_CHNL0_INDEX   (0)
#define SOFT_WEAR_INPUT_DATA_ACCX_INDEX    (1)
#define SOFT_WEAR_INPUT_DATA_ACCY_INDEX    (2)
#define SOFT_WEAR_INPUT_DATA_ACCZ_INDEX    (3)
#define SOFT_WEAR_INPUT_COLOR_INDEX        (4)
#define SOFT_WEAR_INPUT_DATA_ADT_INDEX     (5) //ir  adt data
#define SOFT_WEAR_INPUT_DATA_BG_INDEX      (6)
#define SOFT_WEAR_INPUT_DATA_CAP0_INDEX    (7)
#define SOFT_WEAR_INPUT_DATA_CAP1_INDEX    (8)
#define SOFT_WEAR_INPUT_DATA_CAP2_INDEX    (9)
#define SOFT_WEAR_INPUT_DATA_CAP3_INDEX    (10)


#define SOFT_WEAR_INPUT_DATA_INDEX_TOTAL (11)
#define SOFT_WEAR_OUTPUT_DATA_INDEX_TOTAL (8)


#define SOFT_WEAR_OUTPUT_WEAR_STATE_BIT  (3)   


#define NADTCAP_BYTE_TRUE       (GU8)1
#define NADTCAP_BYTE_FALSE      (GU8)0



#define NADT_SAMPLE_FREQ  25
#define NADT_WWD_IIR_LEN  4

#define NADT_WEAR_LIVE_DETECTION
#ifdef  NADT_WEAR_LIVE_DETECTION // this is wear live dection, full wear dection,can distinguish body and other thing; more Ram
#define NADT_PPG_BUFF_LEN (4) //4
#else
#define NADT_PPG_BUFF_LEN (2) //lite
#endif // NADT_WEAR_LIVE_DETECTION

#define NADT_WWD_FILTER_BUFF_LEN (NADT_PPG_BUFF_LEN*NADT_SAMPLE_FREQ)
#define NADT_MATH_ROUND(v) round(v)
#define NADT_MATH_ABSF(v)  fabs(v)
#define NADT_MATH_ABS(v)   abs(v)
#define NADT_MATH_ROUNDF(v) roundf(v)
#define NADT_ABS_GS32(v) abs((GS32)(v))
#define NADT_MIN(a,b) ((a) > (b) ? (b) : (a))



#define NADT_PV_PEAK (1)
#define NADT_PV_VALLEY (2)
#define NADT_PV_PV_INTER (4)
#define NADT_ACC_TRUNCATE (5)

 

typedef struct
{
    GS32 *pData;
    GS32 nLength;
    GS32 nPos;
}NADT_QUEUE_BUFFER_S32;

typedef struct
{
    GS16 *pData;
    GS32 nLength;
    GS32 nPos;
}NADT_QUEUE_BUFFER_S16;

typedef struct
{
    const GF32 *pfCoefB;
    const GF32 *pfCoefA;
    GF32 *pfXbuff;
    GF32 *pfYbuff;
    GS32 lLen;
    GF32 fThr;
}NADT_ST_IIR_PARAM;

typedef struct
{
    GS32 *pfFftValue;
    GS16 *psPeakIndex;
    GS16 *psValleyIndex;
    GS16 *psPeakValleyIndex;
    GS32 lPeakValleyLen;
    GS32 lPeakLen;
    GS32 lValleyLen;
    GS32 lDataLen;
}NADT_ST_PEAK_VALLEY_INFO;

typedef struct
{
    GBOOL bSearchMax;
    GS32 fExtremumValue;
    GS32 lPreExtremumPos;
    GS32 lExtremumPos;
    GS32 lDataCount;
}NADT_ST_PEAKX_DET;

#endif /* _GH3X_DRV_SOFT_LEAD_H_ */

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/

