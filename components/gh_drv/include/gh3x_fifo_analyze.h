/**
  ****************************************************************************************
  * @file    gh3x_fifo_analyze.h
  * @author  GHealth Driver Team
  * @brief   
  ****************************************************************************************
  * @attention  
  #####Copyright (c) 2022 GOODIX
   All rights reserved.
  
  ****************************************************************************************
  */

#ifndef GH3X_FIFOANALYZE_H
#define GH3X_FIFOANALYZE_H

#include "gh3x_drv.h"


#define CHNL_MAP_CHECK_FAILED (0)
#define CHNL_MAP_CHECK_SUCCESS (1)
#define CHNL_MAP_CEHCK_FIRST (2)

#define CHNL_NUM_MAX (128)

#define CHNLMAP_GET_DATATYPE(x) ((x >> 13) & 0x7)
#define CHNLMAP_GET_CFGMUX(x) ((x >> 8) & 0x1F)
#define CHNLMAP_GET_REPEATFLAG(x) ((x >> 12) & 0x1)
#define CHNLMAP_GET_ADCNO(x) ((x >> 2) & 0x3)
#define CHNLMAP_GET_BGCANCELINDEX(x) ((x >> 0) & 0x1)

#define CHNLMAP_GET_ECGADCNO(x) ((x >> 9) & 0x3)
#define CHNLMAP_GET_ECGTYPE(x) ((x >> 11) & 0x3)

#define CHNLMAP_GET_BIASLOTNUM(x) ((x >> 9) & 0x7)
#define CHNLMAP_GET_BIAIQFLAG(x) ((x >> 12) & 0x1)

#define CHNLMAP_GET_GSRTYPE(x) ((x >> 11) & 0x3)
#define CHNLMAP_GET_GSRFLAG(x) ((x >> 9) & 0x3)

#define DATATYPE_PPG 0
#define DATATYPE_PPG_BG 1
#define DATATYPE_BG_CANCEL 2
#define DATATYPE_LED_DRV 3
#define DATATYPE_ECG 4
#define DATATYPE_BIA 5
#define DATATYPE_GSR 6

#define ECGTYPE_ECG    0
#define ECGTYPE_LEAD   1
#define ECGTYPE_PACE   2
#define ECGTYPE_ECG20K 3


#define GSR_DATATYPE_GSR 0
#define GSR_DATATYPE_PRESSURE 1
#define GSR_DATATYPE_TEMPERATURE 2

#define DEL_PPGCOLOR(x) x = (x & (GU16)(~(0xF << 4)))

#define TAG_GET_PPGGAIN(x) ((x >> 4) & 0xF)
#define TAG_GET_PPGADJFLAG(x) ((x >> 1) & 0x1)
#define TAG_GET_PPGSAFLAG(x) ((x >> 0) & 0x1)

#define TAG_GET_BGECGWEARON(x) ((x >> 1) & 0x1)

#define TAG_GET_ECGLEAD(x) ((x >> 7) & 0x1)
#define TAG_GET_ECGMODE(x) ((x >> 10) & 0x1)
#define TAG_GET_ECGFASTFLAG(x) ((x >> 8) & 0x1)

#define TAG_GET_BIALEAD(x) ((x >> 8) & 0x1)
#define TAG_GET_BIANUM(x) ((x >> 5) & 0x7)

#define TAG_GET_GSRSAFLAG(x) ((x >> 8) & 0x1)

void GH3X_SetFifoFullFlag(GU8 uchFlag);
void GH3X_InitResample(const STFrameInfo * const pstFrameInfo);
void GH3X_DemoEcgUncompress(GU8* puchFifoData, GU16 usDataByteLen,GU32 unFifoFull);

#endif
