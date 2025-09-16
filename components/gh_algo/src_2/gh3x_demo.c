/**
  ****************************************************************************************
  * @file    gh3x_demo.c
  * @author  GHealth Driver Team
  * @brief   gh3x driver example
  ****************************************************************************************
  * @attention  
  #####Copyright (c) 2022 GOODIX
   All rights reserved.
  
  ****************************************************************************************
  */

/* includes */
#include "stdint.h"
#include "string.h"

#include "gh3x_drv.h"
#include "gh3x_demo.h"
#include "gh3x_demo_config.h"
#include "gh3x_demo_inner.h"
#include "gh3x_demo_version.h"
#include "BiaSampleManager.h"
#include "gh_gsr.h"
#include "gh3x_fifo_analyze.h"
#if (__DRIVER_LIB_MODE__ == __DRV_LIB_WITH_ALGO__)
#include "gh3x_demo_algo_call.h"
#include "gh3x_demo_algo_config.h"
#endif

#if (__SUPPORT_SOFT_AGC_CONFIG__)
#include "gh3x_soft_led_agc.h"
#endif

#include "gh_rpccore.h"
#include "gh_multi_sen_pro.h"


#define GH3X_INT_PROCESS_SUCCESS              (0)         /**< gh3x interrupt process success */
#define GH3X_INT_PROCESS_REPEAT               (1)         /**< need process gh3x interrupt process again */

/* GH3X event that need to process */
#define __GH3X_EVENT_PROCESS_MASK__   (\
                                            GH3X_IRQ_MSK_FIFO_WATERMARK_BIT | GH3X_IRQ_MSK_FIFO_FULL_BIT |GH3X_IRQ_MSK_CHIP_RESET_BIT|\
                                            GH3X_IRQ_MSK_WEAR_ON_BIT | GH3X_IRQ_MSK_WEAR_OFF_BIT |\
                                            GH3X_IRQ_MSK_ECG0_LEAD_ON_BIT | GH3X_IRQ_MSK_ECG0_LEAD_OFF_BIT |\
                                            GH3X_IRQ_MSK_ECG1_LEAD_ON_BIT | GH3X_IRQ_MSK_ECG1_LEAD_OFF_BIT |\
                                            GH3X_IRQ_MSK_GSR_LEAD_ON_BIT | GH3X_IRQ_MSK_GSR_LEAD_OFF_BIT\
                                            )

/* GH3X event that need to protocol */
#define __GH3X_EVENT_PROTOCOL_MASK__       (GH3X_IRQ_MSK_FIFO_FULL_BIT | GH3X_IRQ_MSK_WEAR_ON_BIT | GH3X_IRQ_MSK_WEAR_OFF_BIT |\
                                            GH3X_IRQ_MSK_ECG0_LEAD_ON_BIT | GH3X_IRQ_MSK_ECG0_LEAD_OFF_BIT |\
                                            GH3X_IRQ_MSK_ECG1_LEAD_ON_BIT | GH3X_IRQ_MSK_ECG1_LEAD_OFF_BIT |\
                                            GH3X_IRQ_MSK_GSR_LEAD_ON_BIT | GH3X_IRQ_MSK_GSR_LEAD_OFF_BIT\
                                            )

#define __GH3X_LEAD_EVENT_MASK__           (GH3X_IRQ_MSK_ECG0_LEAD_ON_BIT | GH3X_IRQ_MSK_ECG0_LEAD_OFF_BIT |\
                                            GH3X_IRQ_MSK_ECG1_LEAD_ON_BIT | GH3X_IRQ_MSK_ECG1_LEAD_OFF_BIT |\
                                            GH3X_IRQ_MSK_GSR_LEAD_ON_BIT | GH3X_IRQ_MSK_GSR_LEAD_OFF_BIT\
                                            )

#define __GH3X_WEAR_EVENT_MASK__       (    GH3X_IRQ_MSK_WEAR_ON_BIT | GH3X_IRQ_MSK_WEAR_OFF_BIT )

#define DRV_LIB_REG_CFG_EMPTY           (__GH3X_CFG_LIST_MAX_NUM__)    //there is no reg config array loaded into driver lib
#define DRV_LIB_REG_CFG_MIN_LEN         (0x5)     //minimum length of reg config array

int (*gpPrintfUser)(const char *, ...) = 0;
int (*gpSnprintfUser)(char *str, size_t size, const char *format, ...) = 0;

#if __GSENSOR_ENABLE__
GU16 gsensor_soft_fifo_buffer_index = 0;
GS16 gsensor_soft_fifo_buffer[__GSENSOR_DATA_BUFFER_SIZE__];
#endif
/// read data buffer
GU8 gpuchReadRawdataBuffer[__GH3X_RAWDATA_BUFFER_SIZE__];
/// read data buffer len
static GU16 gusReadRawdataLen = 0;
GS8 ia_valid = 0;
GU32 gunDemoFuncMode = 0;
GU8 guchDemoWorkMode = GH3X_DEMO_WORK_MODE_MCU_ONLINE;
void GH3X_SetDemoWorkMode(GU8 uchDemoWorkMode)
{
    guchDemoWorkMode = uchDemoWorkMode;
}
GU8 GH3X_GetDemoWorkMode(void)
{
    return guchDemoWorkMode;
}
/// driver lib demo init flag
static GU8 guchInitFlag = 0;
GU8 guchInterruptProNotFinishFlag = 0;
 
/// current gh3x reg config array index
GU8 guchRegCfgArrIndex = 0;
 
GU32 GH3X_GetDemoFuncMode(void)
{
    return gunDemoFuncMode;
}

void GH3X_GetConfigVersion(const STInitConfig *pstInitConfigParam)
{
    GU32 unCfgTimeStamp = 0;

    if (pstInitConfigParam->usConfigArrLen <= 1)
    {
        return;
    }
    else
    {
        for (GU16 usCnt = 0 ; usCnt < pstInitConfigParam->usConfigArrLen ; usCnt ++)
        {
            switch(pstInitConfigParam->pstRegConfigArr[usCnt].usRegAddr)
            {
            case 0x1006:
                unCfgTimeStamp |=  pstInitConfigParam->pstRegConfigArr[usCnt].usRegData;
                break;
            case 0x1008:
                unCfgTimeStamp |=  (pstInitConfigParam->pstRegConfigArr[usCnt].usRegData << 16);
                break;
            }
        }
    }
    if (unCfgTimeStamp != 0)
    {
        EXAMPLE_LOG("%s : Config Version : %x\n", __FUNCTION__, (unsigned int)unCfgTimeStamp);
    }
    else
    {
        EXAMPLE_LOG("%s : No Config Version !!!\n", __FUNCTION__);
    }
}

GCHAR* GH3X_GetFirmwareVersion(void)
{
    return GH3X_DEMO_VERSION_STRING;
}

GCHAR* GH3X_GetDemoVersion(void)
{
    return GH3X_DEMO_VERSION_STRING;
}

GCHAR *GH3X_GetBootloaderVersion(void)
{
    //return (GCHAR *)EVK_BOOT_LOADER_VERSION_ADDR;
    return GH3X_DEMO_VERSION_STRING;
}

GCHAR* GH3X_GetEvkBootloaderVersion(void)
{
#ifdef GOODIX_DEMO_PLATFORM
    return GetBootloaderVersion();
#else
    return GH3X_DEMO_VERSION_STRING;
#endif
}

void GH3X_FunctionCtrlModuleInit(void)
{
    gunDemoFuncMode = 0;
}

/**
 * @fn     int GH3X_DemoInit(void)
 *
 * @brief  Init GH3X_ module
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  error code
 */
int GH3X_DemoInit(void)
{
    GS8 schret = GH3X_RET_OK;

#if __EXAMPLE_LOG_TYPE__ == __EXAMPLE_LOG_METHOD_0__
    gpSnprintfUser = snprintf;
    EXAMPLE_LOG("__EXAMPLE_LOG_TYPE__ == __EXAMPLE_LOG_METHOD_0__\n");
#elif __EXAMPLE_LOG_TYPE__ == __EXAMPLE_LOG_METHOD_1__
    GH3X_RegisterPrintf(&gpPrintfUser);
#endif

    EXAMPLE_LOG("%s : Democode Version : %s\r\n", __FUNCTION__, GH3X_GetDemoVersion());
    EXAMPLE_LOG("%s : DrvLib Version : %s\r\n", __FUNCTION__, GH3X_GetDriverLibVersion());
#if (__GOODIX_ALGO_CALL_MODE__)
    GCHAR algo_version[150] = {0};
    for (int i = 0;i < GH3X_FUNC_OFFSET_MAX; i++)
    {
        GH3X_AlgoVersion(i, algo_version);
        EXAMPLE_LOG("%s : Algo Version : %s\r\n", __FUNCTION__, algo_version);
    }
#endif

    /* Step 1: init communicate interface. */
    if(0 == guchInitFlag)
    {
#if ( __GH3X_INTERFACE__ == __GH3X_INTERFACE_I2C__ )
        hal_gh3x_i2c_init();
        GH3X_RegisterI2cOperationFunc(__GH3X_I2C_DEVICE_ID__, hal_gh3x_i2c_write, hal_gh3x_i2c_read);
#else //__GH3X_INTERFACE__ == __GH3X_INTERFACE_SPI__
        hal_gh3x_spi_init();
#if (__GH3X_SPI_TYPE__ == __GH3X_SPI_TYPE_HARDWARE_CS__)
        GH3X_RegisterSpiHwCsOperationFunc(hal_gh3x_spi_write, hal_gh3x_spi_write_F1_and_read);
#else
        GH3X_RegisterSpiOperationFunc(hal_gh3x_spi_write,
                                      hal_gh3x_spi_read,
                                      hal_gh3x_spi_cs_ctrl);
#endif
#endif
    }

//     /* set hook */
    GH3X_RegisterHookFunc(gh3x_init_hook_func, 
                          gh3x_samplingstart_hook_func,
                          gh3x_samplingstop_hook_func, 
                          gh3x_get_rawdata_hook_func,
                          hal_gh3x_reset_pin_ctrl,
                          gh3x_bsp_delay_us,
                          gh3x_bsp_delay_ms);

#if __SUPPORT_HARD_RESET_CONFIG__
     if(0 == guchInitFlag)
     {
        hal_gh3x_reset_pin_init();
     }
    GH3X_HardReset();
#else
    GH3X_SoftReset();
#endif
    GH3X_DelayMs(7);
#if __SUPPORT_HARD_RESET_CONFIG__
    ia_valid = gh_ia_filter();
    if(GH3X_RET_OK != ia_valid)
    {
        EXAMPLE_LOG("ECG, BIA, GSR is not valid in low temperatrue!!!\r\n");
    }
#endif

#if __SUPPORT_GHEALTH_PROTOCOL__
    GhCommInit();
#endif

    /* Step 3: Init gh3x chip */
    guchRegCfgArrIndex = 0;
    schret = GH3X_Init(&gstCfgListArr[0]);
    if (GH3X_RET_OK == schret)
    {
        GH3X_GetConfigVersion(&gstCfgListArr[0]);
        EXAMPLE_LOG("Gh3xDemoInit:init success\r\n");
    }
    else
    {
        guchRegCfgArrIndex = DRV_LIB_REG_CFG_EMPTY;
        EXAMPLE_LOG("Gh3xDemoInit:init fail, error code: %d\r\n", schret);
        return schret;
    }
//    gh_read_tia_gain_error();
//    gh_read_led_drv_gain_error();
//    gh_read_dc_cancel_error();
//    gh_read_bg_cancel_error();
//    gh_read_cp_uid();
    GH3X_SetMaxNumWhenReadFifo(__GH3X_RAWDATA_BUFFER_SIZE__);
    GH3X_EnterLowPowerMode();

    /* Step 4: setup EX INT for GH3X INT pin */
#if (__INTERRUPT_PROCESS_MODE__ != __POLLING_INT_PROCESS_MODE__)
    if(0 == guchInitFlag)
    {
        hal_gh3x_int_init();
    }
#endif

#if (__GOODIX_ALGO_CALL_MODE__)
    GH3X_AlgoCallConfigInit(gpstFrameInfo, guchRegCfgArrIndex);
#endif

#if __GH_MULTI_SENSOR_EVENT_PRO_CONIG__
    GhMultSensorWearEventManagerInit();
    GhMultiSensorTimerInit();
#if __GSENSOR_MOVE_DET_ENABLE__
    GhGsMoveDetecterInit();
#endif
#endif

    guchInitFlag = 1;
    return (int)schret;
}
  
/**
 * @fn     GU8 GH3X_DemoInterruptProcess(void)
 *
 * @brief  Interrupt process of GH3X_.
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_DemoInterruptProcess(void)
{
    if(0 == guchInitFlag)
    {
        EXAMPLE_LOG("[%s]:gh3x is not init!!!\r\n", __FUNCTION__);
        return;
    }
    GU32 unGotEvent = 0;
    GU8 uchEventEx = 0;     //bit0:  0: object/no objiect   1: living/nonliving object
    guchInterruptProNotFinishFlag = 1;
    GS8 chLoopCnt = __INT_PROCESS_MAX_LOOP_CNT__;
    do{
        unGotEvent = 0; uchEventEx = 0;
        GH3X_ExitLowPowerMode();
        unGotEvent = GH3X_GetIrqStatus();
        EXAMPLE_LOG("[%s]:unGotEvent = 0x%08x\r\n", __FUNCTION__,unGotEvent);
        /* get fifo data */
        if ((unGotEvent & (GH3X_IRQ_MSK_FIFO_WATERMARK_BIT | GH3X_IRQ_MSK_FIFO_FULL_BIT)) || 
                          (GH3X_GetSoftEvent() & GH3X_SOFT_EVENT_NEED_TRY_READ_FIFO))
        {
            EXAMPLE_LOG("get fifo data-------------------------------------------------------------------");
            GH3X_SetFifoFullFlag((unGotEvent & GH3X_IRQ_MSK_FIFO_FULL_BIT) != 0);
            GH3X_ReadFifodata(gpuchReadRawdataBuffer, &gusReadRawdataLen);
#if __FUNC_TYPE_GSR_ENABLE__
            gsr_current_adj(gpuchReadRawdataBuffer, gusReadRawdataLen);
#endif
            GhConvertFifoEndian(gpuchReadRawdataBuffer, gusReadRawdataLen);
            GhLedAgcProcess(gpuchReadRawdataBuffer, gusReadRawdataLen);
        }
        GH3X_EnterLowPowerMode();
        STSensorRawdata stSensorRawdata = 
        {
            #if __GSENSOR_ENABLE__
            gpuchReadRawdataBuffer,&gusReadRawdataLen,gsensor_soft_fifo_buffer,&gsensor_soft_fifo_buffer_index
            #else
            gpuchReadRawdataBuffer,&gusReadRawdataLen,0,0
            #endif
        };
        GhFifoDataHandle(unGotEvent, &stSensorRawdata);
        GhWearEventHandle(&unGotEvent,&uchEventEx);
#if (__SUPPORT_GHEALTH_PROTOCOL__)
        GhUploadChipEvent(unGotEvent);
#endif
        EXAMPLE_LOG("[%s]:GH3X_GetSoftEvent = 0x%08x\r\n", __FUNCTION__,GH3X_GetSoftEvent());

    }while(GH3X_GetSoftEvent() && (chLoopCnt--) > 0);
    if(GH3X_GetSoftEvent())
    {
        GH3X_IntTimeOutHook();
    }
    guchInterruptProNotFinishFlag = 0;
}

void GhWearEventHandle(GU32* punGotEvent,GU8* puchEventEx)
{
    
#if (__SUPPORT_HARD_ADT_CONFIG__)

    #if __HARD_ADT_ALGO_CHECK_EN__
    //algo adt event
    if(GH3X_GetSoftEvent() & GH3X_SOFT_EVENT_WEAR_OFF_ALGO_ADT)
    {
        (*punGotEvent) |= GH3X_IRQ_MSK_WEAR_OFF_BIT;
        GH3X_ClearSoftEvent(GH3X_SOFT_EVENT_WEAR_OFF_ALGO_ADT);
    }
    else if(GH3X_GetSoftEvent() & GH3X_SOFT_EVENT_WEAR_ON_ALGO_ADT)
    {
        (*punGotEvent) |= GH3X_IRQ_MSK_WEAR_ON_BIT;
        GH3X_ClearSoftEvent(GH3X_SOFT_EVENT_WEAR_ON_ALGO_ADT);
    }
    #endif
    GH3X_WearEventProcess( punGotEvent,  puchEventEx);
    if(!((*punGotEvent) & GH3X_IRQ_MSK_WEAR_OFF_BIT)) //if there is not hard adt wear off , soft wear on/off event is valid
    {
        if((GH3X_GetSoftEvent() & GH3X_SOFT_EVENT_WEAR_OFF))  //nadt wear off
        {
            (*punGotEvent) |= GH3X_IRQ_MSK_WEAR_OFF_BIT;
            (*puchEventEx) |= GH3X_EVENT_EX_BIT_WEAR_LIVING_TYPE;
        }
        if((GH3X_GetSoftEvent() & GH3X_SOFT_EVENT_WEAR_ON))  //nadt wear on
        {
            (*punGotEvent) |= GH3X_IRQ_MSK_WEAR_ON_BIT;
            (*puchEventEx) |= GH3X_EVENT_EX_BIT_WEAR_LIVING_TYPE;
        }
    }
    GH3X_ClearSoftEvent(GH3X_SOFT_EVENT_WEAR_OFF | GH3X_SOFT_EVENT_WEAR_ON);
    if(*punGotEvent & __GH3X_WEAR_EVENT_MASK__)
    {
        GH3X_WearEventHook(punGotEvent, puchEventEx);
    }

#endif
    if(*punGotEvent & __GH3X_LEAD_EVENT_MASK__)
    {
#if (__FUNC_TYPE_ECG_ENABLE__)
        GH3X_LeadEventHook(punGotEvent);
#endif
    }
}

void GhLedAgcProcess(GU8* puchReadRawdataBuffer, GU16 usReadRawdataLen)
{
    #if (__SUPPORT_SOFT_AGC_CONFIG__)
    if(GH3X_DEMO_WORK_MODE_MCU_ONLINE == GH3X_GetDemoWorkMode())
    {
        GH3X_LedAgcProcess(puchReadRawdataBuffer, usReadRawdataLen); ////soft AGC
    }
    #endif
}

void GhConvertFifoEndian(GU8* puchReadRawdataBuffer, GU16 usReadRawdataLen)
{
    if(GH3X_DEMO_WORK_MODE_MCU_ONLINE == GH3X_GetDemoWorkMode() || GH3X_DEMO_WORK_MODE_ENGINEERING == GH3X_GetDemoWorkMode())
    {
        for (GU16 usSearchStartPosi = 0; usSearchStartPosi < usReadRawdataLen; usSearchStartPosi += GH3X_FIFO_RAWDATA_SIZE)
        {
            GH3X_ChangeFifoEndian(&puchReadRawdataBuffer[usSearchStartPosi],GH3X_FIFO_RAWDATA_SIZE);
        }
    }
}

void GhUploadChipEvent(GU32 unGotEvent)
{
#if __GH_MULTI_SENSOR_EVENT_PRO_CONIG__
    if(GhMultSensorWearEventManagerIsEnable())   //is in multi-sensor wear det mode
    {
        unGotEvent &= ~(GH3X_IRQ_MSK_WEAR_ON_BIT);   //ignore wear on event
    }
#endif
    if (((unGotEvent & __GH3X_EVENT_PROTOCOL_MASK__) != 0) && 
            ((((GH3X_GetDemoFuncMode() != 0)
#if __GH_MULTI_SENSOR_EVENT_PRO_CONIG__
            ||(GhMultSensorWearEventManagerIsEnable())
#endif
            ) && GH3X_DEMO_WORK_MODE_EVK != GH3X_GetDemoWorkMode()) || 
            (GH3X_DEMO_WORK_MODE_EVK == GH3X_GetDemoWorkMode()))
        )
    {
#if __SUPPORT_GHEALTH_PROTOCOL__
        GH3X_UprotocolSendChipEvent((unGotEvent & __GH3X_EVENT_PROTOCOL_MASK__));
#endif
    }
}

void GhFifoDataHandle(GU32 unGotEvent, STSensorRawdata* pstSensorRawdata)
{
    if(pstSensorRawdata == 0 || pstSensorRawdata->puchGhRawdataArr == 0 ||
        pstSensorRawdata->pusGhRawdataCnt == 0 || *pstSensorRawdata->pusGhRawdataCnt == 0)
    {
        return;
    }
    if(GH3X_DEMO_WORK_MODE_EVK == GH3X_GetDemoWorkMode() || GH3X_DEMO_WORK_MODE_APP == GH3X_GetDemoWorkMode())
    {
#if (__SUPPORT_GHEALTH_PROTOCOL__)
        GH3X_UprotocolSendFifoData(pstSensorRawdata->puchGhRawdataArr, *pstSensorRawdata->pusGhRawdataCnt,unGotEvent & GH3X_IRQ_MSK_FIFO_FULL_BIT);
#endif
        //GH3X_DemoEcgUncompress(gpuchReadRawdataBuffer, gusReadRawdataLen, unGotEvent & GH3X_IRQ_MSK_FIFO_FULL_BIT);
        *pstSensorRawdata->pusGhRawdataCnt = 0;
    }
    if ((*pstSensorRawdata->pusGhRawdataCnt > 0) && 
        (GH3X_DEMO_WORK_MODE_ENGINEERING == GH3X_GetDemoWorkMode() ||
        GH3X_DEMO_WORK_MODE_MCU_ONLINE == GH3X_GetDemoWorkMode()))
    {
#if (__GSENSOR_ENABLE__)
        if (GH3X_GetGsensorEnableFlag() && pstSensorRawdata->psGsRawdataArr != 0)
        {
            hal_gsensor_drv_get_fifo_data(pstSensorRawdata->psGsRawdataArr, pstSensorRawdata->pusGsRawdataCnt);
            GH3X_NormalizeGsensorSensitivity(pstSensorRawdata->psGsRawdataArr, *pstSensorRawdata->pusGsRawdataCnt);
        }
#endif
        GH3X_FunctionProcess(pstSensorRawdata);
        *pstSensorRawdata->pusGhRawdataCnt = 0;
    }
    *pstSensorRawdata->pusGhRawdataCnt = 0;
}

/**
 * @fn     GS8 GH3X_DemoArrayCfgSwitch(GU8 uchArrayCfgIndex)
 *
 * @brief  array cfg switch (call by user)
 *
 * @attention   you should switch array cfg manually before calling GH3X_DemoStartSampling
 *
 * @param[in]   uchArrayCfgIndex    0: gh3x_reglist0    1: gh3x_reglist0 ...
 * @param[out]  None
 *
 * @return  GH3X_RET_OK:switch application mode success
 * @return  GH3X_RET_RESOURCE_ERROR:can't find corresponding reg config array
 */
GS8 GH3X_DemoArrayCfgSwitch(GU8 uchArrayCfgIndex)
{
    GS8  chRet = GH3X_RET_OK;

    if(uchArrayCfgIndex < __GH3X_CFG_LIST_MAX_NUM__)
    {
        GH3X_DemoSamplingControl(0xFFFFFFFF, UPROTOCOL_CMD_STOP);
        GH3X_ExitLowPowerMode();        
        GH3X_WriteReg(0x0FC0, 0xaa55);

#if __SUPPORT_HARD_RESET_CONFIG__
        GH3X_HardReset();
#else
        GH3X_SoftReset();
#endif
        GH3X_DelayMs(7);  
        GU16 reset_reg = GH3X_ReadReg(0x0FC0);
        if(reset_reg != 0x0501)
        {
            EXAMPLE_LOG("reset chip fail !!.\r\n");
            return GH3X_RET_COMM_ERROR;
        }
        guchRegCfgArrIndex = uchArrayCfgIndex;
        if (GH3X_RET_OK == GH3X_Init(gstCfgListArr + uchArrayCfgIndex))
        {
            EXAMPLE_LOG("cfg%d switch success !!!.\r\n",(int)uchArrayCfgIndex);
        }
        else
        {
            guchRegCfgArrIndex = DRV_LIB_REG_CFG_EMPTY;
            chRet = GH3X_RET_RESOURCE_ERROR;
            EXAMPLE_LOG("Error ! cfg%d switch fail !!!\r\n",(int)uchArrayCfgIndex);
        }
        GH3X_EnterLowPowerMode();        
    }
    else
    {
        EXAMPLE_LOG("Error! uchArrayCfgIndex(%d) is invalid !!!\r\n",(int)uchArrayCfgIndex);
        chRet = GH3X_RET_RESOURCE_ERROR;
    }
    return chRet;
}
/**
 * @fn     void GH3X_SetSampleEnReg(GU32 unFuncMode, GU8 uchStart)
 *
 * @brief  Set register to start sample
 *
 * @attention   None
 *
 * @param[in]   unFuncMode   function mode
 * @param[in]   uchStart     0:stop  1:start
 * @param[out]  None
 *
 * @return  None
 * @return  None
 */
void GH3X_SetSampleEnReg(GU32 unFuncMode, GU8 uchStart)
{
    GU64 unSlotCfg = 0;
    GU8 uchEcgEnBit= 0;
    GU8 uchBiaEnBit= 0;
    GU8 uchGsrEnBit= 0;
    for(GU8 uchFunCnt = 0; uchFunCnt < GH3X_FUNC_OFFSET_MAX; uchFunCnt ++)
    {
        if(gpstFrameInfo[uchFunCnt])
        {
            if(gunDemoFuncMode & (((GU32)1)<< uchFunCnt))
            {
                GH3x_CalFunctionEnBit(gpstFrameInfo[uchFunCnt],&unSlotCfg,0,0,0);
            }
            if(unFuncMode & (((GU32)1)<< uchFunCnt))
            {
                GH3x_CalFunctionEnBit(gpstFrameInfo[uchFunCnt],0,&uchEcgEnBit,&uchBiaEnBit,&uchGsrEnBit);
            }
        }
    }
    //IA EN
    GH3x_SetIaEn(uchEcgEnBit,uchBiaEnBit,uchGsrEnBit,uchStart);
    //SLOT EN
    GH3x_SetSlotEn(unSlotCfg,uchStart);
}

/**
 * @fn     void GH3X_DemoSamplingControl(EMUprotocolParseCmdType emSwitch)
 *
 * @brief  Start/stop sampling of gh3x and sensor
 *
 * @attention   None
 *
 * @param[in]   unFuncMode      function that will be started or stopped
 * @param[in]   emSwitch        stop/start sampling
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_DemoSamplingControl(GU32 unFuncMode, EMUprotocolParseCmdType emSwitch)
{
    if(unFuncMode == 0)
    {
        return;
    }
    EXAMPLE_LOG("[DemoSamplingControl]unFuncMode = 0x%08x , emSwitch = %d\r\n", unFuncMode, emSwitch);
    if(ia_valid != 0)
    {
        unFuncMode &= (~(GH3X_FUNCTION_BIA));
        unFuncMode &= (~(GH3X_FUNCTION_GSR));
        unFuncMode &= (~(GH3X_FUNCTION_ECG));
    }
    GH3X_ExitLowPowerMode();
    GU32 unStartFunc = 0, unStopFunc =0;
    for(GU8 uchFunCnt = 0; uchFunCnt < GH3X_FUNC_OFFSET_MAX; uchFunCnt ++)
    {
        // EXAMPLE_LOG("forforforfor------------------------------------------------");
        if(gpstFrameInfo[uchFunCnt] && (unFuncMode & (((GU32)1)<< uchFunCnt)))
        {
            // EXAMPLE_LOG("gpstFrameInfo:----------------------------------------------------------");
            if((UPROTOCOL_CMD_START == emSwitch)&&((gunDemoFuncMode & (((GU32)1)<< uchFunCnt)) != (((GU32)1)<< uchFunCnt)))
            {
                if(uchFunCnt == GH3X_FUNC_OFFSET_BIA)
                {
                #if __FUNC_TYPE_BIA_ENABLE__
                    GH3X_BiaStart();//only start bia state machine , bia sample start by function start; so as stop
                #endif
                }
                if(uchFunCnt == GH3X_FUNC_OFFSET_GSR)
                {
                #if __FUNC_TYPE_GSR_ENABLE__
                    gsr_current_adj_init();
                #endif
                }
                #if (__SUPPORT_SOFT_AGC_CONFIG__)
                int workmode = GH3X_GetDemoWorkMode();
                // EXAMPLE_LOG("gunDemoFuncMode:%d----------------------------------------------------------",gunDemoFuncMode);
                // EXAMPLE_LOG("GH3X_GetDemoWorkMode:%d--------------------------------------------------------",workmode);
                if(gunDemoFuncMode == 0 && GH3X_GetDemoWorkMode() == GH3X_DEMO_WORK_MODE_MCU_ONLINE)
                {
                    GH3X_LedAgcInit();
                }
                #endif
                EXAMPLE_LOG("GH3X_FunctionStart------------------------------------");
                GH3X_FunctionStart(gpstFrameInfo[uchFunCnt]);
                GH3X_InitResample(gpstFrameInfo[uchFunCnt]);
                GH3X_SamplingControlHook(unFuncMode,emSwitch);
                gunDemoFuncMode |= (((GU32)1)<< uchFunCnt);
                unStartFunc |= (((GU32)1)<< uchFunCnt);
            }
            else if ((UPROTOCOL_CMD_STOP == emSwitch)&&((gunDemoFuncMode & (((GU32)1)<< uchFunCnt)) == (((GU32)1)<< uchFunCnt)))
            {
                if(uchFunCnt == GH3X_FUNC_OFFSET_BIA)
                {
                #if __FUNC_TYPE_BIA_ENABLE__
                    GH3X_BiaStop();
                #endif
                }
                if(uchFunCnt == GH3X_FUNC_OFFSET_GSR)
                {
                #if __FUNC_TYPE_GSR_ENABLE__
                    gsr_current_adj_deinit();
                #endif
                }
                GH3X_FunctionStop(gpstFrameInfo[uchFunCnt]);
                GH3X_SamplingControlHook(unFuncMode,emSwitch);
                gunDemoFuncMode &= (~(((GU32)1)<< uchFunCnt));
                unStopFunc |= (((GU32)1)<< uchFunCnt);
            }
            if (gpstFrameInfo[uchFunCnt] && gpstFrameInfo[uchFunCnt]->pstGsData) {
                gpstFrameInfo[uchFunCnt]->pstGsData->uchEnableFlag = GH3X_GetGsensorEnableFlag();
}
        
        }
    }
    if(UPROTOCOL_CMD_START == emSwitch && unStartFunc != 0)
    {
        GH3X_SetSampleEnReg(unStartFunc,1);
        if (GH3X_GetGsensorEnableFlag() && (GH3X_NO_FUNCTION == (gunDemoFuncMode &(~unFuncMode))))
        {
            hal_gsensor_start_cache_data();
        }
#if __FIFO_MONITOR_EN__
        Gh3xFifoMonitorInfoInit();
#endif
    }
    else if(UPROTOCOL_CMD_STOP == emSwitch && unStopFunc != 0)
    {
        GH3X_SetSampleEnReg(unStopFunc,0);
        if (GH3X_GetGsensorEnableFlag()  && (GH3X_NO_FUNCTION == gunDemoFuncMode) 
#if (__GH_MULTI_SENSOR_EVENT_PRO_CONIG__) && (__GSENSOR_MOVE_DET_ENABLE__)
        && (0 == GhGsMoveDetecterIsEnable())
#endif
        )
        {
            hal_gsensor_stop_cache_data();
        }
#if __FIFO_MONITOR_EN__
        Gh3xFifoMonitorInfoInit();
#endif
        EXAMPLE_LOG("GH3X_SetSoftEvent-------------------------------------------------------------");
        GH3X_SetSoftEvent(GH3X_SOFT_EVENT_NEED_TRY_READ_FIFO);
    }
    EXAMPLE_LOG("gunDemoFuncMode :%d---------------------------------------------------------- ",gunDemoFuncMode);
    if(gunDemoFuncMode == 0)
    {
#if __FIFO_MONITOR_EN__
        Gh3xFifoMonitorInfoDeInit();
#endif
    }
    EXAMPLE_LOG("Current gunDemoFuncMode= 0x%x\r\n",gunDemoFuncMode);
    if(GH3X_GetSoftEvent()&&(0 == guchInterruptProNotFinishFlag))   //avoid nesting
    {
    EXAMPLE_LOG("GH3X_GetSoftEvent= 0x%x\r\n",gunDemoFuncMode);
#ifdef GOODIX_DEMO_PLATFORM
        SendMsgToCardiff(OP_CHIP_HW_INT, NULL, 0,0);
#else
        EXAMPLE_LOG("GH3X_DemoInterruptProcess----------------------------------------------------------");
        GH3X_DemoInterruptProcess();
#endif
    }
    GH3X_EnterLowPowerMode();
}

/**
 * @fn     void GH3X_DemoStartSampling()
 *
 * @brief  Start sampling of GH3X_
 *
 * @attention   None
 *
 * @param[in]   unFuncMode      function mode that will start
 *                              GH3X_FUNCTION_ADT/GH3X_FUNCTION_HR/GH3X_FUNCTION_SPO2/GH3X_FUNCTION_ECG and etc.
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_DemoStartSamplingInner(GU32 unFuncMode, GU8 uchSwitchEnable)
{
    GH3X_DemoSamplingControl(unFuncMode, UPROTOCOL_CMD_START);
}

/**
 * @fn     void GH3X_DemoStartSampling()
 *
 * @brief  Start sampling of GH3X_
 *
 * @attention   None
 *
 * @param[in]   unFuncMode      function mode that will start
 *                              GH3X_FUNCTION_ADT/GH3X_FUNCTION_HR/GH3X_FUNCTION_SPO2/GH3X_FUNCTION_ECG and etc.
 * @param[out]  None
 *
 * @return  None
 */

void GH3X_DemoStartSampling(GU32 unFuncMode)
{
    GH3X_DemoStartSamplingInner(unFuncMode, 0);
#if (__GOODIX_ALGO_CALL_MODE__)
    GH3X_AlgoInit(unFuncMode);
#endif
}

/**
 * @fn     void GH3X_DemoStartSamplingWithCfgSwitch(GU32 unFuncMode, GU8 uchArrayCfgIndex)
 *
 * @brief  Start sampling of GH3X_
 *
 * @attention   None
 *
 * @param[in]   unFuncMode      function mode that will start
 *                              GH3X_FUNCTION_ADT/GH3X_FUNCTION_HR/GH3X_FUNCTION_SPO2/GH3X_FUNCTION_ECG and etc.
 * @param[in]   uchArrayCfgIndex      cfg id
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_DemoStartSamplingWithCfgSwitch(GU32 unFuncMode, GU8 uchArrayCfgIndex)
{
    if (uchArrayCfgIndex == DRV_LIB_REG_CFG_EMPTY)
    {
        EXAMPLE_LOG("[%s]:Array None!!!\r\n", __FUNCTION__);
    }
    GH3X_DemoArrayCfgSwitch(uchArrayCfgIndex);
    GH3X_DemoStartSamplingInner(unFuncMode, 1);
}

 

/**
 * @fn     void GH3X_DemoStopSampling(GU32 unFuncMode)
 *
 * @brief  Stop sampling of GH3X_
 *
 * @attention   None
 *
 * @param[in]   unFuncMode      function mode that will start
 *                              GH3X_FUNCTION_ADT/GH3X_FUNCTION_HR/GH3X_FUNCTION_SPO2/GH3X_FUNCTION_ECG and etc.
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_DemoStopSamplingInner(GU32 unFuncMode)
{
    EXAMPLE_LOG("[%s]:unFuncMode = 0x%X\r\n", __FUNCTION__, (int)unFuncMode);
    GH3X_DemoSamplingControl(unFuncMode, UPROTOCOL_CMD_STOP);
}

void GH3X_DemoStopSampling(GU32 unFuncMode)
{
    GH3X_DemoStopSamplingInner(unFuncMode);
#if (__GOODIX_ALGO_CALL_MODE__)
    GH3X_AlgoDeinit(unFuncMode);
#endif
}

 

void GH3X_DemoSlotControl(GU32 uchSlotCfgNums, GU8 uchStart)
{
    GH3X_ExitLowPowerMode();
    GH3X_SlotControl(uchSlotCfgNums,uchStart);
    GH3X_EnterLowPowerMode();
}
void GH3X_DemoSlotSampleRate(GU32 uchSlotCfgNums, GU32 unSampleRate)
{
    GH3X_ExitLowPowerMode();
    GU8 uchSlotStart = GH3X_ReadRegBitField(8,0,0);
    GU16 usSlotEn[2];
    GH3X_ReadRegs(0xC20, usSlotEn, 2);
    GH3X_WriteRegBitField(0x0008,0, 0,0);// stop
    GH3X_WriteReg(0xC20,0); // slot en
    GH3X_WriteReg(0xC22,0); // slot en
    GU32 uchSlotCfgNum = 0;
    while(uchSlotCfgNums != 0)
    {
        if(uchSlotCfgNums & (1))
        {
            GH3X_SetSlotSampleRate(uchSlotCfgNum,unSampleRate);
        }
        uchSlotCfgNums = uchSlotCfgNums >> 1;
        uchSlotCfgNum++;
    }
    GH3X_WriteRegs(0xC20,usSlotEn, 2);// slot en
    GH3X_WriteRegBitField(0x0008,0, 0,uchSlotStart);//ppg top start
    GH3X_EnterLowPowerMode();
}

void GH3X_DemoSlotCuurent(GU32 uchSlotCfgNums, double currentmA)
{
    GH3X_ExitLowPowerMode();
    GU32 uchSlotCfgNum = 0;
    while(uchSlotCfgNums != 0)
    {
        if(uchSlotCfgNums & (1))
        {
            GH3X_SetLedCurrent(uchSlotCfgNum, currentmA);
        }
        uchSlotCfgNums = uchSlotCfgNums >> 1;
        uchSlotCfgNum++;
    }
    GH3X_EnterLowPowerMode();
}

void GhDemoEngineeringModeStart(void)
{
    //0. load ppg config
    GH3X_DemoArrayCfgSwitch(0);
    //1. set work mode
    GH3X_SetDemoWorkMode(GH3X_DEMO_WORK_MODE_ENGINEERING);
    //2. change ppg parameters
    GH3X_ExitLowPowerMode();
    GhHardAgcDisable();
    GH3X_SetLedCurrent(0, 50);
    GH3X_SetSlotSampleRate(0, 100);
    GH3X_SetSlotIntTime(0, 3);
    GH3X_SetSlotGain(0, 1, 3);
    //3. start sample 
    GH3X_DemoStartSampling(GH3X_FUNCTION_HR);
}

void GhDemoEngineeringModeStop(void)
{
    GH3X_DemoStopSampling(0xFFFFFFFF);
    GH3X_SetDemoWorkMode(GH3X_DEMO_WORK_MODE_MCU_ONLINE);
    GH3X_DemoArrayCfgSwitch(0);
}

#if __GH_MULTI_SENSOR_EVENT_PRO_CONIG__
void GH3X_MoveDetectTimerHandler(GU32 unTimeIntervalMs)
{
    #if (__GSENSOR_MOVE_DET_ENABLE__)
    //check GS
    hal_gsensor_drv_get_fifo_data(gsensor_soft_fifo_buffer , &gsensor_soft_fifo_buffer_index);
    GH3X_NormalizeGsensorSensitivity(gsensor_soft_fifo_buffer ,gsensor_soft_fifo_buffer_index);
    GhMoveDetectByGsData((GS16*)(gsensor_soft_fifo_buffer), gsensor_soft_fifo_buffer_index);
    #endif
}
#endif
/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
