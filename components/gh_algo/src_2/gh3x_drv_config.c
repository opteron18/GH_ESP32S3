/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x_drv_control_ex.c
 *
 * @brief   gh3x extension control functions, using function map like evk
 *
 * @version ref gh3x_drv_version.h
 *
 */


#include <stdio.h>
#include "gh3x_drv_version.h"
#include "gh3x_drv.h"
#include "gh3x_drv_interface.h"
#include "gh3x_drv_control.h"
#include "gh3x_drv_config.h"
#include "gh3x_drv_registers.h"
#include "gh3x_demo_config.h"
#include "gh3x_demo_algo_config.h"
#include "gh3x_soft_led_agc.h"
#if defined( GOODIX_DEMO_PLATFORM) && defined( GR5515_SK)
#include "calendar.h"
#endif


extern const STFrameInfo * const gpstFrameInfo[GH3X_FUNC_OFFSET_MAX];
extern void GH3X_WriteAlgConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue);
extern void GH3X_AlgConfigDeinit(void);

extern GS32 gunHrParamsArr[];
extern STAlgoChnlMap gstHbaAlgoChnlMap;
extern GS32 gnHrvParamsArr[];
extern GS32 gnSpo2ParamssArr[];
extern STAlgoChnlMap gstSpo2AlgoChnlMap;
extern GS32 gnEcgParamsArr[];
extern GS32 gnBiaPInfo[];
extern STAlgoChnlMap gstBiaAlgoChnlMap;
extern STSoftAgcCfg gstSoftAgcCfg;
GU8 g_usCurrentConfigFlag = 1;
GU16 g_usInfoConfigArrVer = 0;
GU16 g_usInfoConfigToolVer = 0;
GU16 g_usInfoProjectId = 0;
GU32 g_unInfoCreatedTimestamp = 0;
GU16 g_usFifoPackageMode = 0;
GU16 g_usNormalFifoWaterLine = 0;
GU16 g_usAdtFifoWaterLine = 0;
GU8 g_uchGsensorVendorId = 0;
GU16 g_usGsensorSampleRate = 0;
GU8 g_uchGsensorCtrl = 0;
GU8 g_uchCapCtrl = 0;
GU16 g_usCapSampleRate = 0;
GU8	g_uchTempCtrl = 0;
GU16 g_usTempSampleRate = 0;
GU16 g_usBiaMeasureMode = 0;
GU16 g_usBiaCalibrateRx = 0;
GU16 g_usBiaCapacitanceTxp = 0;
GU16 g_usBiaCapacitanceTxn = 0;
GU16 g_usBiaCapacitanceRxp = 0;
GU16 g_usBiaCapacitanceRxn = 0;
GU32 g_unCurrentTime;
/* version */
GCHAR *GH3X_GetVirtualRegVersion(void)
{
    return (GCHAR *)GH3X_VIRTUAL_REG_VERSION_STRING;
}

void GH3X_FunctionFrameInfoInit(GU32 unFuncid)
{
    for (int i = 0; i < GH3X_FUNC_OFFSET_MAX; i++)
    {
        if((unFuncid & (1 << i)) && gpstFrameInfo[i] != 0)
        {
            gpstFrameInfo[i]->pstGh3xData->uchChnlNum = 0;
            gpstFrameInfo[i]->pstGh3xData->uchChnlIndex = 0;
            if(gpstFrameInfo[i]->pstFlagInfo != 0 && gpstFrameInfo[i]->pstFlagInfo->punFlag != 0)
            {
                GH3X_Memset(gpstFrameInfo[i]->pstFlagInfo->punFlag,0,GH3X_FRAME_FLAG_NUM*sizeof(GU32));
            }
        }
    }
}

void GH3X_TargetFunctionInit(void)
{
    GU32 unTargetFuncMode = 0;
    for (int i = 0; i < GH3X_FUNC_OFFSET_MAX; i++)
    {
        if(gpstFrameInfo[i] != 0 && gpstFrameInfo[i]->pstGh3xData->uchChnlNum != 0)
        {
            unTargetFuncMode |= (1 << i);
        }
    }
    GH3X_SetProtocolTargetFunction(unTargetFuncMode);
}

void GH3X_WriteAgcConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
#if (__SUPPORT_SOFT_AGC_CONFIG__)
    GU16 usValIndex;
    switch (usVirtualRegAddr)
    {
    case   GH3X_AGC_ADJUST_EN_ADDR                  :
    case   GH3X_BG_CANCEL_ADJUST_EN_ADDR            :
    case   GH3X_AGC_AMB_SLOT_CTRL_ADDR              :
    case   GH3X_AGC_GAIN_LIMIT_ADDR                 :
    case   GH3X_AGC_SPO2REDSLOTCFGEN_ADDR           :
    case   GH3X_AGC_SPO2IRSLOTCFGEN_ADDR            :
    case   GH3X_AGC_CFG0_TRIG_THD_H_MSB_16_ADDR     :
    case   GH3X_AGC_CFG0_TRIG_THD_L_MSB_16_ADDR     :
    case   GH3X_AGC_CFG0_RESTRAIN_THD_H_MSB_16_ADDR :
    case   GH3X_AGC_CFG0_RESTRAIN_THD_L_MSB_16_ADDR :
        usValIndex = (usVirtualRegAddr - GH3X_AGC_ADJUST_EN_ADDR) / GH3X_REG_ADDR_SIZE;
        GH3X_LedAgcPramWrite(usVirtualRegValue, usValIndex);
        break;
    default: // do nothing
        break;
    }
#endif
}
 
void GH3X_WriteAdtDrvConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
    switch (usVirtualRegAddr)
    {
    case 0x7000:
      break;
     
    default:
       
      break;
    }

}

void GH3X_WriteBiaDrvConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
    switch (usVirtualRegAddr)
    {
    case 0xA000:
      g_usBiaMeasureMode = usVirtualRegValue;
      break;
    case 0xA002:
      g_usBiaCalibrateRx = usVirtualRegValue;
      break;
    case 0xA004:
      g_usBiaCapacitanceTxp = usVirtualRegValue;
      break;
     case 0xA006:
      g_usBiaCapacitanceTxn = usVirtualRegValue;
      break;	
    case 0xA008:
      g_usBiaCapacitanceRxp = usVirtualRegValue;
      break;
    case 0xA00A:
      g_usBiaCapacitanceRxn = usVirtualRegValue;
      break;

    default:
       
      break;
    }

}


# if 1
 void GH3X_WriteDrvConfigWithVirtualReg(GU32 unFunctionID, GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
    switch (unFunctionID)
    {
    case GH3X_FUNC_OFFSET_ADT:
        GH3X_WriteAdtDrvConfigWithVirtualReg(usVirtualRegAddr, usVirtualRegValue);
        break;
    case GH3X_FUNC_OFFSET_ECG:
        //GH3X_WriteEcgDrvConfigWithVirtualReg(usVirtualRegAddr, usVirtualRegValue);
        break;
		case GH3X_FUNC_OFFSET_BIA:
        GH3X_WriteBiaDrvConfigWithVirtualReg(usVirtualRegAddr, usVirtualRegValue);
        break;
    default:
        break;
    }
}
 #endif


 void GH3X_WriteFsConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
 {
	 GU8 uchFuncOffsetIndex =  (usVirtualRegAddr - GH3X_ADT_FS_ADDR) / 2;
	 if(gpstFrameInfo[uchFuncOffsetIndex] != 0)
	 {
		 gpstFrameInfo[uchFuncOffsetIndex] ->pstGh3xData->usSampleRate  = (usVirtualRegValue);
	 }
 }

 

 void GH3X_WriteChnlMapConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
 {
	 GU16 usValIndex = 0;
	 usVirtualRegAddr = usVirtualRegAddr - GH3X_ADT_CHMAP_CNT_ADDR;
	 switch (usVirtualRegAddr % GH3X_CHNLMAP_OFFSET)
	 {
	 case 0:
		 GH3xSetFunctionChnlNum(gpstFrameInfo[usVirtualRegAddr / GH3X_CHNLMAP_OFFSET],GH3X_GET_LOW_BYTE_FROM_WORD(usVirtualRegValue));
		 break;
	 default:
		 usValIndex = (GU16)((usVirtualRegAddr % GH3X_CHNLMAP_OFFSET)/2 - 1);
		 if (usValIndex < GH3X_CHANNEL_MAP_MAX_CH)
		 {
			 GH3xSetFunctionChnlMap(gpstFrameInfo[usVirtualRegAddr / GH3X_CHNLMAP_OFFSET],usValIndex,(usVirtualRegValue));
		 }
		 break;
	 }
 }



 /**
 * @fn     GH3X_WriteSwConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
 *
 * @brief  Write software param config with virtual reg
 *
 * @attention   Virtual reg addr has del control bits, so reg addr is [0:11] valid.
 *
 * @param[in]   usVirtualRegAddr        virtual reg addr
 * @param[in]   usVirtualRegValue       virtual reg value
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_WriteSwConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
    // GH3X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
    if (usVirtualRegAddr < GH3X_FIFO_CTRL_ADDR) // config arr info         0x1F
    {
        switch (usVirtualRegAddr)
        {
        case GH3X_CFG_VER_ADDR:
            g_usInfoConfigArrVer = usVirtualRegValue;
            break;
        case GH3X_CFG_TOOL_VER_ADDR:
            g_usInfoConfigToolVer = usVirtualRegValue;
            break;
        case GH3X_PROJECT_ID_ADDR:
            g_usInfoProjectId = usVirtualRegValue;
            break;
        case GH3X_TIMESTAMP_L_ADDR:
            g_unInfoCreatedTimestamp = GH3X_MAKEUP_DWORD2(GH3X_GET_HIGH_WORD_FROM_DWORD(g_unInfoCreatedTimestamp),
                                                                usVirtualRegValue);
            break;
        case GH3X_TIMESTAMP_H_ADDR:
            g_unInfoCreatedTimestamp = GH3X_MAKEUP_DWORD2(usVirtualRegValue,
                                                            GH3X_GET_LOW_WORD_FROM_DWORD(g_unInfoCreatedTimestamp));
            break;
#if defined( GOODIX_DEMO_PLATFORM) && defined( GR5515_SK)
        case GH3X_CURRENT_TIMESTAMP_L_ADDR:
            g_unCurrentTime = GH3X_MAKEUP_DWORD2(GH3X_GET_HIGH_WORD_FROM_DWORD(g_unCurrentTime),
                                                                usVirtualRegValue);
            break;
        case GH3X_CURRENT_TIMESTAMP_H_ADDR:
            g_unCurrentTime = GH3X_MAKEUP_DWORD2(usVirtualRegValue,
                                                            GH3X_GET_LOW_WORD_FROM_DWORD(g_unCurrentTime));
#if 0//!defined(GR5526_SK)
            set_timestamp(g_unCurrentTime);
#endif
            break;
#endif
        case GH3X_REINIT_PARAM_ADDR:
            GH3X_FunctionFrameInfoInit(0xFFFFFFFF);
            break;
        default: // do nothing
            break;
        }
    } // end of if (usVirtualRegAddr < GH3X2X_VINFO_END_REG_ADDR) 
    else if (usVirtualRegAddr < GH3X_G_SENSOR_CFG_ADDR) //  fifo ctrl info
    {
        switch (usVirtualRegAddr)  
        {
        case GH3X_FIFO_PACKAGE_SEND_MODE_ADDR:
            //g_uchFifoPackageMode = usVirtualRegValue;
            break;
		case GH3X_ADT_FIFO_WATER_LINE_ADDR:
            g_usAdtFifoWaterLine = usVirtualRegValue;
			break;
	    case GH3X_NORMAL_FIFO_WATER_LINE_ADDR:
		    g_usNormalFifoWaterLine = usVirtualRegValue;
			break;
        default: 
            break;
        }
    } 
    else if (usVirtualRegAddr < GH3X_CAP_CFG_ADDR) // gSensor info
    {
        switch (usVirtualRegAddr)  
       {
        case GH3X_GSENSOR_VENDOR_ID_ADDR:
    		g_uchGsensorVendorId = GH3X_GET_LOW_BYTE_FROM_WORD(usVirtualRegValue);
    		break;
    	case GH3X_GSENSOR_SAMPLE_RATE_ADDR:
    		g_usGsensorSampleRate = usVirtualRegValue;
    		break;
    	case GH3X_GSENSOR_CTRL_ADDR:
    		g_uchGsensorCtrl =  GH3X_GET_LOW_BYTE_FROM_WORD(usVirtualRegValue);
    		break;
    	default: 
		   break;
        }
    }
    else if (usVirtualRegAddr < GH3X_TEMP_CFG_ADDR) //cap cfg
    {
        switch (usVirtualRegAddr)
        {
        case GH3X_CAP_CTRL_ADDR:
            g_uchCapCtrl = GH3X_GET_LOW_BYTE_FROM_WORD(usVirtualRegValue);
            break;
		case GH3X_CAP_SAMPLE_RATE_ADDR:
			g_usCapSampleRate = usVirtualRegValue;
        default: 
            break;
        }
    } 
    else if (usVirtualRegAddr < GH3X_SOFT_AGC_PARA_ADDR) // temp config
    {
        switch (usVirtualRegAddr)
        {
        case GH3X_TEMP_CTRL_ADDR:
            g_uchTempCtrl = GH3X_GET_LOW_BYTE_FROM_WORD(usVirtualRegValue);
            break;
		case GH3X_TEMP_SAMPLE_RATE_ADDR:
			g_usTempSampleRate = usVirtualRegValue;
        default: 
            break;
        }
    } 
    else if (usVirtualRegAddr < GH3X_FS_PARA_ADDR) //agc config
    {
		GH3X_WriteAgcConfigWithVirtualReg(usVirtualRegAddr, usVirtualRegValue);
    } 
    else if (usVirtualRegAddr < GH3X_CHNL_MAP_ADDR) //fs config
    {
		GH3X_WriteFsConfigWithVirtualReg(usVirtualRegAddr, usVirtualRegValue);
    } 
    else if (usVirtualRegAddr < GH3X_REG_IS_VIRTUAL0X7_BIT)  //channel map
    {
        GH3X_WriteChnlMapConfigWithVirtualReg(usVirtualRegAddr, usVirtualRegValue);//////////////////
    }
}




/**
 * @fn     GS8 GH3X_WriteAlgorithmConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
 *
 * @brief  Write algorithm param config with virtual reg
 *
 * @attention   Virtual reg addr has del control bits, so reg addr is [0:11] valid.
 *
 * @param[in]   usVirtualRegAddr        virtual reg addr
 * @param[in]   usVirtualRegValue       virtual reg value
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_WriteFunctionConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
    // GH3X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
    GU32 unFunctionID = (usVirtualRegAddr - GH3X_REG_IS_VIRTUAL0X7_BIT) / GH3X_VREG_FUNCTION_OFFSET;
    if (usVirtualRegAddr >= GH3X_ADT_DRV_CFG_ADDR + GH3X_VREG_FUNCTION_OFFSET * unFunctionID &&\
        usVirtualRegAddr < GH3X_ADT_ALG_CFG_ADDR + GH3X_VREG_FUNCTION_OFFSET * unFunctionID)
    {
        GH3X_WriteDrvConfigWithVirtualReg(unFunctionID, usVirtualRegAddr, usVirtualRegValue);
    }
   else if (usVirtualRegAddr >= GH3X_ADT_ALG_CFG_ADDR + GH3X_VREG_FUNCTION_OFFSET * unFunctionID &&\
             usVirtualRegAddr < GH3X_ADT_DRV_CFG_ADDR + GH3X_VREG_FUNCTION_OFFSET * (unFunctionID + 1))
    {
#if (__GOODIX_ALGO_CALL_MODE__)
        GH3X_WriteAlgConfigWithVirtualReg(usVirtualRegAddr, usVirtualRegValue);
#endif
   }
}



/**
 * @fn     GS8 GH3X_WriteVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
 *
 * @brief  Write virtual reg val, for software param config
 *
 * @attention   Virtual reg addr has del control bits, so reg addr is [0:11] valid.
 *
 * @param[in]   usVirtualRegAddr        virtual reg addr
 * @param[in]   usVirtualRegValue       virtual reg value
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_WriteVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
#if 0
    if(usVirtualRegAddr == 0xfffd)
    {
//        GH3X_DemoStopSampling(usVirtualRegValue);
    }
    if(usVirtualRegAddr == 0xfffb)
    {
//        GH3X_DemoStartSampling(usVirtualRegValue);
    }
    if(usVirtualRegAddr == 0xfff9)
    {
//        GH3X_DemoSlotControl(usVirtualRegValue,0);
    }
    if(usVirtualRegAddr == 0xfff7)
    {
//        GH3X_DemoSlotControl(usVirtualRegValue,1);
    }
    if(usVirtualRegAddr == 0xfff5)
    {
//        GH3X_DemoSlotSampleRate(10,  usVirtualRegValue); 
    }
    if(usVirtualRegAddr == 0xfff3)
    {
//        GH3X_DemoSlotCuurent(10, (double)usVirtualRegValue); 
    }
#endif
    if (usVirtualRegAddr < GH3X_REG_IS_VIRTUAL0X7_BIT) 
    {
        if (usVirtualRegAddr > GH3X_TOP_INFO_ADDR && 0 == g_usCurrentConfigFlag &&
                usVirtualRegAddr != GH3X_CURRENT_TIMESTAMP_L_ADDR && usVirtualRegAddr != GH3X_CURRENT_TIMESTAMP_H_ADDR
        )
        {
            GH3X_DEBUG_LOG_PARAM("[%s]:Config version error!!!\r\n", __FUNCTION__);
        }
        else
        {
            GH3X_WriteSwConfigWithVirtualReg(usVirtualRegAddr, usVirtualRegValue);
        }
    }
    else if (usVirtualRegAddr < GH3X_FINISH_FLAG_ADDR)
    {
          GH3X_WriteFunctionConfigWithVirtualReg(usVirtualRegAddr, usVirtualRegValue);
	      //GH3X_WriteAlgConfigWithVirtualReg(usVirtualRegAddr, usVirtualRegValue);

    }
    
    switch (usVirtualRegAddr)
    {
        case GH3X_END_FLAG_ADDR: // virtual reg write finish
            if (g_usCurrentConfigFlag == 1)
            {
                g_usCurrentConfigFlag = 0;
            }
            else
            {
                GH3X_DEBUG_LOG_PARAM("[%s]:Config version error!!!\r\n", __FUNCTION__);
            }
            //HOOK_FUNC_CALL(g_pGh3xConfigSetStopHookFunc, ());
            GH3X_TargetFunctionInit();
            break;
        case GH3X_TOP_INFO_ADDR:
            //HOOK_FUNC_CALL(g_pGh3x2xConfigSetStartHookFunc, ());
            g_usCurrentConfigFlag = 1;
            SetPPGSlotInfo();
            GH3X_SwConfigDeinit();
            GH3X_SetDemoWorkMode(usVirtualRegValue);
            GH3X_LoadConfigDoneHook();
            break;
        default:
            break;
    }

}


void GH3X_VirtualRegPrint(void)
{
	#if 0
	GH3X_DEBUG_LOG_PARAM("[%s],\r\n", __FUNCTION__);
	for(int i = 0; i < 21; i++)
	{
		if(gpstFrameInfo[i] != 0)
		{
			GH3X_DEBUG_LOG_PARAM("function id:%d, fs:%d, chn_num:%d!!!\r\n",i,gpstFrameInfo[i]->pstGh3xData->usSampleRate,gpstFrameInfo[i]->pstGh3xData->uchChnlNum);
			GH3X_DEBUG_LOG_PARAM("channel map: \r\n");
			for(int j = 0; j < 17; j++)
			{
				GH3X_DEBUG_LOG_PARAM("%d, ", gpstFrameInfo[i]->pstGh3xData->pusChnlMap[j]);
			}
			GH3X_DEBUG_LOG_PARAM("\r\n\r\n");
		}
	}
	GH3X_DEBUG_LOG_PARAM("[%s], HR algo: \r\n", __FUNCTION__);
	for(int index = 0; index < 5; index++)
	{
		GH3X_DEBUG_LOG_PARAM("gunHrParamsArr[%d] = %d, ", index, gunHrParamsArr[index]);
	}
	GH3X_DEBUG_LOG_PARAM("\r\n");
	GH3X_DEBUG_LOG_PARAM("algo: gstHbaAlgoChnlMap.uchNum = %d \r\n",gstHbaAlgoChnlMap.uchNum);
	int uchNum = gstHbaAlgoChnlMap.uchNum;
	for(int index = 0; index < uchNum; index++)
	{
		GH3X_DEBUG_LOG_PARAM("gstHbaAlgoChnlMap[%d] = %d, ",index, gstHbaAlgoChnlMap.uchAlgoChnlMap[index]);
	}
	GH3X_DEBUG_LOG_PARAM("\r\n\r\n");
  GH3X_DEBUG_LOG_PARAM("[%s], SPO2 algo: \r\n", __FUNCTION__);
  for(int index = 0; index < 11; index++)
	{
		GH3X_DEBUG_LOG_PARAM("gnSpo2ParamssArr[%d] = %d, ", index, gnSpo2ParamssArr[index]);
	}
	GH3X_DEBUG_LOG_PARAM("\r\n");
	GH3X_DEBUG_LOG_PARAM("algo: gstSpo2AlgoChnlMap.uchNum = %d \r\n",gstSpo2AlgoChnlMap.uchNum);
	
	
  for(int index = 0; index < gstSpo2AlgoChnlMap.uchNum; index++)
	{
		GH3X_DEBUG_LOG_PARAM("gstSpo2AlgoChnlMap[%d] = %d, ", index, gstSpo2AlgoChnlMap.uchAlgoChnlMap[index]);
	}
	GH3X_DEBUG_LOG_PARAM("\r\n\r\n");
	GH3X_DEBUG_LOG_PARAM("[%s], ECG algo: \r\n", __FUNCTION__);
	for(int index = 0; index < 3; index++)
	{
		GH3X_DEBUG_LOG_PARAM("gnEcgParamsArr[%d] = %d, ", index, gnEcgParamsArr[index]);
	}
	GH3X_DEBUG_LOG_PARAM("\r\n\r\n");
	GH3X_DEBUG_LOG_PARAM("[%s], BIA algo: \r\n", __FUNCTION__);
	for(int index = 0; index < 5; index++)
	{
		GH3X_DEBUG_LOG_PARAM("gnBiaPInfo[%d] = %d, ", index, gnBiaPInfo[index]);
	}
	GH3X_DEBUG_LOG_PARAM("\r\n");
	GH3X_DEBUG_LOG_PARAM("algo: gstBiaAlgoChnlMap.uchNum = %d \r\n",gstBiaAlgoChnlMap.uchNum);
	for(int index = 0; index < gstBiaAlgoChnlMap.uchNum; index++)
	{
		GH3X_DEBUG_LOG_PARAM("gstBiaAlgoChnlMap[%d] = %d, ", index, gstBiaAlgoChnlMap.uchAlgoChnlMap[index]);
	}
	GH3X_DEBUG_LOG_PARAM("\r\n\r\n");
	GH3X_DEBUG_LOG_PARAM("[%s], Top info: \r\n", __FUNCTION__);
	GH3X_DEBUG_LOG_PARAM("g_usInfoConfigArrVer = %d \r\n",g_usInfoConfigArrVer);
	GH3X_DEBUG_LOG_PARAM("g_usInfoConfigToolVer = %d \r\n",g_usInfoConfigToolVer);
	GH3X_DEBUG_LOG_PARAM("g_usInfoProjectId = %d \r\n",g_usInfoProjectId);
	GH3X_DEBUG_LOG_PARAM("\r\n\r\n");
	GH3X_DEBUG_LOG_PARAM("[%s], FIFO Ctrl: \r\n", __FUNCTION__);
	GH3X_DEBUG_LOG_PARAM("g_uchFifoPackageMode = %d \r\n",g_usFifoPackageMode);
	GH3X_DEBUG_LOG_PARAM("g_usNormalFifoWaterLine = %d \r\n",g_usNormalFifoWaterLine);
	GH3X_DEBUG_LOG_PARAM("g_usAdtFifoWaterLine = %d \r\n",g_usAdtFifoWaterLine);
	GH3X_DEBUG_LOG_PARAM("\r\n\r\n");
	GH3X_DEBUG_LOG_PARAM("[%s], G sensor cfg: \r\n", __FUNCTION__);
	GH3X_DEBUG_LOG_PARAM("g_uchGsensorVendorId = %d \r\n",g_uchGsensorVendorId);
	GH3X_DEBUG_LOG_PARAM("g_usGsensorSampleRate = %d \r\n",g_usGsensorSampleRate);
	GH3X_DEBUG_LOG_PARAM("g_uchGsensorCtrl = %d \r\n",g_uchGsensorCtrl);
	GH3X_DEBUG_LOG_PARAM("\r\n\r\n");	
	GH3X_DEBUG_LOG_PARAM("[%s], Cap cfg: \r\n", __FUNCTION__);
	GH3X_DEBUG_LOG_PARAM("g_uchCapCtrl = %d \r\n",g_uchCapCtrl);
	GH3X_DEBUG_LOG_PARAM("g_usCapSampleRate = %d \r\n",g_usCapSampleRate);
	GH3X_DEBUG_LOG_PARAM("\r\n\r\n");	
	GH3X_DEBUG_LOG_PARAM("[%s], Temp cfg: \r\n", __FUNCTION__);
    GH3X_DEBUG_LOG_PARAM("g_uchTempCtrl = %d \r\n",g_uchTempCtrl);
	GH3X_DEBUG_LOG_PARAM("g_usTempSampleRate = %d \r\n",g_usTempSampleRate);
	GH3X_DEBUG_LOG_PARAM("\r\n\r\n");
	GH3X_DEBUG_LOG_PARAM("[%s], Soft AGC para: \r\n", __FUNCTION__);
    GH3X_DEBUG_LOG_PARAM("uchSlotxSoftAgcAdjEn = %d \r\n",gstSoftAgcCfg.uchSlotxSoftAgcAdjEn);
	GH3X_DEBUG_LOG_PARAM("uchSlotxSoftBgAdjEn = %d \r\n",gstSoftAgcCfg.uchSlotxSoftBgAdjEn);	
	GH3X_DEBUG_LOG_PARAM("uchAmbSampleEn = %d \r\n",gstSoftAgcCfg.stAmbSlotCtrl.uchAmbSampleEn);
	GH3X_DEBUG_LOG_PARAM("uchAmbSlot = %d \r\n",gstSoftAgcCfg.stAmbSlotCtrl.uchAmbSlot);
    GH3X_DEBUG_LOG_PARAM("uchGainLimitBg32uA = %d \r\n",gstSoftAgcCfg.stSoftAgcGainLimit.uchGainLimitBg32uA);
	GH3X_DEBUG_LOG_PARAM("uchGainLimitBg64uA = %d \r\n",gstSoftAgcCfg.stSoftAgcGainLimit.uchGainLimitBg64uA);
	GH3X_DEBUG_LOG_PARAM("uchGainLimitBg128uA = %d \r\n",gstSoftAgcCfg.stSoftAgcGainLimit.uchGainLimitBg128uA);
	GH3X_DEBUG_LOG_PARAM("uchGainLimitBg256uA = %d \r\n",gstSoftAgcCfg.stSoftAgcGainLimit.uchGainLimitBg256uA);
	GH3X_DEBUG_LOG_PARAM("uchSpo2RedSlotCfgEn = %d \r\n",gstSoftAgcCfg.uchSpo2RedSlotCfgEn);
	GH3X_DEBUG_LOG_PARAM("uchSpo2IrSlotCfgEn = %d \r\n",gstSoftAgcCfg.uchSpo2IrSlotCfgEn);
	GH3X_DEBUG_LOG_PARAM("unAgcTrigThd_H = %d \r\n",gstSoftAgcCfg.unAgcTrigThd_H);
	GH3X_DEBUG_LOG_PARAM("unAgcTrigThd_L = %d \r\n",gstSoftAgcCfg.unAgcTrigThd_L);
	GH3X_DEBUG_LOG_PARAM("unAgcRestrainThd_H = %d \r\n",gstSoftAgcCfg.unAgcRestrainThd_H);
	GH3X_DEBUG_LOG_PARAM("unAgcRestrainThd_L = %d \r\n",gstSoftAgcCfg.unAgcRestrainThd_L);
	GH3X_DEBUG_LOG_PARAM("\r\n\r\n");
	#endif
}



/**
 * @fn     GS8 GH3X_LoadNewRegConfigArr(const STReg *pstRegConfigArr, GU16 usRegConfigLen)
 *
 * @brief  Load new gh3x reg config array
 *
 * @attention   If reg val don't need verify, should set reg addr bit 12;
 *              If reg is virtual reg, should set reg addr bit 13;
 *              e.g.      need config reg 0x0000: 0x0611
 *                        {0x0000, 0x0611} //verify write by read reg
 *                        {0x1000, 0x0611} //don't need verify write val
 *                        {0x2000, 0x0611} //set virtual reg
 *
 * @param[in]   pstRegConfigArr       pointer to the reg struct array
 * @param[in]   usRegConfigLen        reg struct array length
 * @param[out]  None
 *
 * @return  errcode
 * @retval  #GH3X_RET_OK               return successfully
 * @retval  #GH3X_RET_COMM_ERROR       gh3x communicate error
 */
GS8 GH3X_LoadNewRegConfigArr(const STReg *pstRegConfigArr, GU16 usRegConfigLen , GU8 uchChipIndex)
{
    GS8 chRet = GH3X_RET_OK;
    GU16 usIndex = 0;
//    GU16 usReadRegVal = 0;
    //GU16 usReadRegVal = 0;

    GH3X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
    if ((pstRegConfigArr != GH3X_PTR_NULL) && (usRegConfigLen != 0))
    {
        for (usIndex = 0; usIndex < usRegConfigLen; usIndex++) // write GH3X reg.
        {
            switch (GH3X_GET_BIT_IS_SET(pstRegConfigArr[usIndex].usRegAddr))
            {
            case 0:
                GH3X_WriteReg(pstRegConfigArr[usIndex].usRegAddr, pstRegConfigArr[usIndex].usRegData);
                // usReadRegVal = GH3X_ReadReg(pstRegConfigArr[usIndex].usRegAddr);
                // if (usReadRegVal != pstRegConfigArr[usIndex].usRegData)
                // {
                //     GH3X_DEBUG_LOG_PARAM("[%s]:vreg reg verify error! addr:0x%.4x,w_val:0x%.4x,r_val:0x%.4x\r\n", __FUNCTION__,
                //                             pstRegConfigArr[usIndex].usRegAddr,
                //                             pstRegConfigArr[usIndex].usRegData, usReadRegVal);
                // }
                GH3X_SetDemoWorkMode(0);
                break;
            default:
                GH3X_WriteVirtualReg(pstRegConfigArr[usIndex].usRegAddr, pstRegConfigArr[usIndex].usRegData);
                break;
            }
        } // end of for (usIndex = 0; usIndex < usRegConfigLen; usIndex++)
    } // end of if ((pstRegConfigArr != GH3X_PTR_NULL) && (usRegConfigLen != 0))

    #if 0
//		for(int j = 0; j < 16; j++)
	GH3X_VirtualRegPrint();
	#endif
//	}

	return chRet;
}


/**
 * @fn     GU8 GH3X2X_GetGsensorEnableFlag(void)
 *
 * @brief  Inquire if need gsensor data
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  1: need gsensor data  0: not need gsensor data
 */
GU8 GH3X_GetGsensorEnableFlag(void)
{
    return g_uchGsensorCtrl;
}



void GH3X_SwConfigDeinit(void)
{
    //host
    g_usInfoConfigArrVer = 0;
    g_usInfoConfigToolVer = 0;
    g_usInfoProjectId = 0;
    g_unInfoCreatedTimestamp = 0;
    g_unInfoCreatedTimestamp = 0;
  //g_uchFifoPackageMode = 0;
    GH3X_FunctionFrameInfoInit(0xFFFFFFFF);
    g_usAdtFifoWaterLine = 0;
    g_usNormalFifoWaterLine = 0;
    g_uchGsensorVendorId = 0;
    g_usGsensorSampleRate = 0;
    g_uchGsensorCtrl =  0;
    g_uchCapCtrl = 0;
    g_usCapSampleRate = 0;
    g_uchTempCtrl = 0;
    g_usTempSampleRate = 0;

    //agc
#if	__SUPPORT_SOFT_AGC_CONFIG__
    GH3X_LedAgcReset();
#endif
//fs //channel map
    for (int uchFuncOffsetIndex = 0; uchFuncOffsetIndex < GH3X_FUNC_OFFSET_MAX; uchFuncOffsetIndex++)
    {
       if(gpstFrameInfo[uchFuncOffsetIndex] != 0)
        {
            gpstFrameInfo[uchFuncOffsetIndex] ->pstGh3xData->usSampleRate  = (0);
            gpstFrameInfo[uchFuncOffsetIndex] ->pstGh3xData->uchChnlNum = 0;
        }
    }
    //drv
  g_usBiaMeasureMode = 0;
  g_usBiaCalibrateRx = 0;
  g_usBiaCapacitanceTxp = 0;
  g_usBiaCapacitanceTxn = 0;
  g_usBiaCapacitanceRxp = 0;
  g_usBiaCapacitanceRxn = 0;
    //alg
#if (__GOODIX_ALGO_CALL_MODE__)
    GH3X_AlgConfigDeinit();
#endif
}
 

