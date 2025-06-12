/**
  ****************************************************************************************
  * @file    gh3x_bia.h
  * @author  GHealth Driver Team
  * @brief   
  ****************************************************************************************
  * @attention  
        FALG:
        pstFrameInfo->pstFlagInfo->punFlag[0]  --->       Last frame flag
        pstFrameInfo->pstFlagInfo->punFlag[1]  --->       EM_BIA_STATE_MACHINE
        pstFrameInfo->pstFlagInfo->punFlag[2]  --->       EM_BIA_REPORT_MESSAGE
		pstFrameInfo->pstFlagInfo->punFlag[3]  --->       EM_BIA_Electrode_STATE

        RESULTS:
        pstFrameInfo->STAlgoData->pnResults[0] --->      bit0:算法停止采集标记  bit1:当前数据算法是否丢弃（slot切换时前几帧数据算法丢掉，为解决App显示纵坐标大的问题：算法丢掉数据App不显示）
        pstFrameInfo->STAlgoData->pnResults[1] --->      佩戴标记
        pstFrameInfo->STAlgoData->pnResults[2] --->      置信度
        pstFrameInfo->STAlgoData->pnResults[3] --->      BIA_I
        pstFrameInfo->STAlgoData->pnResults[4] --->      BIA_Q
        pstFrameInfo->STAlgoData->pnResults[5] --->      除脂体重*100
        pstFrameInfo->STAlgoData->pnResults[6] --->      体脂率*100*100%
        pstFrameInfo->STAlgoData->pnResults[7] --->      体水分量*100
        pstFrameInfo->STAlgoData->pnResults[8] --->      体水分率*100*100%
        pstFrameInfo->STAlgoData->pnResults[9] --->      基础代谢率*100
        pstFrameInfo->STAlgoData->pnResults[10]--->      BMI*100
        pstFrameInfo->STAlgoData->pnResults[11]--->      SMA*100 骨骼肌
        pstFrameInfo->STAlgoData->pnResults[12]--->      Mag*1000 (阻抗幅值,单位: 欧姆)
        pstFrameInfo->STAlgoData->pnResults[13]--->      Phase*1000 (阻抗相角,单位: ° )

        虚拟寄存器：
        NAME        MEAN        TYPE         VIRTURAL_RAG_ADDR_LOW          VIRTURAL_RAG_ADDR_HIGH
        para0       身高          99·256cm；            0xA0C0 -                    0XA0C2
        para1       体重          35`256KG*10;         0xA0C4 -                    0XA0C6
        para2       性别          0 女  1男；             0xA0C8 -                    0XA0CA
        para3       年龄          10·127；              0xA0CC -                    0XA0CE
        para4       是否运动员       0否  1是               0xA0D0 -                    0XA0D2
  #####Copyright (c) 2022 GOODIX
   All rights reserved.
  ****************************************************************************************
  */
#ifndef _GH3X_BIA_H_
#define _GH3X_BIA_H_

#include "gh3x_drv.h"

#define WEAR_OFF     0
#define WEAR_ON      1
 
#define BIA_FrameNum_Cali    0     //// 采集校准电阻
#define BIA_FrameNum_LoopStart  1  //// 循环采集的起始FrameNum
#define BIA_FrameNum_LeadStart  2  ////LeadDet开始的FrameNum
#define BIA_FrameNum_LeadEnd    5    ////LeadDet结束的FrameNum
#define BIA_FrameNum_ContactStart  1  ////接触阻抗测量开始的FrameNum
#define BIA_FrameNum_ContactEnd    4  ////接触阻抗测量结束的FrameNum
#define BIA_FrameNum_RxBody  5   //// 采集人体阻抗
  
typedef enum{
    BIA_STATE_INIT,
    BIA_STATE_OFFSET,
    BIA_STATE_CALIBRATE,
    BIA_STATE_EXECUTE,
    BIA_STATE_LEAD_DET,
    BIA_STATE_EVK_IMPEDANCE,
}EM_BIA_STATE_MACHINE;

typedef enum{
    BIA_NULL_EVENT,
    BIA_START_EVENT,
    BIA_STOP_EVENT,
    BIA_OFFSET_PROCESS_EVENT,
    BIA_OFFSET_PROCESS_FINISH_EVENT,
    BIA_OFFSET_PROCESS_SUCCESS_EVENT,
    BIA_OFFSET_PROCESS_FAIL_EVENT,
    BIA_IMPEDANCE_CALIBRATION_SUCCESS_EVENT,
    BIA_IMPEDANCE_CALIBRATION_FAIL_EVENT,
    BIA_EXECUTE_ABNORMAL_EVENT,
    BIA_EXECUTE_FAIL_EVENT,
    BIA_EXECUTE_SUCCESS_EVENT,
    BIA_EXECUTE_WEAROFF_EVENT,
    BIA_EXECUTE_WEARON_EVENT,
}EM_BIA_EVENT;


typedef enum{
    BIA_TEST_NO_MESSAGE,
    BIA_TEST_SUCCESS_MESSAGE,//测量成功消息
    BIA_TEST_FAIL_WEAR_MESSAGE,//佩戴状态异常导致测量失败消息
    BIA_TEST_FAIL_CONFIDENCE_MESSAGE,//置信度异常导致测量失败消息
    BIA_TEST_ABNORMAL_POSTURE_MESSAGE, //姿势提醒
    BIA_TEST_ABNORMAL_WEAR_MESSAGE,//佩戴提醒
}EM_BIA_REPORT_MESSAGE;


typedef enum{
    BIA_START,
    BIA_STOP,
}EM_BIA_ALGO_STATE;

typedef enum{
    BIA_LEAD_OFF,
    BIA_LEAD_ON,
}EM_BIA_LEAD_STATE;


typedef enum{
    BIA_LEAD_OK,
    BIA_FINGER_RXN_OFF,
    BIA_FINGER_TXN_OFF,
    BIA_FINGER_OFF,
    BIA_WRIST_RXP_OFF,
    BIA_WRIST_TXP_OFF,
    BIA_WRIST_OFF,
}EM_BIA_ELECTRODE_STATE;


typedef struct{
	  GU8 offsetFinish;
    GU16 offsetCnt;
    GS32 offsetIDataSum;
    GS32 offsetQDataSum;
    GS32 offsetIValue;
    GS32 offsetQValue;
}ST_BIA_OFFSET;

typedef struct{
    GU16 RxBodyCnt;
    GU16 RxnCnt;
    GU16 RxpCnt;
    GU16 TxnCnt;
    GU16 TxpCnt;
    GS64 RxBodyPhaseSum;
    GS64 RxBodyMagSum;
    GS64 RxnSumI;
    GS64 RxnSumQ;
    GS64 RxpSumI;
    GS64 RxpSumQ;
    GS64 TxnSumI;
    GS64 TxnSumQ;
    GS64 TxpSumI;
    GS64 TxpSumQ;
    GS64 RxBodySumI;
    GS64 RxBodySumQ;

}ST_BIA_CONTACT;

typedef struct{
    EM_BIA_STATE_MACHINE state;
    EM_BIA_EVENT event;
    EM_BIA_REPORT_MESSAGE message;
    ST_BIA_OFFSET st_offset;
    ST_BIA_CONTACT st_contact;
}ST_BIA;

typedef enum{
    BIA_RET_OKAY,
    BIA_RET_STATE_ERROR,
    BIA_RET_REG_RW_ERROR,
    BIA_RET_ERROR
}EM_BIA_RET_STAET;

EM_BIA_RET_STAET GH3X_BiaUpdateFlag(GU32 *state_flag);
EM_BIA_RET_STAET GH3X_BiaSetWearState(GU8 state);
EM_BIA_RET_STAET GH3X_BiaProcess(const STFrameInfo * const pstFrameInfo);
EM_BIA_RET_STAET GH3X_BiaStop(void);
EM_BIA_RET_STAET GH3X_BiaStart(void);

extern GU16 g_usBiaMeasureMode;
extern GU16 g_usBiaCalibrateRx;
extern GU16 g_usBiaCapacitanceTxp;
extern GU16 g_usBiaCapacitanceTxn;
extern GU16 g_usBiaCapacitanceRxp;
extern GU16 g_usBiaCapacitanceRxn;

#define BIA_MODE_DEMO       0  //手表demo工作模式
#define BIA_MODE_Z_BODY     1  //EVK阻抗测量：自测Zbody
#define BIA_MODE_Z_CONTACT  2  //EVK阻抗测量接触阻抗和Zbody都测

#endif

