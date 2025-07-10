/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 * 
 * @file    gh3x_demo_hook.c
 * 
 * @brief   gh3x driver lib demo code for hook
 * 
 * @author  Gooidx Iot Team
 * 
 */

#include "gh3x_demo.h"
#include "gh3x_demo_config.h"
#include "gh3x_demo_inner.h"
#include "gh_gsr.h"
//#include "gh3x_bia.h"
#if (__DRIVER_LIB_MODE__ == __DRV_LIB_WITH_ALGO__)
#include "gh3x_demo_algo_call.h"
#include "gh3x_demo_algo_config.h"
#endif

/* hook functions */

/**
 * @fn     void gh3x_init_hook_func(void)
 * 
 * @brief  gh3x init hook
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void gh3x_init_hook_func(void)
{

}

/**
 * @fn     void gh3x_samplingstart_hook_func(void)
 * 
 * @brief  gh3x start hook
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void gh3x_samplingstart_hook_func(void)
{
    GOODIX_PLATFROM_SAMPLING_START_HOOK_ENTITY();
}

/**
 * @fn     void gh3x_samplingstop_hook_func(void)
 * 
 * @brief  gh3x stop hook
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void gh3x_samplingstop_hook_func(void)
{

    GOODIX_PLATFROM_SAMPLING_STOP_HOOK_ENTITY();
}

/**
 * @fn     void gh3x_get_rawdata_hook_func(GU8 *read_buffer_ptr, GU16 length)
 * 
 * @brief  gh3x get rawdata hook
 *
 * @attention   None
 *
 * @param[in]   read_buffer_ptr     pointer to read rawdata buffer
 * @param[in]   length              length
 * @param[out]  None
 *
 * @return  None
 */
void gh3x_get_rawdata_hook_func(GU8 *read_buffer_ptr, GU16 length)
{
    /* code implement by user */
    /****************** FOLLOWING CODE IS EXAMPLE **********************************/

}

/**
 * @fn      void gh3x_algorithm_get_io_data_hook_func(const STFrameInfo * const pstFrameInfo)
 * 
 * @brief  get algorithm input and output data
 *
 * @attention   None        
 *
 * @param[in]   pstFrameInfo
 * @param[out]  None
 *
 * @return  None
 */


void gh3x_algorithm_get_io_data_hook_func(const STFrameInfo * const pstFrameInfo)
{
#if (__GOODIX_ALGO_CALL_MODE__)
//    GU8 uchFunctionId;
//    uchFunctionId = pstFrameInfo->uchFuncID;
//    #if __FUNC_TYPE_BIA_ENABLE__
//    if(uchFunctionId == GH3X_FUNC_OFFSET_BIA)
//    {
//        GH3X_BiaUpdateFlag(&pstFrameInfo->pstFlagInfo->punFlag[1]);
//    }
//    #endif
    //GOODIX_PLATFROM_SAMPLING_START_HOOK_ENTITY();
    GH3X_AlgoCalculate(1 << pstFrameInfo->uchFuncID);
    
#endif
    //function id and channel num
    //EXAMPLE_LOG("[IO_DATA]Function ID: 0x%X, channel num = %d, frame cnt = %d\r\n",(int)(pstFrameInfo->uchFuncID),
    //                                                                               (int)(pstFrameInfo->pstGh3xData->uchChnlNum),
    //                                                                               (GU8)(pstFrameInfo->punFrameCnt[0]));
    //gsensor data
#if 0
#if !__GS_GYRO_ENABLE__
    EXAMPLE_LOG("[IO_DATA]Gsensor:x=%d,y=%d,z=%d\r\n",\
                        (int)(pstFrameInfo->pstGsData->pusData[0]),\
                        (int)(pstFrameInfo->pstGsData->pusData[1]),\
                        (int)(pstFrameInfo->pstGsData->pusData[2])\
                );
#else
    EXAMPLE_LOG("[IO_DATA]Gsensor:x=%d,y=%d,z=%d,gx=%d,gy=%d,gz%d\r\n",\
                        (int)(pstFrameInfo->pstGsData->pusData[0]),\
                        (int)(pstFrameInfo->pstGsData->pusData[1]),\
                        (int)(pstFrameInfo->pstGsData->pusData[2]),\
                        (int)(pstFrameInfo->pstGsData->pusData[3]),\
                        (int)(pstFrameInfo->pstGsData->pusData[4]),\
                        (int)(pstFrameInfo->pstGsData->pusData[5])\
                );
#endif
#endif

    //rawdata
    //EXAMPLE_LOG("[IO_DATA]rawdata = ");
    //for(GU8 uchChnlCnt = 0; uchChnlCnt < pstFrameInfo->pstGh3xData->uchChnlNum; uchChnlCnt ++)
    //{
    //    EXAMPLE_LOG("%d,",(int)(pstFrameInfo->pstGh3xData->punRawdata[uchChnlCnt]));
    //}
    //EXAMPLE_LOG("\r\n");
    //if (pstFrameInfo->pstAlgoData->uchFlag)
    //{
    //    EXAMPLE_LOG("[IO_DATA]algoresult = ");
    //    for(GU8 uchChnlCnt = 0; uchChnlCnt < 5; uchChnlCnt ++)
    //    {
    //        EXAMPLE_LOG("%d,",(int)(pstFrameInfo->pstAlgoData->pnResults[uchChnlCnt]));
    //    }
    //    EXAMPLE_LOG("\r\n");
    //}
    #if 1
	if (((0x1) << (pstFrameInfo->uchFuncID)) == GH3X_FUNCTION_HRV)
	{
		EXAMPLE_LOG("hrv_fi=%d,gs=%d,%d,%d,raw:%d,%d,%d,%d,%d,%d,%d,%d \n",
			(int)*(pstFrameInfo->punFrameCnt)&0xFF, 
			(int)pstFrameInfo->pstGsData->pusData[0], 
			(int)pstFrameInfo->pstGsData->pusData[1],
			(int)pstFrameInfo->pstGsData->pusData[2], 
			(int)(pstFrameInfo->pstGh3xData->punRawdata[0] & 0x00FFFFFF),
			(int)(pstFrameInfo->pstGh3xData->punRawdata[1] & 0x00FFFFFF),
			(int)(pstFrameInfo->pstGh3xData->punRawdata[2] & 0x00FFFFFF),
			(int)(pstFrameInfo->pstGh3xData->punRawdata[3] & 0x00FFFFFF),
			(int)(pstFrameInfo->pstGh3xData->punRawdata[4] & 0x00FFFFFF),
			(int)(pstFrameInfo->pstGh3xData->punRawdata[5] & 0x00FFFFFF),
			(int)(pstFrameInfo->pstGh3xData->punRawdata[6] & 0x00FFFFFF),
			(int)(pstFrameInfo->pstGh3xData->punRawdata[7] & 0x00FFFFFF));
		if(pstFrameInfo->pstAlgoData->uchFlag)
		{
			EXAMPLE_LOG("hrv_data rri0:%d,rri1:%d,rri2:%d,rri3:%d,confi:%d,rri_count:%d \n", 
				(int)pstFrameInfo->pstAlgoData->pnResults[0], 
				(int)pstFrameInfo->pstAlgoData->pnResults[1],
				(int)pstFrameInfo->pstAlgoData->pnResults[2],
				(int)pstFrameInfo->pstAlgoData->pnResults[3],
				(int)pstFrameInfo->pstAlgoData->pnResults[4],
				(int)pstFrameInfo->pstAlgoData->pnResults[5]);
		}
	}
#endif
}

#if (__FUNC_TYPE_ECG_ENABLE__)
/**
 * @fn     void GH3X_LeadEventHook(GU32* punGotEvent)
 * 
 * @brief  Lead event hook
 *
 * @attention   None        
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_LeadEventHook(GU32* punGotEvent)
{
    GH3X_LeadEventProcess(punGotEvent);
    //ecg0
    if(*punGotEvent & GH3X_IRQ_MSK_ECG0_LEAD_ON_BIT)
    {
        //GOODIX_PLATFROM_LEAD_ON_EVENT();
    }
    else if(*punGotEvent & GH3X_IRQ_MSK_ECG0_LEAD_OFF_BIT)
    {
        //GOODIX_PLATFROM_LEAD_OFF_EVENT();
    }

    //ecg1
    if(*punGotEvent & GH3X_IRQ_MSK_ECG1_LEAD_ON_BIT)
    {
        //GOODIX_PLATFROM_LEAD_ON_EVENT();
    }
    else if(*punGotEvent & GH3X_IRQ_MSK_ECG1_LEAD_OFF_BIT)
    {
        //GOODIX_PLATFROM_LEAD_OFF_EVENT();
    }

    //gsr
    if(*punGotEvent & GH3X_IRQ_MSK_GSR_LEAD_ON_BIT)
    {
        //GOODIX_PLATFROM_LEAD_ON_EVENT();
    }
    else if(*punGotEvent & GH3X_IRQ_MSK_GSR_LEAD_OFF_BIT)
    {
        //GOODIX_PLATFROM_LEAD_OFF_EVENT();
    }
}
#endif



#if __GH_MULTI_SENSOR_EVENT_PRO_CONIG__

extern const STGhMultiSensorTimerOps g_stGhMultiSensorTimerOps;

void GhMultiSensorWearDetStop(void)
{
    GhMultiSensorTimerStop();
    GhMultiSensorTimerOpsUnregister();

    GhMultSensorWearEventManagerDisable();
#if (__GSENSOR_MOVE_DET_ENABLE__)
    GhGsMoveDetecterDisable();
#endif
    //restart ADT
    GH3X_DemoStopSampling(GH3X_FUNCTION_ADT);
    GH3X_DemoStartSampling(GH3X_FUNCTION_ADT);
}


void GhMultiSensorFinalWearOnHook(void)
{
    
}


/** @brief hook when new multi sensor wear event is generated,
 you can do operation as below in this hook. 
firstly, you can check event record in Event list via API GhMultiSensorConditionCheckInWindow or GhMultiSensorConditionCheckIsNew;
soconly, you can disable multi sensor event manager via API GhMultSensorWearEventManagerDisable
thirdly, you can open/close function via API GH3X_DemoStartSampling/GH3X_DemoStopSampling according event record 
**/
/// @param uchNewEvent such as: GH_MULTI_SENSOR_EVENT_GH_WEAR_ON
void GhMultiSensorEventHook(GU32 uchNewEvent)
{
    #if 1
    if(GH_MULTI_SENSOR_EVENT_GS_MOVE_TIME_OUT == uchNewEvent)
    {
        EXAMPLE_LOG("[GhMultiSensorEventHook] gs move time out, stop adt\r\n");
        GH3X_DemoStopSampling(GH3X_FUNCTION_ADT);
    }
    if(GH_MULTI_SENSOR_EVENT_GS_MOVE == uchNewEvent)
    {
        EXAMPLE_LOG("[GhMultiSensorEventHook] sensor gs move, start adt\r\n");
        GH3X_DemoStartSampling(GH3X_FUNCTION_ADT);
    }
    if(GhMultiSensorConditionCheckIsNew(GH_MULTI_SENSOR_EVENT_GH_WEAR_ON, GH_MULTI_SENSOR_EVENT_GH_WEAR_OFF))
    {
        if(GhMultiSensorConditionCheckInWindow(GH_MULTI_SENSOR_EVENT_GS_MOVE, 3000, 1))
        {
            //final wear on
            EXAMPLE_LOG("[GhMultiSensorEventHook] final wear on !!!! \r\n");
            GhMultiSensorWearDetStop();
            /****************START:   add your handle you want after final wear on ***************/
            GhMultiSensorFinalWearOnHook();
            /****************END:   add your handle you want after final wear on ***************/      
        }
    }
    #endif
}

void GhMultiSensorWearDetStart(void)
{
    EXAMPLE_LOG("[GhMultiSensorWearDetStart] Multi Sensor Wear Detect Start\r\n");
    GH3X_DemoStopSampling(GH3X_FUNCTION_ADT);

    GhMultiSensorTimerInit();
    GhMultiSensorTimerOpsRegister(&g_stGhMultiSensorTimerOps);
    GhMultiSensorTimerStart();

    GhMultSensorWearEventManagerInit();
    GhMultSensorWearEventManagerHookRegister(GhMultiSensorEventHook);
    GhMultSensorWearEventManagerEnable();
#if (__GSENSOR_MOVE_DET_ENABLE__)
    GhGsMoveDetecterInit();
    GhGsMoveDetecterEnable();
#endif
}
#endif




#if (__SUPPORT_HARD_ADT_CONFIG__)
/**
 * @fn     void GH3X_WearEventHook(GU32* punGotEvent, GU8* puchExentEx);
 * 
 * @brief  Wear event hook
 *
 * @attention   None        
 *
 * @param[in]   wear event
 * @param[in]   wear off type  0: no object  1: nonliving object    wear on type   0: object     1: living object
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_WearEventHook(GU32* punGotEvent, GU8* puchExentEx)
{
    if (*punGotEvent & GH3X_IRQ_MSK_WEAR_OFF_BIT)
    {
        GH3X_DemoStopSampling((GH3X_GetProtocolTargetFunction()) &(~GH3X_FUNCTION_ECG) &(~GH3X_FUNCTION_ADT)&(~GH3X_FUNCTION_BIA));
        GOODIX_PLATFROM_WEAR_OFF_EVENT();
        if((*puchExentEx)&GH3X_EVENT_EX_BIT_WEAR_LIVING_TYPE)
        {
            EXAMPLE_LOG("Wear off, no-living object!!!\r\n");
            #if __GH_MULTI_SENSOR_EVENT_PRO_CONIG__
            GhMultiSensorWearDetStart();
            #endif
        }
        else
        {
            EXAMPLE_LOG("Wear off, no object!!!\r\n");

            #if __GH_MULTI_SENSOR_EVENT_PRO_CONIG__
            if(GhMultSensorWearEventManagerIsEnable())   // multi-sensor wear detection is opened
            {
                GhMultSensorWearEventSend(GH_MULTI_SENSOR_EVENT_GH_WEAR_OFF);
            }
            #endif
        }

    }
    else if (*punGotEvent & GH3X_IRQ_MSK_WEAR_ON_BIT)
    {
        #if __GH_MULTI_SENSOR_EVENT_PRO_CONIG__
        if(GhMultSensorWearEventManagerIsEnable())
        {
            *punGotEvent &= (~GH3X_IRQ_MSK_WEAR_ON_BIT);   //avoid send wear on event to app when multi-sensor wear detection
        }
        else
        #endif
        {
            GOODIX_PLATFROM_WEAR_ON_EVENT();
        }
        
        if((*puchExentEx)&GH3X_EVENT_EX_BIT_WEAR_LIVING_TYPE)
        {
            EXAMPLE_LOG("Wear on, living-object!!!\r\n");
        }
        else
        {
            EXAMPLE_LOG("Wear on, object !!!\r\n");
            #if __GH_MULTI_SENSOR_EVENT_PRO_CONIG__
            if(0 == GhMultSensorWearEventManagerIsEnable())   // multi-sensor wear detection is not opened
            #endif
            {
                GH3X_DemoStartSampling((GH3X_GetProtocolTargetFunction()) &(~GH3X_FUNCTION_ECG) &(~GH3X_FUNCTION_ADT)&(~GH3X_FUNCTION_BIA));
            }

            #if __GH_MULTI_SENSOR_EVENT_PRO_CONIG__
            GhMultSensorWearEventSend(GH_MULTI_SENSOR_EVENT_GH_WEAR_ON);
            #endif

        }
    }
}
#endif
void GH3X_SamplingControlHook(GU32 unFuncMode, EMUprotocolParseCmdType emSwitch)
{
   GH3X_SamplingControlHookProcess(unFuncMode,  emSwitch);
}

void GH3X_IntTimeOutHook(void)
{
//    GOODIX_PLATFROM_TIMEOUT_HOOK_ENTITY();
}

void GH3X_LoadConfigDoneHook(void)
{
    GU16 usPllRefSel = GH3X_ReadRegBitField(0x0008,13,13);
    if(usPllRefSel)
    {
        GH3X_WriteRegBitField(0x0008,6,7,1);
    }
#if __FUNC_TYPE_GSR_ENABLE__
    GU16 gsr_current =  GH3X_ReadRegBitField(0x0206,8,10);
    set_gsr_current(gsr_current);
    GU16 gsr_tx_rate_reg =  GH3X_ReadRegBitField(0x696,0,2);
    float gsr_iq_rate = 125;
    for(GU8 i = 0; i < gsr_tx_rate_reg; i++)
    {
        gsr_iq_rate = gsr_iq_rate / 2.0f;
    }
    const STFrameInfo * gsrFrameInfo = GetstFrameInfo(GH3X_FUNC_OFFSET_GSR);
    gsrFrameInfo->pstReSampleInfo->fRawdataFs = gsr_iq_rate;
#endif
#if (__INTERRUPT_PROCESS_MODE__ == __POLLING_INT_PROCESS_MODE__)
    GH3X_WriteRegBitField(0x0008, 10, 10, 0);
#endif
}

