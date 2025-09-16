/**
  ****************************************************************************************
  * @file    GH_PROTOCOL_cmd.c
  * @author  GHealth Driver Team
  * @brief   
  ****************************************************************************************
  * @attention  
  #####Copyright (c) 2022 GOODIX
   All rights reserved.
  
  ****************************************************************************************
  */

#include "gh3x_drv.h"
#include "gh3x_demo.h"
#include "gh3x_demo_inner.h"
#include "stdint.h"
#include "gh3x_protocol.h"
#include "gh3x_protocol_cmd.h"
#include "gh3x_demo_protocol.h"
#if (__DRIVER_LIB_MODE__ == __DRV_LIB_WITH_ALGO__)
#include "gh3x_demo_algo_call.h"
#include "gh3x_demo_algo_config.h"
#endif

#ifdef GOODIX_DEMO_PLATFORM
#ifdef    GR5526_SK
#include "iap_control.h"
#else
#ifndef    GR5515_SK
#include "iap_control.h"
#endif
#endif  
#endif
 
//static GU8 guchSrcCallback[GH_PROTOCOL_BUF_LEN] = {0};
static GU32 gunTargetFuncMode = 0;
extern GU8 guchProtocolStatus;
extern GCHAR* GH3X_GetDemoVersion(void);
extern GCHAR* GH3X_GetBootloaderVersion(void);

GS32 GH3x_WaitPllReady(GS32 timeOut_ms)
{
    while(timeOut_ms > 0 && 1 != GH3X_ReadRegBitField(0x786, 2, 2))
    {
        GH3X_DelayMs(1);
        timeOut_ms -= 1;
    }
    GH3X_WriteRegBitField(0x786, 2, 2, 1);
    return (timeOut_ms > 0) ? GH_PRO_HAL_FAILED : GH_PRO_HAL_SUCCESS;
}
void GH3X_SetProtocolTargetFunction(GU32 unTargetFuncMode)
{
    gunTargetFuncMode = unTargetFuncMode;
}

GU32 GH3X_GetProtocolTargetFunction(void)
{
    return gunTargetFuncMode;
}
#if __SUPPORT_GHEALTH_PROTOCOL__
void GH3X_GetVersion(uint8_t uchVerType,int8_t* pchVer, size_t* size)
{
    switch (uchVerType)
    {
        case UPROTOCOL_GET_VER_TYPE_FW_VER:
            *size = GH3X_Strlen((const GCHAR *)GH3X_GetDemoVersion()) + 1;
            GH3X_Memcpy(pchVer, GH3X_GetDemoVersion(), *size);
            break;
        case UPROTOCOL_GET_VER_TYPE_DEMO_VER:
            *size = GH3X_Strlen((const GCHAR *)GH3X_GetDemoVersion()) + 1;
            GH3X_Memcpy(pchVer, GH3X_GetDemoVersion(), *size);
            break;
        case UPROTOCOL_GET_VER_TYPE_BOOTLOADER_VER:
            *size = GH3X_Strlen((const GCHAR *)GH3X_GetBootloaderVersion()) + 1;
            GH3X_Memcpy(pchVer, GH3X_GetBootloaderVersion(), *size);
            break;
        case UPROTOCOL_GET_VER_TYPE_PROTOCOL_VER:
            *size = GH3X_Strlen((const GCHAR *)GH3X_GetDemoVersion()) + 1;
            GH3X_Memcpy(pchVer, GH3X_GetDemoVersion(), *size);
            break;
        case UPROTOCOL_GET_VER_TYPE_VIRTUAL_REG_VER:
            *size = GH3X_Strlen((const GCHAR *)GH3X_GetVirtualRegVersion()) + 1;
            GH3X_Memcpy(pchVer, GH3X_GetVirtualRegVersion(), *size);
            break;
        case UPROTOCOL_GET_VER_TYPE_DRV_VER:
            *size = GH3X_Strlen((const GCHAR *)GH3X_GetDriverLibVersion()) + 1;
            GH3X_Memcpy(pchVer, GH3X_GetDriverLibVersion(), *size);
            break;
        default:
        if (uchVerType >= UPROTOCOL_GET_VER_TYPE_ALGO_VER && uchVerType <= UPROTOCOL_GET_VER_TYPE_ALGO_VER_END)
        {
            GU32 unFunctionOffset = (GU32)(uchVerType - UPROTOCOL_GET_VER_TYPE_ALGO_VER);
            GCHAR uchVersion[150] = {0};
#if (__GOODIX_ALGO_CALL_MODE__)
            GH3X_AlgoVersion(unFunctionOffset, uchVersion);
#endif
            *size = GH3X_Strlen((const GCHAR *)uchVersion) + 1;
            GH3X_Memcpy(pchVer, uchVersion, *size);
        }
        else
        {
            *size = GH3X_Strlen((const GCHAR *)UPROTOCOL_GET_VER_DEFAULT_VER_STRING) + 1;
            GH3X_Memcpy(pchVer, UPROTOCOL_GET_VER_DEFAULT_VER_STRING, *size);
        }
        break;
    }
}

void GH3X_RegsWriteCmd(uint16_t* pusRegs,int32_t nSize)
{
#ifdef GOODIX_DEMO_PLATFORM
    GH_IntfLock();
#endif
    if(nSize >= 2)
    {
        GU16 usRegAddr = pusRegs[0];
        GU16 usRegLen = nSize - 1;
        switch (GH3X_GET_BIT_IS_SET(usRegAddr))
        {
            case GH3X_REG_IS_VIRTUAL0X0_BIT:
            GH3X_ExitLowPowerMode();
            GH3X_WriteRegs(usRegAddr, &pusRegs[1], usRegLen);
            GH3X_EnterLowPowerMode();
            break;
            default:
            for (GU16 cnt = 0; cnt < usRegLen; cnt++)
            {
                GH3X_WriteVirtualReg(usRegAddr + cnt * GH3X_REG_ADDR_SIZE, pusRegs[cnt + 1]);
            }
            break;
        }
    }
#ifdef GOODIX_DEMO_PLATFORM
    GH_IntfUnLock();
#endif
}

void GH3X_RegsReadCmd(uint16_t usRegAddr,int32_t nReadLen, uint16_t *pusRegValueBuffer, int32_t* pnLen)
{
#ifdef GOODIX_DEMO_PLATFORM
    GH_IntfLock();
#endif
    *pnLen = nReadLen > 100 ? 100 : nReadLen;
    switch (GH3X_GET_BIT_IS_SET(usRegAddr))
    {
    case GH3X_REG_IS_VIRTUAL0X0_BIT:
        GH3X_ExitLowPowerMode();
        GH3X_ReadRegs(usRegAddr, pusRegValueBuffer, *pnLen);
        GH3X_EnterLowPowerMode();
        break;
    default:
        break;
    }
#ifdef GOODIX_DEMO_PLATFORM
    GH_IntfUnLock();
#endif
}
GU8 GH3x_NeedWaitPllReady(STRegField regField)
{
    if(regField.unRegAddr == 0x780)
    {
       GU16 usMakData = (((0x0001) << (regField.uchMsb - regField.uchLsb + 1)) - 1) << regField.uchLsb;
       GU16 usRegData = 0;
       usRegData &= (~(usMakData));
       usRegData |= ((regField.usRegData << regField.uchLsb) & usMakData);
       GU16 usCheckMakData = (((0x0001) << (10 - 8 + 1)) - 1) << 8;

       if((usRegData & usCheckMakData ) != 0)
          return 1;
    }
    return 0;
}
void GH3X_RegsBitFieldWriteCmd(uint16_t* usRegBits, size_t size)
{
#ifdef GOODIX_DEMO_PLATFORM
    GH_IntfLock();
#endif
    size -= (size % 4);
    uint16_t usRegAddr;uint8_t uchMsb;uint8_t uchLsb;uint16_t usRegVal;
    for(GU16 i = 0; i < size;i += 4)
    {
        usRegAddr = usRegBits[i];
        uchLsb = usRegBits[i+1];
        uchMsb = usRegBits[i+2];
        usRegVal = usRegBits[i+3];
        switch (GH3X_GET_BIT_IS_SET(usRegAddr))
        {
        case GH3X_REG_IS_VIRTUAL0X0_BIT:
            GH3X_ExitLowPowerMode();
            GH3X_WriteRegBitField((GU32)usRegAddr, (GU8)uchLsb, (GU8)uchMsb, usRegVal);
            STRegField regField;
            regField.unRegAddr = (GU32)usRegAddr;
            regField.uchLsb = uchLsb;
            regField.uchMsb = uchMsb;
            regField.usRegData = usRegVal;
            if(GH3x_NeedWaitPllReady(regField))
            {
                GH3X_WriteRegBitField(0x0008, 10, 10, 0);
                GH3x_WaitPllReady(5);
            }
            GH3X_EnterLowPowerMode();
            break;
        default:
            break;
        }
    }
#ifdef GOODIX_DEMO_PLATFORM
    GH_IntfUnLock();
#endif
}

void GH3X_RegBitFieldWriteCmd(uint16_t usRegAddr,uint8_t uchLsb,uint8_t uchMsb, uint16_t usRegVal)
{
#ifdef GOODIX_DEMO_PLATFORM
    GH_IntfLock();
#endif
    switch (GH3X_GET_BIT_IS_SET(usRegAddr))
    {
    case GH3X_REG_IS_VIRTUAL0X0_BIT:
        GH3X_ExitLowPowerMode();
        GH3X_WriteRegBitField((GU32)usRegAddr, (GU8)uchLsb, (GU8)uchMsb, usRegVal);
        STRegField regField;
        regField.unRegAddr = (GU32)usRegAddr;
        regField.uchLsb = uchLsb;
        regField.uchMsb = uchMsb;
        regField.usRegData = usRegVal;
        if(GH3x_NeedWaitPllReady(regField))
        {
            GH3X_WriteRegBitField(0x0008, 10, 10, 0);
            GH3x_WaitPllReady(5);
        }
        GH3X_EnterLowPowerMode();
        break;
    default:
        break;
    }
#ifdef GOODIX_DEMO_PLATFORM
    GH_IntfUnLock();
#endif
}
void GH3X_ChipCtrl(uint8_t uchCtrlType)
{
#ifdef GOODIX_DEMO_PLATFORM
    GH_IntfLock();
#endif
    GH3X_SAMPLE_LOG_PARAM("[%s]:cmd = 0x%02x\n", __FUNCTION__, uchCtrlType);
    switch (uchCtrlType)
    {
    case UPROTOCOL_CHIP_CTRL_TYPE_HARD_RESET:
        GH3X_HardReset();
        GH3X_SetDemoWorkMode(0);
        GH3X_DelayMs(30);
        break;
    case UPROTOCOL_CHIP_CTRL_TYPE_SOFT_RESET:
        GH3X_SoftReset();
        GH3X_SetDemoWorkMode(0);
        GH3X_DelayMs(30);
        break;
    case UPROTOCOL_CHIP_CTRL_TYPE_WAKEUP:
        GH3X_ExitLowPowerMode();
        break;
    case UPROTOCOL_CHIP_CTRL_TYPE_SLEEP:
        GH3X_EnterLowPowerMode();
        break;
    default:
        break;
    }
#ifdef GOODIX_DEMO_PLATFORM
    GH_IntfUnLock();
#endif
}
void GH3X_RegsListWriteCmd(uint16_t* usRegs, uint16_t usLen)
{
#ifdef GOODIX_DEMO_PLATFORM
    GH_IntfLock();
#endif
    usLen -= usLen % 2;
    GH3X_ExitLowPowerMode();
    for(GU16 i = 0; i < usLen; i +=2)
    {
        GU16 usRegAddr = usRegs[i];
        GU16 usRegVal = usRegs[i+1];
        switch (GH3X_GET_BIT_IS_SET(usRegAddr))
        {
        case GH3X_REG_IS_VIRTUAL0X0_BIT:
            GH3X_WriteReg((GU32)usRegAddr, usRegVal);
            //usReadRegVal = GH3X_ReadReg((GU32)usRegAddr);
            if(usRegAddr == 0x780)
            {
                GH3x_WaitPllReady(5);
                GH3X_LoadConfigDoneHook();
            }
            break;
        default:
            GH3X_WriteVirtualReg((GU32)usRegAddr, usRegVal);
            break;
        }
    }
    GH3X_EnterLowPowerMode();
#ifdef GOODIX_DEMO_PLATFORM
    GH_IntfUnLock();
#endif
}
extern void GhMultiSensorWearDetStop(void);
void GH3X_SwFunctionCmd(uint32_t unTargetFuncMode, uint8_t uchCtrlType)
{
#ifdef GOODIX_DEMO_PLATFORM
    GH_IntfLock();
#endif
    if(unTargetFuncMode != 0)
    {
        if (UPROTOCOL_CMD_START == uchCtrlType)
        {
            gunTargetFuncMode |= unTargetFuncMode;
            if (gunTargetFuncMode & (1 << GH3X_FUNC_OFFSET_ADT))
            {
                #if __GH_MULTI_SENSOR_EVENT_PRO_CONIG__
                if (GhMultSensorWearEventManagerIsEnable())   //if ADT is started by master(PC/APP), we need stop mulit sensor wear detection firstly
                {
                    GhMultiSensorWearDetStop();
                }
                #endif
                GH3X_DemoStartSampling(GH3X_FUNCTION_ADT);
            }
            else
            {
                GH3X_DemoStartSampling(unTargetFuncMode);
            }
        }
        else if (UPROTOCOL_CMD_STOP== uchCtrlType)
        {
            gunTargetFuncMode &= (~unTargetFuncMode);
            GH3X_DemoStopSampling(unTargetFuncMode);
        }
    }
#ifdef GOODIX_DEMO_PLATFORM
    GH_IntfUnLock();
#endif
}

void GHSetWorkModeCmd(uint8_t uchWorkMode)
{

}

#ifdef GOODIX_DEMO_PLATFORM
#ifdef GR5526_SK
void IapSlavePackets(u8 *pPackets,u16 PacketsLen,u8* puchRet, u32* pRetLen);
#endif
#endif
void GH3X_FwUpdateCmd(uint8_t* pSrc,uint32_t usLen,uint8_t* puchRet,uint32_t* pRetLen)
{
#ifdef GOODIX_DEMO_PLATFORM
#ifdef GR5526_SK

    //GU8 *puchSrc = (GU8*)pSrc;
    //IapSlaveReceivesIapPackets(puchSrc);
    IapSlavePackets(pSrc,usLen,puchRet,(u32*)pRetLen);
#else
#ifndef GR5515_SK
    GU8 *puchSrc = (GU8*)pSrc;
    //IapSlaveReceivesIapPackets(puchSrc);
#endif
#endif
#endif
}
#endif

