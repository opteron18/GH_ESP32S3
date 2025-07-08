/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x2x_drv_soft_led_agc.c
 *
 * @brief   gh3x2x led agc process functions
 *
 * @version see gh3x2x_drv_version.h
 *
 */
#include <stdio.h>
#include "gh3x_drv.h"
#include "gh3x_demo_config.h"
#include "gh3x_soft_led_agc.h"

#if __SUPPORT_SOFT_AGC_CONFIG__

#define MAX_PPGCFG_DRVCUR_NUM     (16)  // ppgcfg0-15 need store drv current
GU8 g_puchDrvCurrentPreSoftAgc[MAX_PPGCFG_DRVCUR_NUM] = {0}; // 16 PPGCFG numble
static const STGh3xAgcReg g_pstGH3xAgcReg[] = 
{
    {6,  11, 8},              //0: agc en
    {24, 7,  0},              //1: led current up limit
    {24, 15, 8},               //2: led current down limit
    {18, 7,  0},               //3: led current drv0
    {18, 15, 8},               //4: led current drv1
    {20, 7,  0},               //5: led current drv2
    {20, 15, 8},               //6: led current drv3
    { 8, 3, 0},               //7: rx0 tia gain
    { 8, 7, 4},               //8: rx1 tia gain
    { 8, 11, 8},               //9: rx2 tia gain
    { 8, 15, 12},               //10: rx3 tia gain
    { 6, 7,6},               //11: agc src
    {16, 1,0},               //12: rx0 bg cancel
    {16, 3,2},               //13: rx1 bg cancel    
    {16, 5,4},               //14: rx2 bg cancel
    {16, 7,6},               //15: rx2 bg cancel
    { 0, 15,8},               //16: sr mulipliper
    { 0, 3,0},               //17: TiaRx en
};

// soft agc parameter
static STSoftAgcCfg g_stSoftAgcCfg =
{
    0x0000,   /**< soft agc enable */
    0,        /**< soft bg cancel adjust enable */
    {0,0,0},       /**< amb slot ctrl */
    {7,6,5,4},   /**< soft gain limit */
    0,
    0,
};

static const GU16 g_usGH3xTiaGainR[] =    
{
    78,
    156,
    313,
    625,
    1250,
    2500,
    5000,
    10000,
    20000,       //unit 0.1Kom : To ensure a certain precision without taking up too much memory
};

typedef struct 
{
      GU16 Sat_CheckCnt[GH3X_NEW_AGC_SLOT_NUM_LIMIT];  /* num of data checked*/
      GU16 Sat_cnt[GH3X_NEW_AGC_SLOT_NUM_LIMIT]; /* sat num of each channel*/
      GU16 Sat_flag; /* sat flag one bit each channel*/
} STGh3x2xNewAgcMainChnlSat;

//main chnl
static STGh3xNewAgcMainChnlInfo  g_pstGh3xNewAgcMainChnlInfoEntity[GH3X_NEW_AGC_SLOT_NUM_LIMIT];
static STGh3xNewAgcMainChnlMeanInfo g_pstGh3xNewAgcMainChnlMeanInfoEntity[GH3X_NEW_AGC_SLOT_NUM_LIMIT];
static STGh3xNewAgcMainChnlIdaelAdjInfo g_pstGh3xNewAgcMainChnlIdaelAdjInfoEntity[GH3X_NEW_AGC_SLOT_NUM_LIMIT];
//sub chnl
#if GH3X_NEW_AGC_SUB_CHNL_NUM_LIMIT > 0
static STGh3xNewAgcSubChnlRxInfo g_pstGh3xNewAgcSubChnlRxInfoEntity[GH3X_NEW_AGC_SUB_CHNL_NUM_LIMIT];
#endif

static STGh3x2xNewAgcMainChnlSat g_pstGh3x2xNewAgcMainChnlSat;
const GU32 g_unNewAgcSpo2Trig_H_Thd = __GH3X_AGC_SPO2_TRIG_H_THD__;
const GU32 g_unNewAgcSpo2Trig_L_Thd = __GH3X_AGC_SPO2_TRIG_L_THD__;
const GU32 g_unNewAgcSpo2Ideal_value = __GH3X_AGC_SPO2_IDEAL_VALUE__;
const GU32 g_unNewAgcGeneTrig_H_Thd = __GH3X_AGC_GENERAL_TRIG_H_THD__;
const GU32 g_unNewAgcGeneTrig_L_Thd = __GH3X_AGC_GENERAL_TRIG_L_THD__;
const GU32 g_unNewAgcGeneIdeal_value = __GH3X_AGC_GENERAL_IDEAL_VALUE__;
const GU8  g_uchAgcConvergence_level = __GH3X2X_AGC_CONVERGENCE_LEVEL__;

static GU8 g_Gh3x2xNewAgcInitFlag = 0;

static void GH3xSetAgcReg(GU8 uchAgcReg,  GU8 uchCfgNum, GU16 usValue)
{
    GH3X_WriteRegBitField( GH3X_AGC_PPGCFG0_ADDRESS + uchCfgNum*GH3X_AGC_PPGCFG_ADRS_GAP + g_pstGH3xAgcReg[uchAgcReg].uchAddrOffset, g_pstGH3xAgcReg[uchAgcReg].uchLsb, g_pstGH3xAgcReg[uchAgcReg].uchMsb, usValue);
}

static GU16 GH3xGetAgcReg(GU8 uchAgcReg,  GU8 uchCfgNum)
{
    return GH3X_ReadRegBitField(GH3X_AGC_PPGCFG0_ADDRESS + uchCfgNum*GH3X_AGC_PPGCFG_ADRS_GAP + g_pstGH3xAgcReg[uchAgcReg].uchAddrOffset, g_pstGH3xAgcReg[uchAgcReg].uchLsb, g_pstGH3xAgcReg[uchAgcReg].uchMsb);
}


static void GH3X_StoreRXxDrvCurrentPreAgc(GU8 uchCurrent, GU8 uchSlotIdx, GU8* puchCurrentRecord)
{
    puchCurrentRecord[uchSlotIdx] = uchCurrent;
}


#if GH3X_NEW_AGC_SUB_CHNL_NUM_LIMIT > 0
static void Gh3x2xNewAgcSubChnlModuleReset(void)
{
    GH3X_Memset((GU8*)g_pstGh3xNewAgcSubChnlRxInfoEntity,0,(GU16)GH3X_NEW_AGC_SUB_CHNL_NUM_LIMIT*sizeof(STGh3xNewAgcSubChnlRxInfo));
}
#endif 


void GH3xNewAgcModuleReset(void)
{
    g_Gh3x2xNewAgcInitFlag = 0;
    GH3X_Memset((GU8*)g_pstGh3xNewAgcMainChnlInfoEntity,0,(GU16)GH3X_NEW_AGC_SLOT_NUM_LIMIT*sizeof(STGh3xNewAgcMainChnlInfo));
    GH3X_Memset( (GU8*)(&g_pstGh3x2xNewAgcMainChnlSat),0,sizeof(STGh3x2xNewAgcMainChnlSat)); // init  variable
    GH3X_Memset((GU8*)g_pstGh3xNewAgcMainChnlMeanInfoEntity,0,(GU16)GH3X_NEW_AGC_SLOT_NUM_LIMIT*sizeof(STGh3xNewAgcMainChnlMeanInfo)); 
    GH3X_Memset((GU8*)g_pstGh3xNewAgcMainChnlIdaelAdjInfoEntity,0,(GU16)GH3X_NEW_AGC_SLOT_NUM_LIMIT*sizeof(STGh3xNewAgcMainChnlIdaelAdjInfo));                           

#if GH3X_NEW_AGC_SUB_CHNL_NUM_LIMIT > 0
    Gh3x2xNewAgcSubChnlModuleReset();
#endif  
    
}


//static GU16 GH3xGetAgcRegSlotIdx( GU8 uchSlot)
//{
//    const STGh3xAgcReg g_pstGH3xAgcRegSlotIdxCtrl[] = 
//    {
//        {0,  4, 0},              //0: even slotidx bit field 
//        {1, 12, 8},              //1: odd slotidx bit field 
//    };
//    return GH3X_ReadRegBitField(GH3X_AGC_SLOTIDX_CTRL0_ADDRESS + (uchSlot/2)*2 , g_pstGH3xAgcRegSlotIdxCtrl[(uchSlot%2)].uchLsb, g_pstGH3xAgcRegSlotIdxCtrl[(uchSlot%2)].uchMsb);
//}


//static void GH3X_NewAgcRecoveryAgcRegToMainChnl(GU8 uchSlotIdx, STGh3xNewAgcMainChnlInfo *pstAgcMainChnlInfo)
//{
//    GH3xSetAgcReg(GH3X_AGC_REG_LED_CURRENT_UP_LIMIT, uchSlotIdx, pstAgcMainChnlInfo->uchBakRegLedCurrentMax);
//    GH3xSetAgcReg(GH3X_AGC_REG_LED_CURRENT_DOWN_LIMIT, uchSlotIdx, pstAgcMainChnlInfo->uchBakRegLedCurrentMin);
//    GH3xSetAgcReg(GH3X_AGC_REG_AGC_SRC, uchSlotIdx, pstAgcMainChnlInfo->uchBakRegAgcSrc);
//    GH3X_DEBUG_LOG_PARAM("[NewAgc] recover agc src:agc_src = %d, SlotNo = %d\r\n",pstAgcMainChnlInfo->uchBakRegAgcSrc,uchSlotIdx);            
//}

static GU8 GH3X_NewAgcFindSubChnlSlotInfo(GU8 uchSlot)
{
    for(GU8 uchSlotCnt = 0; uchSlotCnt < GH3X_NEW_AGC_SLOT_NUM_LIMIT; uchSlotCnt ++)
    {
        STGh3xNewAgcMainChnlInfo *pstAgcMainChnlInfo = &g_pstGh3xNewAgcMainChnlInfoEntity[uchSlotCnt];
        if(pstAgcMainChnlInfo->uchBakRegAgcEn)
        {
            if(uchSlot == (pstAgcMainChnlInfo->uchSlotAdc >> 2))
            {
                return uchSlotCnt;
            }
        }
    }
    return 0;
}

static GU16 GH3xCalTrigerUpLimitForBiggerGain(GU8 uchCurrentGain ,GU8 uchSlotIdx)
{
    GU32 unTempRawdata;
    GU32 unTempTrigThd_H;

    GU8 uchSlotCnt = GH3X_NewAgcFindSubChnlSlotInfo(uchSlotIdx);
    STGh3xNewAgcMainChnlInfo *pstAgcMainChnlInfo = &g_pstGh3xNewAgcMainChnlInfoEntity[uchSlotCnt];
    if(pstAgcMainChnlInfo->uchSpo2ChnlIndex)
    {
        unTempTrigThd_H = g_unNewAgcSpo2Trig_H_Thd;
    }else
    {
        unTempTrigThd_H = g_unNewAgcGeneTrig_H_Thd;
    }

    if(8 <= uchCurrentGain) // The maximum gain is 8
    {
        return 0xFFFF;
    }
    else
    {
        unTempRawdata = unTempTrigThd_H - GH3X_DATA_BASE_LINE;
        unTempRawdata = GH3X_DATA_BASE_LINE + (GU32)(((float)unTempRawdata)/(float)1.3*(float)g_usGH3xTiaGainR[uchCurrentGain]/(float)g_usGH3xTiaGainR[uchCurrentGain + 1]);
        return unTempRawdata>>8;
    }
}


#if GH3X_NEW_AGC_SUB_CHNL_NUM_LIMIT > 0
static void GH3xNewAgcSubChnlClearAnalysisCntAndSetGain(GU8 uchSlotIdx, GU8 uchRxEn ,GU8 uchNextGain)
{
    for(GU8 uchRxCnt = 0; uchRxCnt < 4; uchRxCnt ++)
    {
        if(uchRxEn&(1 << uchRxCnt))
        {
            for(GU8 uchChnlCnt = 0; uchChnlCnt < GH3X_NEW_AGC_SUB_CHNL_NUM_LIMIT; uchChnlCnt ++)
            {
                STGh3xNewAgcSubChnlRxInfo *pstSTGh3xNewAgcSubChnlRxInfo = &(g_pstGh3xNewAgcSubChnlRxInfoEntity[uchChnlCnt]);
                if(pstSTGh3xNewAgcSubChnlRxInfo->uchEn)
                {
                    if(((uchSlotIdx << 2) + uchRxCnt) == pstSTGh3xNewAgcSubChnlRxInfo->uchSlotAdcNo)
                    {
                        pstSTGh3xNewAgcSubChnlRxInfo->usAnalysisCnt = 0;
                        if(0xFF != uchNextGain)
                        {
                            GH3xSetAgcReg(GH3X_AGC_REG_TIA_GAIN_RX0 + uchRxCnt, uchSlotIdx, 0);
                            pstSTGh3xNewAgcSubChnlRxInfo->uchCurrentGain = uchNextGain;
                            pstSTGh3xNewAgcSubChnlRxInfo->usTrigeUpLimitForBiggerGain = GH3xCalTrigerUpLimitForBiggerGain(uchNextGain,pstSTGh3xNewAgcSubChnlRxInfo->uchSlotAdcNo >> 2);
                        }
                    }
                }
            }
        }
    }
}


static GU8 GH3xNewAgcSetNewSubChnl(GU8 uchSlotIdx, GU8 uchSubRxEn)
{
    GU8 uchSetSuccess = 1;
    for(GU8 uchRxCnt = 0; uchRxCnt < 4; uchRxCnt ++)
    {
        if(uchSubRxEn&(1 << uchRxCnt))
        {
            uchSetSuccess = 0;
            for(GU8 uchChnlCnt = 0; uchChnlCnt < GH3X_NEW_AGC_SUB_CHNL_NUM_LIMIT; uchChnlCnt ++)
            {
                STGh3xNewAgcSubChnlRxInfo *pstSTGh3xNewAgcSubChnlRxInfo = &(g_pstGh3xNewAgcSubChnlRxInfoEntity[uchChnlCnt]);
                if(0 ==  pstSTGh3xNewAgcSubChnlRxInfo->uchEn)
                {
                    pstSTGh3xNewAgcSubChnlRxInfo->uchEn = 1;
                    pstSTGh3xNewAgcSubChnlRxInfo->uchSlotAdcNo = (uchSlotIdx << 2) + uchRxCnt;
                    //read current gain
                    pstSTGh3xNewAgcSubChnlRxInfo->uchCurrentGain = GH3xGetAgcReg(GH3X_AGC_REG_TIA_GAIN_RX0 + uchRxCnt, uchSlotIdx);                    
                    GH3X_DEBUG_LOG_PARAM("[NewAgc] Sub chnl current Gain = %d, Slot = %d, Rx = %d\r\n",pstSTGh3xNewAgcSubChnlRxInfo->uchCurrentGain,uchSlotIdx,uchRxCnt);         
                    pstSTGh3xNewAgcSubChnlRxInfo->usTrigeUpLimitForBiggerGain = GH3xCalTrigerUpLimitForBiggerGain(pstSTGh3xNewAgcSubChnlRxInfo->uchCurrentGain, uchSlotIdx);    
                    GH3X_DEBUG_LOG_PARAM("[NewAgc] usTrigeUpLimitForBiggerGain = %d, Slot = %d, Rx = %d\r\n",((GU32)pstSTGh3xNewAgcSubChnlRxInfo->usTrigeUpLimitForBiggerGain)<<8,uchSlotIdx,uchRxCnt);       
                    uchSetSuccess = 1;
                    break;
                }
            }
            if(0 == uchSetSuccess)
            {
                GH3X_DEBUG_LOG_PARAM("[NewAgc] Set new sub chnl fail !!! Slot = %d, Rx = %d\r\n",uchSlotIdx,uchRxCnt);
                while(1);
            }
        }
    }
    return uchSetSuccess;
}
#endif


static void GH3X_AgcGetThreshold( GU8 uchSpo2ChnlIndex, GU32* punTemp_H_Thd, GU32* punTemp_L_Thd )
{
   if(uchSpo2ChnlIndex) // is  spo2 slot
   {
      *punTemp_H_Thd = g_unNewAgcSpo2Trig_H_Thd; 
      *punTemp_L_Thd = g_unNewAgcSpo2Trig_L_Thd; 
   }else
   { 
      *punTemp_H_Thd = g_unNewAgcGeneTrig_H_Thd;
      *punTemp_L_Thd = g_unNewAgcGeneTrig_L_Thd;      
   }
}


//Multiple DRV drive LED simultaneously, the initial current should be set the same, if the setting is wrong, the average value
static GU8 GH3xGetDrvCurrent(GU8 uchSlotIdx ,GU8 uchAgcEn) 
{
    GU8 uchTempDrv = 0, uchCnt = 0;
    GU16 usDrvSun = 0;
    for(GU8 i = 0; i < 4; i++)
    {
       if(uchAgcEn&(1 << i))
       {    
            uchTempDrv = GH3xGetAgcReg(GH3X_AGC_REG_LED_CURRENT_DRV0 + i, uchSlotIdx);
            usDrvSun += uchTempDrv;
            uchCnt++; 
       }
    }

    if(uchCnt)
    {
        uchTempDrv = usDrvSun/uchCnt;
    }
    return uchTempDrv;
}


static GU8 GH3xGetDrvCurrentAvg(GU8 uchSlotIdx) 
{
    GU8 uchTempDrv = 0, uchCnt = 0;
    GU16 usDrvSun = 0;
    for(GU8 i = 0; i < 4; i++)
    {  
        uchTempDrv = GH3xGetAgcReg(GH3X_AGC_REG_LED_CURRENT_DRV0 + i, uchSlotIdx);
        if(uchTempDrv)
        {    
             usDrvSun += uchTempDrv;
             uchCnt++; 
        }
    }

    if(uchCnt)
    {
        uchTempDrv = usDrvSun/uchCnt;
    }
    return uchTempDrv;
}



static void GH3xNewAgcChnlInfoInit(void)
{
    // EXAMPLE_LOG("GH3X_NEW_AGC_SLOT_NUM_LIMIT:%d-------------------------------",GH3X_NEW_AGC_SLOT_NUM_LIMIT);
    if(0 == GH3X_NEW_AGC_SLOT_NUM_LIMIT)
    {
        return;
    }
    // EXAMPLE_LOG("g_Gh3x2xNewAgcInitFlag:%d-------------------------------",g_Gh3x2xNewAgcInitFlag);
    if(0 == g_Gh3x2xNewAgcInitFlag)
    {
        // EXAMPLE_LOG("g_Gh3x2xNewAgcInitFlag:%d-------------------------------",g_Gh3x2xNewAgcInitFlag);
        GU8 uchNewAgcSlotCnt = 0;
        GU16 usPPG_CFGN_Used = 0;
        if(g_stSoftAgcCfg.uchSlotxSoftAgcAdjEn)  // if configure software AGC 
        {
             GH3X_WriteRegBitField( GH3X_AGC_MainSwitch_ADDR, 0, 0, 0); //  Turn off the AGC main switch
        }
        
        STPPGSlotInfo* pstPPGSlotInfo = get_stPPGSlotInfo();
        GU8* puchConfigSlotIdx = (GU8*)(pstPPGSlotInfo->usSlotIndex); 

        for(GU8 uchSlotCnt = 0; uchSlotCnt < 16; uchSlotCnt ++)   // Support up to 16 slots
        {    
            // EXAMPLE_LOG("Support up to 16 slots--------------------------------------------------------------------");
             GU8 uchSlotIdx; //Indicates which CFG is used by the current slot
             uchSlotIdx = puchConfigSlotIdx[uchSlotCnt];
             GU8 uchRegAgcEn =    GH3xGetAgcReg(GH3X_AGC_REG_AGC_EN, uchSlotIdx);
             if(15<uchSlotIdx)  ////not ppg cfg no AGC
             {
                  continue;
             }

             if(usPPG_CFGN_Used&(1<<uchSlotIdx))  //// drop duplicate ppgcfg 
             {
                 continue;
             }
             usPPG_CFGN_Used |= (1<<uchSlotIdx);

            if((g_stSoftAgcCfg.uchSlotxSoftAgcAdjEn & (1 << uchSlotCnt)))   // configed SoftAGC
            {
                // EXAMPLE_LOG("g_stSoftAgcCfg.uchSlotxSoftAgcAdjEn & (1 << uchSlotCnt)--------------------------------------------------------------------");
                if(uchNewAgcSlotCnt < GH3X_NEW_AGC_SLOT_NUM_LIMIT)
                {
                    g_pstGh3xNewAgcMainChnlInfoEntity[uchNewAgcSlotCnt].uchSlotAdc = (uchSlotIdx << 2); 
                    g_pstGh3xNewAgcMainChnlInfoEntity[uchNewAgcSlotCnt].uchBakRegAgcEn = uchRegAgcEn;
                     /******* slot function Distinguish   START******************************************************/          
                    if((g_stSoftAgcCfg.uchSpo2RedSlotCfgEn & (1 << uchSlotCnt))||(g_stSoftAgcCfg.uchSpo2IrSlotCfgEn & (1 << uchSlotCnt)))
                    {
                        g_pstGh3xNewAgcMainChnlInfoEntity[uchNewAgcSlotCnt].uchSpo2ChnlIndex =  1;
                        GH3X_DEBUG_LOG_PARAM("[NewAgc] SlotNum = %d, Spo2RedSlotCfgEn = %d, Spo2IrSlotCfgEn = %d\r\n",uchSlotCnt, g_stSoftAgcCfg.uchSpo2RedSlotCfgEn, g_stSoftAgcCfg.uchSpo2IrSlotCfgEn);   
                    }
                    EXAMPLE_LOG("SlotNum--------------------------------------------------------------------");
                    /******* slot function Distinguish   END******************************************************/
                    GH3X_DEBUG_LOG_PARAM("[NewAgc] SlotNum = %d, PPGCFG = %d, AGCEn = %d\r\n",uchSlotCnt,uchSlotIdx,uchRegAgcEn);             
                    uchNewAgcSlotCnt ++;
                }
                else
                {
                    GH3X_DEBUG_LOG_PARAM("[NewAgc] error !!! main chnl info is no enough! \r\n");    
                    while(1);
                }
            }

            if(uchSlotIdx < MAX_PPGCFG_DRVCUR_NUM)
            {
                GU8 uchDrvCur = GH3xGetDrvCurrentAvg(uchSlotIdx);
                GH3X_StoreRXxDrvCurrentPreAgc(uchDrvCur, uchSlotIdx, g_puchDrvCurrentPreSoftAgc);    
            }

        }
        g_Gh3x2xNewAgcInitFlag = 1;
    }

    for(GU8 uchSlotCnt = 0; uchSlotCnt < GH3X_NEW_AGC_SLOT_NUM_LIMIT; uchSlotCnt ++)
    {
        STGh3xNewAgcMainChnlInfo *pstAgcMainChnlInfo = &g_pstGh3xNewAgcMainChnlInfoEntity[uchSlotCnt];
        GH3X_DEBUG_LOG_PARAM("AGC_InitValue %d: Enable = %d, AgcAdjEn = %d\r\n",uchSlotCnt, pstAgcMainChnlInfo->uchBakRegAgcEn,g_stSoftAgcCfg.uchSlotxSoftAgcAdjEn);    
        if(pstAgcMainChnlInfo->uchBakRegAgcEn)
        {
            GU8 uchSlotIdx; //Indicates which CFG is used by the current slot
            uchSlotIdx = ((pstAgcMainChnlInfo->uchSlotAdc >> 2) & 0x0f);
            GH3X_DEBUG_LOG_PARAM("AGC_InitFlag%d: SlotCfgX=%d, Vaule = %d\r\n",uchSlotCnt, uchSlotIdx, pstAgcMainChnlInfo->uchHaveBakRegFlag);
            if(0 == pstAgcMainChnlInfo->uchHaveBakRegFlag)
            {
                GU8 uchBgCancel;
                GU8 uchGainLimit;
                //bak reg
                pstAgcMainChnlInfo->uchBakRegAgcSrc = GH3xGetAgcReg(GH3X_AGC_REG_AGC_SRC, uchSlotIdx);
                pstAgcMainChnlInfo->uchSlotAdc = (uchSlotIdx << 2) + pstAgcMainChnlInfo->uchBakRegAgcSrc;
                pstAgcMainChnlInfo->uchBakRegLedCurrentMax = GH3xGetAgcReg(GH3X_AGC_REG_LED_CURRENT_UP_LIMIT, uchSlotIdx);                
                pstAgcMainChnlInfo->uchBakRegLedCurrentMin = GH3xGetAgcReg(GH3X_AGC_REG_LED_CURRENT_DOWN_LIMIT, uchSlotIdx);    
                pstAgcMainChnlInfo->uchBakTiaGain = GH3xGetAgcReg(GH3X_AGC_REG_TIA_GAIN_RX0 + pstAgcMainChnlInfo->uchBakRegAgcSrc, uchSlotIdx);
                pstAgcMainChnlInfo->uchCurrentTiaGain = pstAgcMainChnlInfo->uchBakTiaGain;
                pstAgcMainChnlInfo->uchCurrentDrv = GH3xGetDrvCurrent(uchSlotIdx,pstAgcMainChnlInfo->uchBakRegAgcEn);  
                uchBgCancel = GH3xGetAgcReg(GH3X_AGC_REG_BG_CANCEL_RX0 + pstAgcMainChnlInfo->uchBakRegAgcSrc, uchSlotIdx);
                if(0 == uchBgCancel)
                {
                    uchGainLimit = g_stSoftAgcCfg.stSoftAgcGainLimit.uchGainLimitBg32uA;
                }
                else if(1 == uchBgCancel)
                {
                    uchGainLimit = g_stSoftAgcCfg.stSoftAgcGainLimit.uchGainLimitBg64uA;
                }
                else if(2 == uchBgCancel)
                {
                    uchGainLimit = g_stSoftAgcCfg.stSoftAgcGainLimit.uchGainLimitBg128uA;
                }
                else
                {
                    uchGainLimit = g_stSoftAgcCfg.stSoftAgcGainLimit.uchGainLimitBg256uA;
                }
                GH3X_DEBUG_LOG_PARAM("[NewAgc] SlotAdc=%d, CurrentDrv=%d, uchGainLimit=%d, uchSlotIdx=%d\r\n",pstAgcMainChnlInfo->uchSlotAdc,pstAgcMainChnlInfo->uchCurrentDrv,uchGainLimit,uchSlotIdx);    
                pstAgcMainChnlInfo->uchTiaGainUpLimit = uchGainLimit;
                GU16 usFastestSampleRate =  GH3X_ReadRegBitField(GH3X_FASTEST_SAMPLERATE_ADDR,0,7);
                GU16 usSampleRate = GH3X_SYS_SAMPLE_RATE/(usFastestSampleRate + 1)/(GH3xGetAgcReg(GH3X_AGC_REG_SR_MULIPLIPER, uchSlotIdx) + 1);
                g_pstGh3xNewAgcMainChnlMeanInfoEntity[uchSlotCnt].usSampleRate = usSampleRate;  //init

                GU16 usPpgRxEn = GH3xGetAgcReg(GH3X_AGC_PPG_RX_EN, uchSlotIdx);
                usPpgRxEn &= (~(1 << pstAgcMainChnlInfo->uchBakRegAgcSrc));   //clear main channel RX enable bit
                //sub channel slot
#if GH3X_NEW_AGC_SUB_CHNL_NUM_LIMIT > 0
                GH3xNewAgcSetNewSubChnl(uchSlotIdx,usPpgRxEn);
#endif
//              GH3xSetAgcReg(GH3X_AGC_REG_AGC_EN, uchSlotIdx, 0);   // disable hardware agc 
                pstAgcMainChnlInfo->uchHaveBakRegFlag = 1;
            }
            else
            {
              ////get initial DRV and current in case them change before StartSampling()  
              pstAgcMainChnlInfo->uchCurrentTiaGain = GH3xGetAgcReg(GH3X_AGC_REG_TIA_GAIN_RX0 + pstAgcMainChnlInfo->uchBakRegAgcSrc, uchSlotIdx);
              pstAgcMainChnlInfo->uchCurrentDrv = GH3xGetDrvCurrent(uchSlotIdx,pstAgcMainChnlInfo->uchBakRegAgcEn);
              GH3X_DEBUG_LOG_PARAM("Drv Gian inite read %d: IniteDrv = %d, IniteGain = %d\r\n",uchSlotIdx, pstAgcMainChnlInfo->uchCurrentDrv, pstAgcMainChnlInfo->uchCurrentTiaGain);
              //GH3X_NewAgcRecoveryAgcRegToMainChnl(uchSlotIdx,pstAgcMainChnlInfo);
            }
        }
    }
}


static GU32 GH32x2xMedSel(GU32 unData0, GU32 unData1, GU32 unData2)
{
    if((unData0 >= unData1)&&(unData0 <= unData2))
    {
        return unData0;
    }
    if((unData0 >= unData2)&&(unData0 <= unData1))
    {
        return unData0;
    }
    if((unData1 >= unData0)&&(unData1 <= unData2))
    {
        return unData1;
    }
    if((unData1 >= unData2)&&(unData1 <= unData0))
    {
        return unData1;
    }
    return unData2;
}


#if GH3X_NEW_AGC_SUB_CHNL_NUM_LIMIT > 0
static void GH3X_NewAgcSubChnlGainAdj(GU8* puchReadFifoBuffer, GU16 usFifoLen)
{
    for(GU8 uchChnlCnt = 0; uchChnlCnt < GH3X_NEW_AGC_SUB_CHNL_NUM_LIMIT; uchChnlCnt ++)
    {
        GU16 usAnalysisDiscardLen;
        GU16 usAnalysisEndLen;
        GU8  uchSlotCnt;
        GU8  uchSlotIdx;
        GU8  uchAdcNo;
        GU32 unTempTrigThd_L;
        GU32 unTempTrigThd_H;
        STGh3xNewAgcSubChnlRxInfo *pstSTGh3xNewAgcSubChnlRxInfo = &(g_pstGh3xNewAgcSubChnlRxInfoEntity[uchChnlCnt]);
        GU8 uchSubChnlSlotAdcNo = pstSTGh3xNewAgcSubChnlRxInfo->uchSlotAdcNo;
        GU8 uchSubChnlEn = pstSTGh3xNewAgcSubChnlRxInfo->uchEn;
        uchSlotIdx = pstSTGh3xNewAgcSubChnlRxInfo->uchSlotAdcNo >> 2;
        uchAdcNo = pstSTGh3xNewAgcSubChnlRxInfo->uchSlotAdcNo&0x03;
        uchSlotCnt = GH3X_NewAgcFindSubChnlSlotInfo(uchSlotIdx);
        GU16 usSampleRate = g_pstGh3xNewAgcMainChnlMeanInfoEntity[uchSlotCnt].usSampleRate;

        GU8 uchTiaGainLimit = g_pstGh3xNewAgcMainChnlInfoEntity[uchSlotCnt].uchTiaGainUpLimit;
        usAnalysisDiscardLen = (usSampleRate<<1)/5;    //  2/5 Fs
        usAnalysisEndLen = usAnalysisDiscardLen + usSampleRate * GH3X_NEW_AGC_SUB_CHNNL_ANAYLSIS_TIME_SEC;

        GH3X_AgcGetThreshold(g_pstGh3xNewAgcMainChnlInfoEntity[uchSlotCnt].uchSpo2ChnlIndex,&unTempTrigThd_H, &unTempTrigThd_L);    

        if(uchSubChnlEn)
        {
            // prevent unRawdataSum(GU32) overflow
            if( (usAnalysisEndLen - usAnalysisDiscardLen) > 200 )  
            {
               usAnalysisDiscardLen = usAnalysisEndLen - 200;   // maximum number of accumulated rawdata is 200
            }

            for (GU16 usIndex = 0; usIndex < usFifoLen; usIndex += GH3X_FIFO_RAWDATA_SIZE) 
            {

                if( puchReadFifoBuffer[usIndex]&0xE0 ) // not ppg Rawdata
                {
                     continue;
                }    
                GU8 uchSlotAdcNo = ((puchReadFifoBuffer[usIndex]&0x1f)<<2) + ((puchReadFifoBuffer[usIndex+1] & 0x0f)>>2);
                // GH3X_DEBUG_LOG_PARAM("ChnlSlotAdcNo=%d,SlotAdcNo=%d\r\n",pstSTGh3xNewAgcSubChnlRxInfo->uchSlotAdcNo,uchSlotAdcNo );    
                if(uchSlotAdcNo == uchSubChnlSlotAdcNo)  
                {
                    GU32 unDataRaw = GH3X_MAKEUP_3BYTES(puchReadFifoBuffer[usIndex+2], puchReadFifoBuffer[usIndex+3],puchReadFifoBuffer[usIndex+4]);

                    if(usAnalysisDiscardLen == pstSTGh3xNewAgcSubChnlRxInfo->usAnalysisCnt) //init
                    {
                        pstSTGh3xNewAgcSubChnlRxInfo->unRawdataSum = 0;
                        pstSTGh3xNewAgcSubChnlRxInfo->unLastLastRawdata = unDataRaw;
                        pstSTGh3xNewAgcSubChnlRxInfo->unLastRawdata = unDataRaw;
                        pstSTGh3xNewAgcSubChnlRxInfo->usMoreThanUpLimitPointCnt = 0;
                        pstSTGh3xNewAgcSubChnlRxInfo->usMoreThanUpLimitPointCntForBiggerGain = 0;
                    }
                    if(pstSTGh3xNewAgcSubChnlRxInfo->usAnalysisCnt < usAnalysisEndLen)  //analysising
                    {                
                        GU32 unTempRawdata = GH32x2xMedSel(pstSTGh3xNewAgcSubChnlRxInfo->unLastLastRawdata, pstSTGh3xNewAgcSubChnlRxInfo->unLastRawdata, unDataRaw);
                        if(usAnalysisDiscardLen == pstSTGh3xNewAgcSubChnlRxInfo->usAnalysisCnt)
                        {
                            GH3X_DEBUG_LOG_PARAM("[NewAgc] sub chnl unTempRawdata  = %d, Slot = %d, rx = %d\r\n",unTempRawdata, uchSlotIdx, uchAdcNo);                                
                        }
                        pstSTGh3xNewAgcSubChnlRxInfo->unRawdataSum += unTempRawdata;
                        if(unTempRawdata > unTempTrigThd_H)
                        {
                            pstSTGh3xNewAgcSubChnlRxInfo->usMoreThanUpLimitPointCnt ++;
                        }
                        if((unTempRawdata >> 8) > pstSTGh3xNewAgcSubChnlRxInfo->usTrigeUpLimitForBiggerGain)
                        {
                            pstSTGh3xNewAgcSubChnlRxInfo->usMoreThanUpLimitPointCntForBiggerGain ++;
                        }
                        pstSTGh3xNewAgcSubChnlRxInfo->unLastLastRawdata = pstSTGh3xNewAgcSubChnlRxInfo->unLastRawdata;
                        pstSTGh3xNewAgcSubChnlRxInfo->unLastRawdata = unDataRaw;
                        pstSTGh3xNewAgcSubChnlRxInfo->usAnalysisCnt ++;                    
                    }
                    //// Complete adjust as soon as possible
                    if(pstSTGh3xNewAgcSubChnlRxInfo->usAnalysisCnt == usAnalysisEndLen) //anlysis finished
                    {
                        GU32 unAvaRawdata;
                        GU16 unRealAnaylsisNum = usAnalysisEndLen - usAnalysisDiscardLen;
                        GU8  uchNextGain = 0xFF;
                        GU8  uchCurrentGain = pstSTGh3xNewAgcSubChnlRxInfo->uchCurrentGain;
                        unAvaRawdata = pstSTGh3xNewAgcSubChnlRxInfo->unRawdataSum/unRealAnaylsisNum;
//                        GH3X_DEBUG_LOG_PARAM("[NewAgc] sub chnl rawdata ava  = %d,DownLimit = %d, Slot = %d, rx = %d\r\n",unAvaRawdata, unTempTrigThd_L,uchSlotIdx, uchAdcNo);                                
//                        GH3X_DEBUG_LOG_PARAM("[NewAgc] usMoreThanUpLimitPointCnt  = %d,UpLimit = %d, Slot = %d, rx = %d\r\n",pstSTGh3xNewAgcSubChnlRxInfo->usMoreThanUpLimitPointCnt, unTempTrigThd_H,uchSlotIdx, uchAdcNo);                        
//                        GH3X_DEBUG_LOG_PARAM("[NewAgc] usMoreThanUpLimitPointCntForBiggerGain = %d,UpLimit = %d, Slot = %d, rx = %d\r\n",pstSTGh3xNewAgcSubChnlRxInfo->usMoreThanUpLimitPointCntForBiggerGain, ((GU32)pstSTGh3xNewAgcSubChnlRxInfo->usTrigeUpLimitForBiggerGain)<<8, uchSlotIdx, uchAdcNo);            
                        if(pstSTGh3xNewAgcSubChnlRxInfo->usMoreThanUpLimitPointCnt > (unRealAnaylsisNum >> 3))  // 1/8 points is bigger than trige up limit
                        {
                            if(uchCurrentGain > 0)
                            {
                                uchNextGain = uchCurrentGain - 1;
                            }
                        }
                        if((pstSTGh3xNewAgcSubChnlRxInfo->usMoreThanUpLimitPointCntForBiggerGain < (unRealAnaylsisNum >> 3))&&(unAvaRawdata < unTempTrigThd_L))
                        {
                            if(uchCurrentGain < uchTiaGainLimit)
                            {
                                uchNextGain = uchCurrentGain + 1;
                            }
                        }
                        if(0xFF != uchNextGain)
                        {                            
                            GH3X_DEBUG_LOG_PARAM("[NewAgc] sub chnl gain change: NewGain = %d, OldGain = %d, Slot = %d, rx = %d\r\n",uchNextGain,uchCurrentGain, uchSlotIdx, uchAdcNo);            
                            GH3xSetAgcReg(GH3X_AGC_REG_TIA_GAIN_RX0 + uchAdcNo, uchSlotIdx, uchNextGain);
                            pstSTGh3xNewAgcSubChnlRxInfo->uchCurrentGain = uchNextGain;
                            pstSTGh3xNewAgcSubChnlRxInfo->usTrigeUpLimitForBiggerGain = GH3xCalTrigerUpLimitForBiggerGain(uchNextGain, uchSlotIdx);                                    
                        }
                        pstSTGh3xNewAgcSubChnlRxInfo->usAnalysisCnt = 0;
                        break;// Data after completing a mean calculation is discarded
                    }
                }
            }
        }
    }
}
#endif


/**
 * @fn     GU8 GH3X_GetLedAgcState(void)
 * 
 * @brief  get agc state
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  GH3X_LED_AGC_ENABLE or GH3X_LED_AGC_DISABLE
 */ 
GU8 GH3X_GetLedAgcState(void)
{
    return GH3X_LED_AGC_ENABLE;
}


/*********************************************self-test function start*****************************************/
#define GH3X_SELF_TEST  0


#if GH3X_SELF_TEST
const GU32 g_uchSpo2AgcSelftestSlotAdc = 0x01;  //slot0, adc1
const GU32 g_usSpo2AgcFifoLen = 25*4;
const GU32 g_unSpo2AgcRawdata[] = 
{
    10111744,9818116,10074798,9832931,10038819,9812989,10067119,9791632,10220862,9843610,9888564,9974512,10041146,10117796,9858828,10260863,9900963,10159857,10259720,10054459,10247477,9957985,10078474,9916146,9821879
};

GU8 g_uchSpo2AgcFifoBuffer[4*100];

void GH3X_Spo2AgcFifoDataGen(void)
{
    for (GU16 usIndex = 0; usIndex < g_usSpo2AgcFifoLen; usIndex += GH3X_FIFO_RAWDATA_SIZE) 
    {
        if(usIndex >= sizeof(g_uchSpo2AgcFifoBuffer))
        {
            break;
        }
        g_uchSpo2AgcFifoBuffer[usIndex + 0] = (g_uchSpo2AgcSelftestSlotAdc<<3);
        g_uchSpo2AgcFifoBuffer[usIndex + 1] = (0x000000FF&(g_unSpo2AgcRawdata[usIndex/4] >> 16));
        g_uchSpo2AgcFifoBuffer[usIndex + 2] = (0x000000FF&(g_unSpo2AgcRawdata[usIndex/4] >> 8));
        g_uchSpo2AgcFifoBuffer[usIndex + 3] = (0x000000FF&(g_unSpo2AgcRawdata[usIndex/4] >> 0));
    }
}
#endif
/*********************************************self-test function end*****************************************/


/****************************************************************
* Description: calculate mean value of main channel  
* Input:  rawdata  
* Return: none
******************************************************************/
static void GH3X_NewAgcMainChnlMeanValueCal(GU8* puchReadFifoBuffer, GU16 usFifoLen, GU32 *punRawdataMean, GU16 *pAdjFlag)
{
    for(GU8 uchSlotCnt = 0; uchSlotCnt < GH3X_NEW_AGC_SLOT_NUM_LIMIT; uchSlotCnt ++)
    {
        STGh3xNewAgcMainChnlInfo *pstAgcMainChnlInfo = &g_pstGh3xNewAgcMainChnlInfoEntity[uchSlotCnt];
        GU8 uchMainChnlSlotAdcNo = pstAgcMainChnlInfo->uchSlotAdc;

        if(pstAgcMainChnlInfo->uchBakRegAgcEn)   // is  enabled  slot
        {
            GU32 unTemp_Trig_H_Thd;    
            GU32 unTemp_Trig_L_Thd;    
            GU32 unTemp_Ideal_value;
            GU16 usAnalysisEndLen; 
            GU16 usAnalysisDiscardLenAdjust = 0;
            GU16 usAnalysisDiscardLenTrige;
            GU32 TrigeMean  = 0; 

            if(pstAgcMainChnlInfo->uchSpo2ChnlIndex) // is  spo2 slot
            {
                unTemp_Ideal_value= g_unNewAgcSpo2Ideal_value;
            }else
            {
                unTemp_Ideal_value= g_unNewAgcGeneIdeal_value;
            }
            GH3X_AgcGetThreshold( pstAgcMainChnlInfo->uchSpo2ChnlIndex, &unTemp_Trig_H_Thd, &unTemp_Trig_L_Thd);  

            STGh3xNewAgcMainChnlMeanInfo *pstNewAgcMainChnlMeanInfo = &g_pstGh3xNewAgcMainChnlMeanInfoEntity[uchSlotCnt];
            STGh3xNewAgcMainChnlIdaelAdjInfo *pstNewAgcMainChnlIdaelAdjInfo = &g_pstGh3xNewAgcMainChnlIdaelAdjInfoEntity[uchSlotCnt];
            GU16 usSampleRate =  pstNewAgcMainChnlMeanInfo->usSampleRate;
            usAnalysisEndLen =  (usSampleRate << 2)/5 ;  // 4*Fs/5
            usAnalysisDiscardLenTrige = (usSampleRate << 1)/5;  // drop the first 2/5 data       
            //have agc motion at last fifo read period or first init                        
            if(pstNewAgcMainChnlMeanInfo->uchHaveAgcAtLastReadPrd)   
            {
                usAnalysisDiscardLenAdjust =  (usSampleRate << 1)/5;  // drop the first 2/5 data
                //// After AGC, the previous data is drop to calcluate precise mean
                if(!pstNewAgcMainChnlMeanInfo->uchDropFlag)
                {  // The frame count is only cleared for the first time to prevent data num cannot to be SampleRate
                    pstNewAgcMainChnlMeanInfo->usAnalysisCnt = 0;  
                    pstNewAgcMainChnlMeanInfo->uchDropFlag = 1;
                    g_pstGh3x2xNewAgcMainChnlSat.Sat_CheckCnt[uchSlotCnt] = 0; 
                    g_pstGh3x2xNewAgcMainChnlSat.Sat_cnt[uchSlotCnt] = 0; 
                }
            }

            //The data fluctuates when initializing the early stage  
            if(!pstNewAgcMainChnlIdaelAdjInfo->uchIniteFirst)  
            {
                usAnalysisDiscardLenAdjust =  (usSampleRate<< 1)/5;  // drop the first 2/5 data
            }

            // prevent unSumAdjust(GU32) overflow
            if( (usAnalysisEndLen - usAnalysisDiscardLenAdjust) > 200 )
            {
              usAnalysisDiscardLenAdjust = usAnalysisEndLen - 200; // maximum number of accumulated rawdata is 200
            }

            if( (usAnalysisEndLen - usAnalysisDiscardLenTrige) > 200 )
            {
              usAnalysisDiscardLenTrige = usAnalysisEndLen - 200;
            }

            for (GU16 usIndex = 0; usIndex < usFifoLen; usIndex += GH3X_FIFO_RAWDATA_SIZE) 
            {                       
                if( puchReadFifoBuffer[usIndex]&0xE0 ) // not ppg Rawdata  
                {
                     continue;
                }
                GU8 uchSlotAdcNo = ((puchReadFifoBuffer[usIndex]& 0x1f)<<2) + ((puchReadFifoBuffer[usIndex+1]& 0x0f)>>2);
                //GH3X_DEBUG_LOG_PARAM("MainChnlSlotAdcNo=%d,SlotAdcNo=%d\r\n",uchMainChnlSlotAdcNo,uchSlotAdcNo);    
                if(uchSlotAdcNo == uchMainChnlSlotAdcNo)    
                {
                    GU32 unDataRaw = GH3X_MAKEUP_3BYTES(puchReadFifoBuffer[usIndex+2], puchReadFifoBuffer[usIndex+3],puchReadFifoBuffer[usIndex+4]);
                    // begin Saturation judge 
                    if(usAnalysisDiscardLenAdjust <= pstNewAgcMainChnlMeanInfo->usAnalysisCnt)                          
                    {
                        GU16 usSat_Check_Num_Max =   (usSampleRate<<2) + usSampleRate;   //// 5 seconds window
                        g_pstGh3x2xNewAgcMainChnlSat.Sat_CheckCnt[uchSlotCnt]++;
                        if( unDataRaw > unTemp_Trig_H_Thd)
                        {
                             g_pstGh3x2xNewAgcMainChnlSat.Sat_cnt[uchSlotCnt]++;
                        }
                         //// as soon as posible set the Saturation flag 
                        if( g_pstGh3x2xNewAgcMainChnlSat.Sat_cnt[uchSlotCnt] >= (usSat_Check_Num_Max>>4) ) // about 8% of all checked data
                        {
                             g_pstGh3x2xNewAgcMainChnlSat.Sat_flag |= (1<<uchSlotCnt);  
                             g_pstGh3x2xNewAgcMainChnlSat.Sat_cnt[uchSlotCnt] = 0; 
                        }     
                        if(g_pstGh3x2xNewAgcMainChnlSat.Sat_CheckCnt[uchSlotCnt] >= usSat_Check_Num_Max ) 
                        {
                             g_pstGh3x2xNewAgcMainChnlSat.Sat_cnt[uchSlotCnt] = 0;
                             g_pstGh3x2xNewAgcMainChnlSat.Sat_CheckCnt[uchSlotCnt] = 0;  
                        }
                    } 
                    // end Saturation judge  

                    //// begin mean calculate    
//                    GH3X_DEBUG_LOG_PARAM("slotCnt=%d,MainChnlSlotAdc=%d,SlotAdc=%d,Raw=%d\r\n",uchSlotCnt,uchMainChnlSlotAdcNo,uchSlotAdcNo,unDataRaw);                                    
                    if(usAnalysisDiscardLenAdjust == pstNewAgcMainChnlMeanInfo->usAnalysisCnt) //init
                    {
                        pstNewAgcMainChnlMeanInfo->unSumAdjust =  0;
                        pstNewAgcMainChnlMeanInfo->unMaxRawdata =  0;      
                        pstNewAgcMainChnlMeanInfo->unMinRawdata =  16777216;
                    }
                                        
                    if(usAnalysisDiscardLenTrige == pstNewAgcMainChnlMeanInfo->usAnalysisCnt) //init
                    {
                        pstNewAgcMainChnlMeanInfo->unSumTrige  =  0;
                    }    
                                        
                    if(pstNewAgcMainChnlMeanInfo->usAnalysisCnt  < usAnalysisEndLen)   //analysising
                    {
                         pstNewAgcMainChnlMeanInfo->unSumAdjust +=  unDataRaw;
                         pstNewAgcMainChnlMeanInfo->unSumTrige +=  unDataRaw;
                         pstNewAgcMainChnlMeanInfo->usAnalysisCnt++;   
                         pstNewAgcMainChnlMeanInfo->unMaxRawdata = ((unDataRaw > pstNewAgcMainChnlMeanInfo->unMaxRawdata)? unDataRaw: pstNewAgcMainChnlMeanInfo->unMaxRawdata ); // Max of  rawdata
                         pstNewAgcMainChnlMeanInfo->unMinRawdata = ((unDataRaw < pstNewAgcMainChnlMeanInfo->unMinRawdata)? unDataRaw: pstNewAgcMainChnlMeanInfo->unMinRawdata ); // Min of  rawdata
                    }
                    //Anlysis finished calculate mean and judge whether AGC (Complete AGC as soon as possible)
                    if(pstNewAgcMainChnlMeanInfo->usAnalysisCnt == usAnalysisEndLen) 
                    {
                        // mean value to calculate gain and current
                        if(usAnalysisEndLen > usAnalysisDiscardLenAdjust)
                        {
                            punRawdataMean[uchSlotCnt] = pstNewAgcMainChnlMeanInfo->unSumAdjust/(usAnalysisEndLen - usAnalysisDiscardLenAdjust);    
                            TrigeMean = pstNewAgcMainChnlMeanInfo->unSumTrige / (usAnalysisEndLen - usAnalysisDiscardLenTrige);
                        }else //  The normal case will not be executed, in case the division by 0 in abnormal case
                        {
                            punRawdataMean[uchSlotCnt] =   unDataRaw; 
                            TrigeMean =   unDataRaw; 
                        }
                        pstNewAgcMainChnlMeanInfo->unRawdataMean = punRawdataMean[uchSlotCnt];
                        pstNewAgcMainChnlMeanInfo->usAnalysisCnt=0;
                        // The mean calculation key variable is cleared    (Prevent mean calculation from being too small:Accumulation lost some data, but numble of raw is all)    
                        usAnalysisDiscardLenAdjust = 0; 
                        pstNewAgcMainChnlMeanInfo->uchHaveAgcAtLastReadPrd = 0; //  clear  after caculate mean!!!       
                        //// end mean calculat  
                        ///////////////////// whether AGC  and adjust the mean value to calculate gain and current
                        if( (TrigeMean < unTemp_Trig_L_Thd) || (TrigeMean > unTemp_Trig_H_Thd) )
                        {
                          *pAdjFlag |= (1<<uchSlotCnt);  //need adjust gain and current 
                        }

                        if( g_pstGh3x2xNewAgcMainChnlSat.Sat_flag & (1<<uchSlotCnt)) //if the channel is saturation
                        {
                            GU32 unTempMean = 0;         //Pay attention to initialization of local variables
                            if( pstNewAgcMainChnlMeanInfo->unMaxRawdata > unTemp_Trig_H_Thd)
                            {
                                 /////////// Pay attention to data truncation, use 64-bi
                                unTempMean =   (GS64)unTemp_Ideal_value*(GS64)(pstNewAgcMainChnlMeanInfo->unMaxRawdata)/15099494;  // to adjust the max data to (0.8*2^23 + 2^23)
                                *pAdjFlag |= (1<<uchSlotCnt);  //need adjust gain and current 
                            }

                            if( punRawdataMean[uchSlotCnt] > unTemp_Ideal_value)
                            {
                                // adjust the mean value to calculate gain and current (reduce the DC amount)
                                punRawdataMean[uchSlotCnt] = ( (punRawdataMean[uchSlotCnt]>unTempMean)? punRawdataMean[uchSlotCnt]:unTempMean );
                                *pAdjFlag |= (1<<uchSlotCnt);  //need adjust gain and current 
                            } 
                            else if( pstNewAgcMainChnlMeanInfo->unMaxRawdata > unTemp_Trig_H_Thd)
                            {
                                punRawdataMean[uchSlotCnt] = unTempMean; //  Increasing the mean value to reduce the DC amount
                            } 
                        }
                        ////////////////////////////////////////////////////
                        //////////////begin ideal value adjust 
                        if( (!((*pAdjFlag) & (1<<uchSlotCnt))) && (!( g_pstGh3x2xNewAgcMainChnlSat.Sat_flag & (1<<uchSlotCnt))))//if the channel not adjust and satuartion
                        {
                            //// The deviation of the mean and the ideal target value is above 1/8  and   Data fluctuation is less than a certain value
                             if(((pstNewAgcMainChnlMeanInfo->unRawdataMean*g_uchAgcConvergence_level < (unTemp_Ideal_value*(g_uchAgcConvergence_level-1)) ) \
                                || (pstNewAgcMainChnlMeanInfo->unRawdataMean*g_uchAgcConvergence_level > (unTemp_Ideal_value*(g_uchAgcConvergence_level+1)))) \
                                &&( 10*pstNewAgcMainChnlMeanInfo->unMaxRawdata< 12*pstNewAgcMainChnlMeanInfo->unMinRawdata) ) 
                            {
                                if( !pstNewAgcMainChnlIdaelAdjInfo->uchIniteFirst ) //// Inite First time 
                                {
                                    pstNewAgcMainChnlIdaelAdjInfo->uchIniteFirst = 1;
                                    *pAdjFlag |= (1<<uchSlotCnt);  //need adjust gain and current 
                                }
                                else
                                {
                                    pstNewAgcMainChnlIdaelAdjInfo->uchSeconsCnt++;
                                    if(pstNewAgcMainChnlIdaelAdjInfo->uchSeconsCnt >= 19)  //  19*0.8 = 15 seconds
                                    {
                                        *pAdjFlag |= (1<<uchSlotCnt);  //need adjust gain and current 
                                        pstNewAgcMainChnlIdaelAdjInfo->uchSeconsCnt  = 0; 
                                    }    
                                }
                            }
                            else
                            {
                                pstNewAgcMainChnlIdaelAdjInfo->uchSeconsCnt  = 0;
                            }
                        }
                        else
                        {
                            pstNewAgcMainChnlIdaelAdjInfo->uchSeconsCnt  = 0;
                        }
                        pstNewAgcMainChnlIdaelAdjInfo->uchIniteFirst = 1;  //// Must be set to 1 after the first second!!!
                        //////////////end ideal value adjust 
                        GH3X_DEBUG_LOG_PARAM("Mean cal: Rawmean = %d, Adjmean = %d, Slot_num = %d\r\n",pstNewAgcMainChnlMeanInfo->unRawdataMean, punRawdataMean[uchSlotCnt], uchSlotCnt);
                        // GH3X_DEBUG_LOG_PARAM("Sat flag: SatCnt = %d, SatCheck = %d, CheckMaxNum = %d, Satflag = %d\r\n", g_pstGh3x2xNewAgcMainChnlSat.Sat_cnt[uchSlotCnt], g_pstGh3x2xNewAgcMainChnlSat.Sat_CheckCnt[uchSlotCnt], g_pstGh3x2xNewAgcMainChnlSat.Sat_Check_Num_Max[uchSlotCnt], g_pstGh3x2xNewAgcMainChnlSat.Sat_flag);
                        GH3X_DEBUG_LOG_PARAM("Ideal adjust: SeconsCnt = %d\r\n", pstNewAgcMainChnlIdaelAdjInfo->uchSeconsCnt);
                        GH3X_DEBUG_LOG_PARAM("Adj flag: Trigmean = %d, L_Thd = %d, H_Thd = %d, Adjflag = %d\r\n", TrigeMean, unTemp_Trig_L_Thd, unTemp_Trig_H_Thd, *pAdjFlag);                                     
                        g_pstGh3x2xNewAgcMainChnlSat.Sat_flag &= (~(1<<uchSlotCnt)) ; ////clear the Sat flag 
                        break; // Data after completing a mean calculation is discarded                        
                   }
                }  
             }
        }
    }
}


/**
 * @fn     GU8 GH3X_NewAgcCalDrvCurrentAndGain(GU32 CurRawdata, GU32 IdealRawdata, STGh3xNewAgcMainChnlInfo *pstAgcMainChnlInfo, GU8* puchIdealDrvCurrent, GU8* puchIdealGain)
 *
 * @brief  mark abnormal chnl
 *
 * @attention
 *
 * @param[in]   CurRawdata, IdealRawdata, uchMainChnlInfoIndex
 * @param[out]  puchIdealDrvCurrent, puchIdealGain
 *
 * @return  0:DrvCurrent or gain will NOT change  1: DrvCurrent or gain will  change
 */
static GU8 GH3X_NewAgcCalDrvCurrentAndGain(GU32 CurRawdata, GU8 uchSlotCnt,  GU8* puchIdealDrvCurrent, GU8* puchIdealGain)
{
    STGh3xNewAgcMainChnlInfo *pstAgcMainChnlInfo = &g_pstGh3xNewAgcMainChnlInfoEntity[uchSlotCnt];
    STGh3xNewAgcMainChnlMeanInfo *pstMainChnlExtralInfo = &g_pstGh3xNewAgcMainChnlMeanInfoEntity[uchSlotCnt];

    GU8 uchDefaultGain = pstAgcMainChnlInfo->uchBakTiaGain;
    GU8 uchGainUpLimit = pstAgcMainChnlInfo->uchTiaGainUpLimit;
    GU8 uchLedCurrentMin = pstAgcMainChnlInfo->uchBakRegLedCurrentMin;
    GU8 uchLedCurrentMax = pstAgcMainChnlInfo->uchBakRegLedCurrentMax;
    GU8 uchCurrentDrv = pstAgcMainChnlInfo->uchCurrentDrv;
    GU8 uchCurrentTiaGain = pstAgcMainChnlInfo->uchCurrentTiaGain;
 
    GU8 uchNewGain = uchDefaultGain;
    GU32 unIdealRawdata_no;
    GU32 unTempDrvCurrent;
    GU8 uchChangeFlag = 0;  //bit0: drv current flag  bit1: gain flag
    GU8 uchCalFinish = 0;
    GU32 unTrigUpLimit;
//      GU32 unTrigLowLimit;
    GU8  unCalNum = 8;  //  9 gears, up to 8 calculations
    if(0 != pstAgcMainChnlInfo->uchSpo2ChnlIndex) // is  spo2 slot
    {
        unTrigUpLimit = g_unNewAgcSpo2Trig_H_Thd;
//        unTrigLowLimit = g_unNewAgcSpo2Trig_L_Thd;
        unIdealRawdata_no= g_unNewAgcSpo2Ideal_value - GH3X_DATA_BASE_LINE;
    }else
    {
        unTrigUpLimit = g_unNewAgcGeneTrig_H_Thd;
//        unTrigLowLimit = g_unNewAgcGeneTrig_L_Thd;
        unIdealRawdata_no = g_unNewAgcGeneIdeal_value - GH3X_DATA_BASE_LINE;
    }


    GH3X_DEBUG_LOG_PARAM("Drv cal: UpLimit = %d, Mean = %d, LastMean = %d\r\n",unTrigUpLimit, pstMainChnlExtralInfo->unRawdataMean, pstMainChnlExtralInfo->unLastRawdataMean);            
        // reduce rawdata as soon as posible
    if((unTrigUpLimit < pstMainChnlExtralInfo->unRawdataMean)&&(unTrigUpLimit < pstMainChnlExtralInfo->unLastRawdataMean)&&(uchCurrentTiaGain > 0)) 
    {
        uchNewGain = 0;  
        unTempDrvCurrent =  uchLedCurrentMin;
        uchCalFinish = 1;
    }
    pstMainChnlExtralInfo->unLastRawdataMean = pstMainChnlExtralInfo->unRawdataMean;

    if(0 == uchCalFinish)
    {
        //remove offset
        if(CurRawdata > (GH3X_DATA_BASE_LINE + 2048))  //// Can't open DC Cance !!!
        {
            CurRawdata -= GH3X_DATA_BASE_LINE;
        }
        else
        {
            //The value may be less than 2^23, preventing division by 0
            CurRawdata = 2048;  // Take an appropriately large number to prevent calculated DRV current overflow 32bit
        }
        //Rawdata stretch  (The principle of magnification: the mean divided by the ideal target value is the data reduction factor )
        if(CurRawdata < 8000000)
        {
        }
        else if(CurRawdata < 8100000)
        {
            CurRawdata = 8000000 + (CurRawdata - 8000000)*6;
        }
        else if(CurRawdata < 8200000)
        {
            CurRawdata = 8000000 + ( (CurRawdata - 8000000)<<3 );
        }
        else
        {
            // The more saturated the data, the bigger the stretch, the key is that the data is really saturated
            CurRawdata = 8000000 + (CurRawdata - 8000000)*14;   //  Blood oxygen drops 1 times, heart rate drops 2.5 times  
        }

        //find new drv current and gain
        do
        {   
            unTempDrvCurrent = (GS64)(((GU32)uchCurrentDrv*(GU32)g_usGH3xTiaGainR[uchCurrentTiaGain]))* \
                 (GS64)unIdealRawdata_no/g_usGH3xTiaGainR[uchNewGain]/CurRawdata;

            GH3X_DEBUG_LOG_PARAM("Drv cal: uchCurrentDrv = %d, uchCurrentTiaGain = %d, unIdealRawdata_no = %d\r\n",uchCurrentDrv, uchCurrentTiaGain, unIdealRawdata_no);
            GH3X_DEBUG_LOG_PARAM("Drv cal: unTempDrvCurrent = %d, uchNewGain = %d, Meanraw = %d, CurRawdata = %d\r\n",unTempDrvCurrent, uchNewGain, pstMainChnlExtralInfo->unRawdataMean, CurRawdata);

            if(unTempDrvCurrent > uchLedCurrentMax)  //drv is too big, need increase gain
            {
                if(uchNewGain < uchGainUpLimit)
                {
                    uchNewGain ++ ;
                }
                else
                {
                    unTempDrvCurrent = uchLedCurrentMax;
                    break;
                }
            }
            else if(unTempDrvCurrent < uchLedCurrentMin)  //drv is too small, need dncrease gain
            {
                if(uchNewGain > 0)
                {
                    uchNewGain -- ;
                }
                else
                {
                    unTempDrvCurrent = uchLedCurrentMin;
                    break;
                }
            }
            else   //got right drv
            {
                break;
            }
            unCalNum --;
        }
        while (unCalNum);
    }

    puchIdealDrvCurrent[0] = (GU8)unTempDrvCurrent;
    puchIdealGain[0] = (GU8)uchNewGain;

    if(puchIdealDrvCurrent[0] != uchCurrentDrv)
    {
        uchChangeFlag |= 0x01;
    }
    if(puchIdealGain[0] != uchCurrentTiaGain)
    {
        uchChangeFlag |= 0x02;
    }

    return uchChangeFlag;
}


GU16 NewAgcMainChelFlag = 0;

static void GH3X_NewAgcMainChnlPro(GU8* puchReadFifoBuffer, GU16 usFifoLen)
{
    GU32 unNewAgcRawdataMeanChnl[16] = {0};
    GU16 usAdjFlag = 0;     // Local variables used as judgments must be initialized to 0
    GU8 uchNewDrvCurrent = 0;
    GU8 uchNewGain = 0;    
    GU8 uchChangeFlag = 0;  //bit0: drv current flag  bit1: gain flag

    if(0 == GH3X_NEW_AGC_SLOT_NUM_LIMIT)
    {
        return;
    }

#if GH3X_SELF_TEST
    GH3X_Spo2AgcFifoDataGen();
    puchReadFifoBuffer = g_uchSpo2AgcFifoBuffer;
    usFifoLen = g_usSpo2AgcFifoLen;
#endif


    GH3X_NewAgcMainChnlMeanValueCal( puchReadFifoBuffer,usFifoLen,unNewAgcRawdataMeanChnl,&usAdjFlag); // caculate mean and whther adjust gain  current 
//    GH3X_DEBUG_LOG_PARAM("AGC_Flag: AdjFlag = %d\r\n",usAdjFlag);
    for(GU8 uchSlotCnt = 0; uchSlotCnt < GH3X_NEW_AGC_SLOT_NUM_LIMIT; uchSlotCnt ++)
    {
         STGh3xNewAgcMainChnlInfo *pstAgcMainChnlInfo = &g_pstGh3xNewAgcMainChnlInfoEntity[uchSlotCnt];
         GU8  uchSlotIdx = ((pstAgcMainChnlInfo->uchSlotAdc >> 2) & 0x0f);
         if(pstAgcMainChnlInfo->uchBakRegAgcEn)// AGC enable
         {
             GH3X_StoreRXxDrvCurrentPreAgc(pstAgcMainChnlInfo->uchCurrentDrv, uchSlotIdx, g_puchDrvCurrentPreSoftAgc);
         }

         if( usAdjFlag & (1<<uchSlotCnt)) // need to AGC
         {
           uchChangeFlag = GH3X_NewAgcCalDrvCurrentAndGain(unNewAgcRawdataMeanChnl[uchSlotCnt], uchSlotCnt, &uchNewDrvCurrent,&uchNewGain);
           if(uchChangeFlag&0x02)  //write tia gain to chip
           {       
                GH3xSetAgcReg(GH3X_AGC_REG_TIA_GAIN_RX0 + pstAgcMainChnlInfo->uchBakRegAgcSrc, uchSlotIdx, uchNewGain);
                GH3X_DEBUG_LOG_PARAM("AGC MainChl Change gain Slotcfg=%d, AgcSrc=%d, NewGain=%d,PreGain=%d\r\n", uchSlotIdx,pstAgcMainChnlInfo->uchBakRegAgcSrc,uchNewGain,pstAgcMainChnlInfo->uchCurrentTiaGain);                 
                pstAgcMainChnlInfo->uchCurrentTiaGain = uchNewGain;       
           }

            if(uchChangeFlag&0x01)  //write drv current to chip
            {
                //change drv0/1/2/3
                pstAgcMainChnlInfo->uchCurrentDrv = uchNewDrvCurrent;
                for(GU8 i = 0; i < 4; i++)
                {     
                    if(pstAgcMainChnlInfo->uchBakRegAgcEn&(1 << i))
                    {    
                        GH3xSetAgcReg(GH3X_AGC_REG_LED_CURRENT_DRV0 + i, uchSlotIdx, uchNewDrvCurrent); 
                        GH3X_DEBUG_LOG_PARAM("[ SoftAGC Change Current] Slotcfg = %d, LED_x = %d, DrvCurrent = %d\r\n", uchSlotIdx, i, uchNewDrvCurrent); 
                    }
                }

#if GH3X_NEW_AGC_SUB_CHNL_NUM_LIMIT > 0
                GH3xNewAgcSubChnlClearAnalysisCntAndSetGain(uchSlotIdx, 0x0F, 0xFF); //clear all rx analysis cnt in this slotcfg
#endif
            }
            if(uchChangeFlag)
            {
                 g_pstGh3xNewAgcMainChnlMeanInfoEntity[uchSlotCnt].uchHaveAgcAtLastReadPrd = 1; // current or  gain change at last time
                 g_pstGh3xNewAgcMainChnlMeanInfoEntity[uchSlotCnt].uchDropFlag = 0;
                 NewAgcMainChelFlag |= (1<<uchSlotCnt); // ADjFlag write to CSV file
            }
         }
     }
}




/**
 * @fn  void GH3xNewAgcMeanInfoReset(GU8 uchSlotIdx)
 *
 * @brief agc mean calculation parameter reset
 *
 * @attention    None
 *
 * @param[in]    uchSlotIdx   slotcfg index 
 * @param[out]   None
 *
 * @return  None
 */
void GH3xNewAgcMeanInfoReset(GU8 uchSlotIdx)
{
    for(GU8 uchAgcSlotCnt = 0; uchAgcSlotCnt < GH3X_NEW_AGC_SLOT_NUM_LIMIT; uchAgcSlotCnt ++) //// uchAgcSlotCnt
    {
        GU8  uchSlotIdx_store = ((g_pstGh3xNewAgcMainChnlInfoEntity[uchAgcSlotCnt].uchSlotAdc >> 2) & 0x0f);
        GU8  uchEnable	= g_pstGh3xNewAgcMainChnlInfoEntity[uchAgcSlotCnt].uchBakRegAgcEn;
        if( uchEnable )  
        { 
           if( uchSlotIdx == uchSlotIdx_store)  
           {
               g_pstGh3xNewAgcMainChnlMeanInfoEntity[uchAgcSlotCnt].usAnalysisCnt = 0;
               #if GH3X_NEW_AGC_SUB_CHNL_NUM_LIMIT > 0
               GH3xNewAgcSubChnlClearAnalysisCntAndSetGain(uchSlotIdx, 0x0F, 0xFF); //clear all rx analysis cnt in this slot
               #endif 
           }
        }
    }
}    

/**
 * @fn  void GH3X_LedAgcReset(void)
 *
 * @brief   reset agc parameter
 *
 * @attention    None
 *
 * @param[in]    None
 * @param[out]   None
 *
 * @return  None
 */
void GH3X_LedAgcReset(void)
{
    GH3X_Memset(&g_stSoftAgcCfg, 0, sizeof(STSoftAgcCfg));
    GH3xNewAgcModuleReset();
}

/**
 * @fn     void GH3X_LedAgcPramWrite(GU16 usRegVal, GU16 usRegPosi)
 *
 * @brief  write AGC parameter
 *
 * @attention   None
 *
 * @param[in]   usRegVal         reg value
 * @param[in]   uchRegPosi         reg posi
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_LedAgcPramWrite(GU16 usRegVal, GU16 usRegPosi)
{

    GU16 usByteOffset;
    GU8 uchByteVal;
    GU8 uchByteCnt;
    for(uchByteCnt = 0; uchByteCnt < 2; uchByteCnt ++)
    {
        uchByteVal = ((usRegVal >> (8*uchByteCnt))&0x00FF);
        usByteOffset = (usRegPosi * 2) + uchByteCnt;
        if(usByteOffset < sizeof(g_stSoftAgcCfg))
        {
            GH3X_Memcpy(((GU8 *)(&g_stSoftAgcCfg)) + usByteOffset, (GU8*)(&uchByteVal), 1);
        }

        //All virtual registers copied complete print detection
        if((1 == uchByteCnt)&&(usRegPosi ==((GH3X_AGC_CFG0_RESTRAIN_THD_L_MSB_16_ADDR - GH3X_AGC_ADJUST_EN_ADDR) / GH3X_REG_ADDR_SIZE)))
        {
            GH3X_DEBUG_LOG_PARAM("uchSlotxSoftAgcAdjEn = 0x%X, uchSlotxSoftBgAdjEn = 0x%X\r\n",(int)g_stSoftAgcCfg.uchSlotxSoftAgcAdjEn,(int)g_stSoftAgcCfg.uchSlotxSoftBgAdjEn);
            GH3X_DEBUG_LOG_PARAM("uchAmbSampleEn = 0x%X, uchAmbSlot = 0x%X\r\n",(int)g_stSoftAgcCfg.stAmbSlotCtrl.uchAmbSampleEn,(int)g_stSoftAgcCfg.stAmbSlotCtrl.uchAmbSlot);
            GH3X_DEBUG_LOG_PARAM("stSoftAgcGainLimit = 0x%X,0x%X,0x%X,0x%X\r\n",g_stSoftAgcCfg.stSoftAgcGainLimit.uchGainLimitBg32uA,g_stSoftAgcCfg.stSoftAgcGainLimit.uchGainLimitBg64uA,g_stSoftAgcCfg.stSoftAgcGainLimit.uchGainLimitBg128uA,g_stSoftAgcCfg.stSoftAgcGainLimit.uchGainLimitBg256uA);
            GH3X_DEBUG_LOG_PARAM("unAgcTrigThd_H = %d,unAgcTrigThd_L = %d\r\n",(int)g_stSoftAgcCfg.unAgcTrigThd_H,(int)g_stSoftAgcCfg.unAgcTrigThd_L);
            GH3X_DEBUG_LOG_PARAM("unAgcRestrainThd_H = %d,unAgcRestrainThd_L = %d\r\n",(int)g_stSoftAgcCfg.unAgcRestrainThd_H,(int)g_stSoftAgcCfg.unAgcRestrainThd_L);
        }

    }
}

/**
 * @fn     static void GH3X_LedAgcInit()
 * 
 * @brief  init led agc
 *
 * @attention   just use to show function support
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_LedAgcInit(void)
{
    GH3xNewAgcChnlInfoInit();
}


/**
 * @fn     void GH3X_LedAgcProcess(GU8* puchReadFifoBuffer, GU16 usFifoLen)
 * 
 * @brief  led agc process function
 *
 * @attention   None
 *
 * @param[in]   puchReadFifoBuffer         pointer to read fifo data buffer
 * @param[in]   usFifoLen           read fifo data buffer length
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_LedAgcProcess(GU8* puchReadFifoBuffer, GU16 usFifoLen)
{

    if(usFifoLen==0)
    {
        return;
    }

    GH3X_NewAgcMainChnlPro(puchReadFifoBuffer, usFifoLen);

#if GH3X_NEW_AGC_SUB_CHNL_NUM_LIMIT > 0
    GH3X_NewAgcSubChnlGainAdj(puchReadFifoBuffer, usFifoLen);
#endif

}



#define GH3X_SOFT_AGC_VERSION "SoftAgc_V0.2.1.0"

char* GH3X2X_GetSoftAgcVersion(void)
{
    return GH3X_SOFT_AGC_VERSION;
}

#endif
/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
