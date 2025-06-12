/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh_demo_multi_sen_pro.h
 *
 * @brief   
 *
 * @version 
 *
 */


#ifndef _GH_DEMO_MULTI_SEN_PRO_H_
#define _GH_DEMO_MULTI_SEN_PRO_H_
#include "gh3x_drv.h"
#include "gh3x_demo_config.h"
#if __GH_MULTI_SENSOR_EVENT_PRO_CONIG__



/* adt confirm status */
#define GH_SENSOR_IS_NOT_MOVING                 ((GU8)0)
#define GH_SENSOR_IS_MOVING                     ((GU8)1)

#define GH_MULTI_SENSOR_EVENT_LL_INDEX_GH_WEAR_ON            0   //index of member in linked list for gh senosr wear on
#define GH_MULTI_SENSOR_EVENT_LL_INDEX_GH_WEAR_OFF           1   //index of member in linked list for gh senosr wear off
#define GH_MULTI_SENSOR_EVENT_LL_INDEX_GS_MOVE               2   //index of member in linked list for gsensor move
#define GH_MULTI_SENSOR_EVENT_LL_INDEX_GS_MOVE_TIME_OUT      3   //index of member in linked list for gsenosr move time out

#define GH_MULTI_SENSOR_EVENT_NUM_MAX           4
#define GH_MULTI_SENSOR_EVENT_LL_INDEX_INVALID     255

#define GH_MULTI_SENSOR_EVENT_GH_WEAR_ON        (1 << GH_MULTI_SENSOR_EVENT_LL_INDEX_GH_WEAR_ON)        // gh sensor wear on
#define GH_MULTI_SENSOR_EVENT_GH_WEAR_OFF       (1 << GH_MULTI_SENSOR_EVENT_LL_INDEX_GH_WEAR_OFF)       // gh sensor wear off
#define GH_MULTI_SENSOR_EVENT_GS_MOVE           (1 << GH_MULTI_SENSOR_EVENT_LL_INDEX_GS_MOVE)           // gsensor move
#define GH_MULTI_SENSOR_EVENT_GS_MOVE_TIME_OUT  (1 << GH_MULTI_SENSOR_EVENT_LL_INDEX_GS_MOVE_TIME_OUT)  //gs  move time out







void GhMultSensorWearEventManagerInit(void);
void GhMultSensorWearEventManagerHookRegister(void (*WearEventHook) (GU32 uchNewEvent));
void GhMultSensorWearEventManagerEnable(void);
void GhMultSensorWearEventManagerDisable(void);
GU8 GhMultSensorWearEventManagerIsEnable(void);
void GhMultSensorWearEventSend(GU32 unEvent);
GU8 GhMultiSensorConditionCheckIsNew (GU32 unEventNew, GU32 unEventOld);
GU8 GhMultiSensorConditionCheckInWindow (GU32 unEventComb, GU32 unWindow, GU8 uchNowTimeNeedInWin);





#define GH_MULTI_SENSOR_TIMER_USER_TIME_STAMP 0
#define GH_MULTI_SENSOR_TIMER_USER_MOVE_DET   1

#define GH_MULTI_SENSOR_TIMER_USER_NUM_MAX 2


typedef struct
{
	void (*TimerCreate)(GU32 unTimerPeriodMs);
    void (*TimerStart)(void);
	void (*TimerStop)(void);
    void (*TimerDelete)(void);
}STGhMultiSensorTimerOps;

typedef struct
{
	void (*TimerHandle)(GU32 unTimeIntervalMs);
}STGhMultiSensorTimerUser;


typedef struct
{
	const STGhMultiSensorTimerOps *pstTimerOps;
	STGhMultiSensorTimerUser stTimerUsers[GH_MULTI_SENSOR_TIMER_USER_NUM_MAX];
}STGhMultiSensorTimer;




void GhMultiSensorTimerInit(void);
void GhMultiSensorTimerStart(void);
void GhMultiSensorTimerStop(void);
void GhMultiSensorTimerOpsRegister(const STGhMultiSensorTimerOps *pstTimerOps);
void GhMultiSensorTimerOpsUnregister(void);
void GhMultiSensorTimerHandle(void);



void GhGsMoveDetecterInit(void);
void GhGsMoveDetecterEnable(void);
void GhGsMoveDetecterDisable(void);
GU8 GhGsMoveDetecterIsEnable(void);

void GhMoveDetectByGsData(GS16* pusGsData, GU16 usGsDataCnt);
#endif
#endif

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/

