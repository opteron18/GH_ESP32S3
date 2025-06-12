/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x_drv_adt.c
 *
 * @brief   gh3x adt functions
 *
 * @version ref gh3x_drv_version.h
 *
 */
#include <stdio.h>
#include <math.h>
#include "string.h"
#include <stdlib.h>
#include "gh3x_demo_config.h"
#include "gh3x_demo.h"
#include "gh3x_demo_inner.h"
#include "gh3x_drv.h"
#include "gh3x_adt.h"
#include "gh3x_fifo_analyze.h"


#if 1//(__USE_GOODIX_WEAR_PROCESS__)
GU8 guchEcgLeadStatus = 0;//ecg lead status 
GU8 guchAdtWearStatus = 0;//0:ppg 5Hz wear off, 1:ppg 5Hz wear on
extern GU32 GH3X_GetDemoFuncMode(void);
extern void GH3X_UprotocolSendChipEvent(GU32 unEvent);

void GH3X_SamplingControlHookProcess(GU32 unFuncMode, EMUprotocolParseCmdType emSwitch)
{
    if(((GH3X_FUNCTION_ADT & unFuncMode) || (GH3X_FUNCTION_BIA & unFuncMode)) && emSwitch == UPROTOCOL_CMD_START)
    {
        guchAdtWearStatus = 0;
        guchEcgLeadStatus=0;
    }
    else if((GH3X_FUNCTION_ADT & unFuncMode || (GH3X_FUNCTION_BIA & unFuncMode)) && emSwitch == UPROTOCOL_CMD_STOP)
    {
        guchAdtWearStatus = 0;
        guchEcgLeadStatus=0;
        GOODIX_PLATFROM_LEAD_OFF_EVENT();
        GOODIX_PLATFROM_WEAR_OFF_EVENT();
    }
}

void GH3X_WearEventProcess(GU32* punGotEvent, GU8* puchExentEx)
{
    if(0 == (GH3X_FUNCTION_ADT & GH3X_GetDemoFuncMode()) && 0 == (GH3X_FUNCTION_BIA & GH3X_GetDemoFuncMode()))
    {
        return;
    }
    GU8 uchLastAdtWearStatus = guchAdtWearStatus;
    if (*punGotEvent & GH3X_IRQ_MSK_WEAR_OFF_BIT)
    {
        guchAdtWearStatus = 0;
    }
    else if (*punGotEvent & GH3X_IRQ_MSK_WEAR_ON_BIT)
    {
        guchAdtWearStatus = 1;
    }
    if(guchAdtWearStatus == uchLastAdtWearStatus)
    {
        return;
    }
#if 0 == __HARD_ADT_ALGO_CHECK_EN__
    if(guchAdtWearStatus)
    {
        GH3X_SetWearStatusReg(1);
    }
    else
    {
        GH3X_SetWearStatusReg(0);
    }
#endif

}



void GH3X_LeadEventProcess(GU32* punGotEvent)
{
    if(0 == (GH3X_FUNCTION_ADT & GH3X_GetDemoFuncMode()) || 0 == (GH3X_FUNCTION_ECG & GH3X_GetProtocolTargetFunction()))
    {
        return;
    }
    GU8 uchLastEcgLeadStatus = guchEcgLeadStatus;
    
    //ecg1
    if(*punGotEvent & GH3X_IRQ_MSK_ECG1_LEAD_ON_BIT)
    {
        guchEcgLeadStatus |= 2;
    }
    else if(*punGotEvent & GH3X_IRQ_MSK_ECG1_LEAD_OFF_BIT)
    {
        guchEcgLeadStatus &= (~2);
    }
    //ecg0
    if(*punGotEvent & GH3X_IRQ_MSK_ECG0_LEAD_ON_BIT)
    {
        guchEcgLeadStatus |= 1;
    }
    else if(*punGotEvent & GH3X_IRQ_MSK_ECG0_LEAD_OFF_BIT)
    {
        guchEcgLeadStatus &= (~1);
    }
    //"gsr"
    if(*punGotEvent & GH3X_IRQ_MSK_GSR_LEAD_ON_BIT)
    {
        guchEcgLeadStatus |= 4;
    }
    else if(*punGotEvent & GH3X_IRQ_MSK_GSR_LEAD_OFF_BIT)
    {
        guchEcgLeadStatus &= (~4);
    }
    
    if(guchEcgLeadStatus == uchLastEcgLeadStatus)
    {
        return;
    }
    GU8 uchEcgCnt = gpstFrameInfo[GH3X_FUNC_OFFSET_ECG]->pstGh3xData->uchChnlNum - GH3X_ExistPace(gpstFrameInfo[GH3X_FUNC_OFFSET_ECG]);
    if(GhPopCount(guchEcgLeadStatus) >= uchEcgCnt)
    {
        GOODIX_PLATFROM_LEAD_ON_EVENT();
        const STRegField regfields1[]={
            {0x538,0,11,0xE49},  // switch to only iq
            {0x506,7,12,21},     // switch to itype
            {0X900,0,0,0},       // reset IQ
            {0X900,0,0,1},
            {0X910,0,0,0},
            {0X910,0,0,1},
            {0X920,0,0,0},
            {0X920,0,0,1},
            {0X580,2,3,0},       //fast recovery
            {0X580,2,3,3}, 
            {0X254,2,2,0},       // lead on output feedback
            {0X500,2,2,0},       // reset lead
            {0X500,2,2,1},       // reset lead
            {0X880,0,0,0},       // reset pace
            {0X880,0,0,1}, 

        };
         
        GH3X_WriteRegBitFields( regfields1 , sizeof(regfields1)/sizeof(regfields1[0]));
        GH3X_DemoStartSampling((GH3X_GetProtocolTargetFunction()) &(GH3X_FUNCTION_ECG));
    }
    else
    {
        GOODIX_PLATFROM_LEAD_OFF_EVENT();
        const STRegField regfields1[]={
            {0x506,7,12,42},      // switch toRtype
            {0x538,0,11,0xE00},   // switch to only CMP
            {0X254,2,2,1},        // lead off output VCM
        };
        GH3X_DemoStopSampling((GH3X_GetProtocolTargetFunction()) &(GH3X_FUNCTION_ECG));
        GH3X_WriteRegBitFields( regfields1 , sizeof(regfields1)/sizeof(regfields1[0]));
        GH3X_WriteRegBitField( 0x0536 , 0,8,(((uchLastEcgLeadStatus - guchEcgLeadStatus) & 0x07) << 6 ) & 0x01EA);
        //gh3x_bsp_delay_ms(500);
    }

}

#endif

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/



