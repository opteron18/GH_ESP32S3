/**
  ****************************************************************************************
  * @file    BiaSampleManager.h
  * @author  GHealth Driver Team
  * @brief   
  #####Copyright (c) 2022 GOODIX
   All rights reserved.
  ****************************************************************************************
  */
#ifndef _BiaSampleManager_H_
#define _BiaSampleManager_H_
#include "gh3x_bia_common.h"

#define RG_BIA_SLT0_4_ISL_ADDR 0x0234 
#define RG_BIA_SLT5_7_ISL_ADDR 0x0236 

#define RG_BIA_SLT0_1_FSL_ADDR 0x0236 
#define RG_BIA_SLT2_6_FSL_ADDR 0x0238
#define RG_BIA_SLT7_FSL_ADDR   0x023a 

#define RG_BIA_SLT_FSL_MASK  0x0007

#define BIA_TX_SL_8UA 0x36db
#define BIA_ISL0_4_NOTUSE 0x7fff
#define BIA_ISL5_7_NOTUSE 0x01ff

#define RG_BIA_IQ_CALI_OFFSET_I_LOW_ADDR 0x06dc
#define RG_BIA_IQ_CALI_OFFSET_I_LOW_FIELD_MASK 0xFFFF
#define RG_BIA_IQ_CALI_OFFSET_I_HIGH_ADDR 0x06de
#define RG_BIA_IQ_CALI_OFFSET_I_FIELD_MASK 0x00ff
#define RG_BIA_IQ_CALI_OFFSET_Q_LOW_ADDR 0x06e0
#define RG_BIA_IQ_CALI_OFFSET_Q_LOW_FIELD_MASK 0xFFFF
#define RG_BIA_IQ_CALI_OFFSET_Q_HIGH_ADDR 0x06e2
#define RG_BIA_IQ_CALI_OFFSET_Q_HIGH_FIELD_MASK 0x00ff

#define RG_BIA_IQ_CALI_I_LOW_ADDR 0x06a4
#define RG_BIA_IQ_CALI_I_LOW_FIELD_MASK 0xFFFF
#define RG_BIA_IQ_CALI_I_HIGH_ADDR 0x06a6
#define RG_BIA_IQ_CALI_I_HIGH_FIELD_MASK 0x0007
#define RG_BIA_IQ_CALI_Q_LOW_ADDR 0x06a8
#define RG_BIA_IQ_CALI_Q_LOW_FIELD_MASK 0xFFFF
#define RG_BIA_IQ_CALI_Q_HIGH_ADDR 0x06aa
#define RG_BIA_IQ_CALI_Q_HIGH_FIELD_MASK 0x0007

#define RG_FIFO_RESET_ADDR 0x0008
#define RG_FIFO_RESET_FIELD_MASK 0xc000

#define RG_BIA_CTRL0 0x0208
#define RG_BIA_EN_MASK 0x1
#define RG_BIA_SlotTime_MASK 0x00f0
#define RG_BIA_FrameNum_MASK 0x0700
#define RG_BIA_SlotNum_MASK  0x7000

#define RG_BIA_CTRL1 0x020A
#define RG_BIA_QcTime_MASK 0x7000
#define BIA_QcTime_10mS  0x0000

#define BIA_SlotTime_64mS  0x0000
#define BIA_SlotTime_128mS  0x0010
#define BIA_SlotTime_512mS  0x0030
#define BIA_SlotTime_1s  0x0040
#define BIA_FrameNum_1  0x0100
#define BIA_FrameNum_5  0x0500
#define BIA_SlotNum_0   0x0000
                 
#define RG_BIA_REG0 0x020c
#define RG_BIA_FrameLoopEn_MASK  0x0800
#define RG_BIA_FrameLoopNum_MASK 0x7000
#define RG_BIA_FrameLoopEn  0x0800

#define BIA_FrameLoopNum_1  0x1000


typedef enum{
    BIA_RET_OKAY,
    BIA_RET_STATE_ERROR,
    BIA_RET_REG_RW_ERROR,
    BIA_RET_ERROR
}EM_BIA_RET_STAET;


typedef enum{
    BIA_NULL_EVENT,
    BIA_START_EVENT,
    BIA_STOP_EVENT,
}EM_BIA_EVENT;

extern void BiaSampleManagerSampleSwitch( EM_BIA_SAMPLE_STATE uchStateNext);
extern EM_BIA_RET_STAET GH3X_BiaStart(void);
extern EM_BIA_RET_STAET GH3X_BiaStop(void);

#endif
