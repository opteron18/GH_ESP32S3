/**
  ****************************************************************************************
  * @file    gh3x_BiaSampleManager.c
  * @author  GHealth Driver Team
  * @brief   gh3x bia SampleManager functions
  ****************************************************************************************
  * @attention  
  #####Copyright (c) 2024 GOODIX
   All rights reserved.
  
  ****************************************************************************************
  */

#include <stdio.h>
#include <string.h>
#include "gh3x_demo_config.h"
#include "BiaSampleManager.h"
#include "impedance_algo.h"

#if __FUNC_TYPE_BIA_ENABLE__

static EM_BIA_SAMPLE_STATE uchBiaSampleState;
STImpedanceAglo stImpedanceAglo;


#if __BIA_IMPEDANCE_FIXER__

#define BIA_ZOPEN_NUM 4
complex_double gstZopen[BIA_ZOPEN_NUM];
    #ifdef  GR5526_SK  ////EVK platform 
    const complex_double gstZopenDefault[BIA_ZOPEN_NUM]={ // EVK Collection of sub-board without resistor (inserted sub-board but without resistor) 250K excitation
        {-6743.124078,-37129.83915+13.5},
        {-7141.660572,-37337.97977+13.5},
        {-6301.007541,-35453.96924+13.5},
        {-6814.532421,-36579.2365+13.5}
    };
    complex_double gstZopenOff[BIA_ZOPEN_NUM]={ // demo F7:3D 250K excitation 2uA X20 shied off
        {-6743.124078,-35129.83915+13.5},
        {-7141.660572,-35337.97977+13.5},
        {-6301.007541,-33453.96924+13.5},
        {-6814.532421,-34579.2365+13.5}   
    };
    complex_double gstZopenOn[BIA_ZOPEN_NUM]={ // demo F6E3 250K excitation 2uA X20 shied on (Collection without fixtures)
        {-5085.4,-28688.6+13.5},
        {-8060.8,-33499.2+13.5},
        {-3575.2,-27031.1+13.5},
        {-5937.3,-30011.7+13.5}    
    };
    #else
//    const complex_double gstZopenDefault[BIA_ZOPEN_NUM]={ // demo F5:E4 Collection of sub-board without resistor (inserted sub-board but without resistor) 250K excitation 2uA X20 shied on
//        {-4737.289821,-27437.18268+13.5},
//        {-6555.191459,-31787.31721+13.5},
//        {-2881.383941,-26391.5715+13.5},
//        {-4897.993935,-29149.79524+13.5}    
//    };
    //const complex_double gstZopenDefault[BIA_ZOPEN_NUM]={ // demo F6:f9 Collection of sub-board without resistor (inserted sub-board but without resistor) 250K excitation 2uA X20 shied on
    //    {-4839.302445,-28157.58342+13.5},
    //    {-6807.550256,-32662.22949+13.5},
    //    {-2933.385823,-26966.12278+13.5},
    //    {-5029.187888,-30033.98211+13.5}    
    //};
//    const complex_double gstZopenDefault[BIA_ZOPEN_NUM]={ // demo F7:3D Collection of sub-board without resistor (inserted sub-board but without resistor) 250K excitation 2uA X20 shied on
//        {-1812.1,-25183.5},
//        {-3852.2,-30523.3},
//        {-1752.4,-25494.8},
//        {-4245.7,-29317.7}    
//    };
//    complex_double gstZopenOn[BIA_ZOPEN_NUM]={ // demo F7:3D 250K excitation 2uA X20 shied on (Collection without fixtures)
//        {-5418.6,-30713.7+13.5},
//        {-8219.7,-36013.5+13.5},
//        {-2529.5,-27855.4+13.5},
//        {-6159.9,-32290.9+13.5}    
//    };
//    complex_double gstZopenOff[BIA_ZOPEN_NUM]={ // demo F7:3D 250K excitation 2uA X20 shied off (Collection without fixtures)
//        {-2619.6,-28071.1+13.5},
//        {-5452.2,-34000.5+13.5},
//        {-2396.4,-27794.8+13.5},
//        {-5944.7,-32287.5+13.5}    
//    };
    const complex_double gstZopenDefault[BIA_ZOPEN_NUM]={ // demo F5:70 Collection of sub-board without resistor (inserted sub-board but without resistor) 250K excitation 2uA X20 shied on
        {-1992.3,-24068.9},
        {-3906.9,-27932.1},
        {-2489.2,-23619.2},
        {-5431.1,-32356.4}
    };
    complex_double gstZopenOn[BIA_ZOPEN_NUM]={ // demo F6E3 250K excitation 2uA X20 shied on (Collection without fixtures)
        {-5085.4,-28688.6+13.5},
        {-8060.8,-33499.2+13.5},
        {-3575.2,-27031.1+13.5},
        {-5937.3,-30011.7+13.5}    
    };
    complex_double gstZopenOff[BIA_ZOPEN_NUM]={ // demo F6E3 250K excitation 2uA X20 shied off (Collection without fixtures)
        {-2300.2,-26446.0+13.5},
        {-5341.9,-32046.1+13.5},
        {-3226.0,-27011.5+13.5},
        {-5524.3,-29999.3+13.5}
    };
    //const complex_double gstZopenDefault[BIA_ZOPEN_NUM]={ // demo F8:98 Collection of sub-board without resistor (inserted sub-board but without resistor) 250K excitation 2uA X20 shied on
    //    {-1574.5,-24086.9},
    //    {-3914.3,-29329.9},
    //    {-1439,-23928.3},
    //    {-4305.4,-27771.4}    
    //};
    //const complex_double gstZopenDefault[BIA_ZOPEN_NUM]={ // demo F6DA Collection of sub-board without resistor (inserted sub-board but without resistor) 250K excitation 2uA X20 shied on
    //    {-1724.6,-22895.3},
    //    {-3363.9,-27494.5},
    //    {-111.4, -22535.8},
    //    {-4315.6,-26583.3}    
    //};
//    const complex_double gstZopenDefault[BIA_ZOPEN_NUM]={ // demo F679 Collection of sub-board without resistor (inserted sub-board but without resistor) 250K excitation 2uA X20 shied on
//        {-2207.9,-24285.8},
//        {-3292.1,-27824.5},
//        {-432.9,-23968.1},
//        {-4951.8,-28884.8}    
//    };
//    const complex_double gstZopenDefault[BIA_ZOPEN_NUM]={ // demo F6E3 Collection of sub-board without resistor (inserted sub-board but without resistor) 250K excitation 2uA X20 shied on
//        {-1343.4,-23453},
//        {-2871.6,-27652.6},
//        {-232.8,-22943.2},
//        {-5094.4,-28987.6}    
//    };
//    complex_double gstZopenOn[BIA_ZOPEN_NUM]={ // demo F6E3 250K excitation 2uA X20 shied on (Collection without fixtures)
//        {-5222.6,-27841.8+13.5},
//        {-6519.5,-31619.5+13.5},
//        {-71.4,   -25061.6+13.5},
//        {-7170.8,-31510.1+13.5}    
//    };
//    complex_double gstZopenOff[BIA_ZOPEN_NUM]={ // demo F6E3 250K excitation 2uA X20 shied off (Collection without fixtures)
//        {-2133.5,-25551.4+13.5},
//        {-3884.9,-29932.5+13.5},
//        {204.6,  -24944.4+13.5},
//        {-6640.4,-31586.9+13.5}    
//    };
    //const complex_double gstZopenDefault[BIA_ZOPEN_NUM]={ // demo F7B8 Collection of sub-board without resistor (inserted sub-board but without resistor) 250K excitation 2uA X20 shied on
    //    {-233.8,-22548.2},
    //    {-2782.6,-27509.4},
    //    {1349.7,-22181.3},
    //    {-4349.3,-28214.2}    
    //};
    #endif

#define BIA_ALOG_PARM_INDEX_ZOPEN  12
extern GS32 gnBiaPInfo[];
static void GhUpdateZopenParmFromConfig(void)
{
    if(gnBiaPInfo[BIA_ALOG_PARM_INDEX_ZOPEN])  //is valid
    {
        GH3X_SAMPLE_LOG_PARAM("[%s]: Zopen in config is ok, it will be used.\r\n", __FUNCTION__ );
        for(GU8 uchZcnt = 0; uchZcnt < BIA_ZOPEN_NUM; uchZcnt ++)
        {
            gstZopen[uchZcnt].real = ((double)gnBiaPInfo[BIA_ALOG_PARM_INDEX_ZOPEN + 2*uchZcnt + 0])/((double)1000);
            gstZopen[uchZcnt].imag = ((double)gnBiaPInfo[BIA_ALOG_PARM_INDEX_ZOPEN + 2*uchZcnt + 1])/((double)1000);
        }
    }
    else   //use
    {
        GH3X_SAMPLE_LOG_PARAM("[%s]: Zopen in config is invalid, default value will be used.\r\n", __FUNCTION__ );
        GH3X_Memcpy((void *)gstZopen, (void *)gstZopenDefault, sizeof(gstZopen));
    }

    for(GU8 uchZcnt = 0; uchZcnt < BIA_ZOPEN_NUM; uchZcnt ++)
    {
        GH3X_SAMPLE_LOG_PARAM("[%s]: gstZopen%d: i = %ld, q = %ld\r\n", __FUNCTION__ , (int)uchZcnt, (GS32)(gstZopen[uchZcnt].real*1000), (GS32)(gstZopen[uchZcnt].imag*1000));
    }
    
}
#endif

static EM_BIA_RET_STAET GH3X_BiaDriverStop(void)
{
    EM_BIA_RET_STAET ret = BIA_RET_OKAY; 
    GU16 usRgBiaEn = 0;
    usRgBiaEn = GH3X_ReadReg(RG_BIA_CTRL0);
    GH3X_WriteReg(RG_BIA_CTRL0 , usRgBiaEn & (~ RG_BIA_EN_MASK));
    if(GH3X_ReadReg(RG_BIA_CTRL0) != (usRgBiaEn & (~ RG_BIA_EN_MASK)))
    {
        ret = BIA_RET_REG_RW_ERROR;
    }
    //clear fifo: Disable first and then enable
    GU16 usFifoResetRegVal = 0;
    usFifoResetRegVal = GH3X_ReadReg(RG_FIFO_RESET_ADDR);
    GH3X_WriteReg(RG_FIFO_RESET_ADDR , usFifoResetRegVal & (~RG_FIFO_RESET_FIELD_MASK));
    GH3X_WriteReg(RG_FIFO_RESET_ADDR , usFifoResetRegVal);
    

    if(GH3X_ReadReg(RG_FIFO_RESET_ADDR) == usFifoResetRegVal)
    {
        ret = BIA_RET_REG_RW_ERROR;
    }
    return ret;
}

static EM_BIA_RET_STAET GH3X_BiaDriverStart(void)
{
    EM_BIA_RET_STAET ret = BIA_RET_OKAY;

    GU16 usFifoResetRegVal = 0;
    GU16 usRgBiaEn = 0;
    //clear fifo: Disable first and then enable
    usFifoResetRegVal = GH3X_ReadReg(RG_FIFO_RESET_ADDR);
    GH3X_WriteReg(RG_FIFO_RESET_ADDR , usFifoResetRegVal ^ RG_FIFO_RESET_FIELD_MASK);
    GH3X_WriteReg(RG_FIFO_RESET_ADDR , usFifoResetRegVal);

    if(GH3X_ReadReg(RG_FIFO_RESET_ADDR) != usFifoResetRegVal)
    {
        ret = BIA_RET_REG_RW_ERROR;
    }

    //start bia
    usRgBiaEn = GH3X_ReadReg(RG_BIA_CTRL0);
    GH3X_WriteReg(RG_BIA_CTRL0 , usRgBiaEn | RG_BIA_EN_MASK);
    if(GH3X_ReadReg(RG_BIA_CTRL0) != (usRgBiaEn | RG_BIA_EN_MASK))
    {
        ret = BIA_RET_REG_RW_ERROR;
    }
    return ret;
}


static GU16 s_bia_defualt_sltIsl0_4 = 0;  // The default power of the slot in the configuration
static GU16 s_bia_defualt_sltIsl5_7 = 0;  // The default power of the slot in the configuration
static GU16 s_bia_reg_ctrl0value = 0;     // default value of register 

static EM_BIA_RET_STAET GH3X_BiaDownloadOffsetConfigArr(void)
{
    EM_BIA_RET_STAET ret = BIA_RET_OKAY; 
    s_bia_defualt_sltIsl0_4 = GH3X_ReadReg(RG_BIA_SLT0_4_ISL_ADDR);
    // TX current   111(not use)
    GH3X_WriteReg(RG_BIA_SLT0_4_ISL_ADDR,BIA_ISL0_4_NOTUSE);
    if((GH3X_ReadReg(RG_BIA_SLT0_4_ISL_ADDR) != BIA_ISL0_4_NOTUSE))
    {
        ret =  BIA_RET_REG_RW_ERROR;
    }
    
    s_bia_defualt_sltIsl5_7 = GH3X_ReadReg(RG_BIA_SLT5_7_ISL_ADDR);
    // TX current   111 (not use)
    GH3X_WriteReg(RG_BIA_SLT5_7_ISL_ADDR, s_bia_defualt_sltIsl5_7|BIA_ISL5_7_NOTUSE);
    if((GH3X_ReadReg(RG_BIA_SLT5_7_ISL_ADDR) != (s_bia_defualt_sltIsl5_7|BIA_ISL5_7_NOTUSE)))
    {
        ret = BIA_RET_REG_RW_ERROR;
    }
    
    //set FrameNum and FrameLoopNum to  0 only execute in offset  
    s_bia_reg_ctrl0value = GH3X_ReadReg(RG_BIA_CTRL0);
    GH3X_WriteReg(RG_BIA_CTRL0 , s_bia_reg_ctrl0value&(~RG_BIA_FrameNum_MASK)); //set FrameNum to  0
    if(GH3X_ReadReg(RG_BIA_CTRL0) != (s_bia_reg_ctrl0value&(~RG_BIA_FrameNum_MASK)))
    {
        ret = BIA_RET_REG_RW_ERROR;
    }
    
    GU16 usBiaRegValue; 
    usBiaRegValue = GH3X_ReadReg(RG_BIA_REG0);
    GH3X_WriteReg(RG_BIA_REG0 , ((usBiaRegValue&(~RG_BIA_FrameLoopNum_MASK))) ); // set FrameLoopNum to 0 
    if(GH3X_ReadReg(RG_BIA_REG0) != (usBiaRegValue&(~RG_BIA_FrameLoopNum_MASK)))
    {
        ret = BIA_RET_REG_RW_ERROR;
    }
    
    usBiaRegValue = GH3X_ReadReg(RG_BIA_CTRL0);
    GH3X_WriteReg(RG_BIA_CTRL0 , BIA_SlotTime_512mS|(usBiaRegValue&(~RG_BIA_SlotTime_MASK))); // set slottime to 512ms
    if(GH3X_ReadReg(RG_BIA_CTRL0) != (BIA_SlotTime_1s|(usBiaRegValue&(~RG_BIA_SlotTime_MASK))))
    {
        ret = BIA_RET_REG_RW_ERROR;
    }
    
    //I Q offset config 0
    GH3X_WriteReg(RG_BIA_IQ_CALI_OFFSET_I_LOW_ADDR, 0);
    if(GH3X_ReadReg(RG_BIA_IQ_CALI_OFFSET_I_LOW_ADDR != 0))
    {
        ret = BIA_RET_REG_RW_ERROR;
    }
    GH3X_WriteReg(RG_BIA_IQ_CALI_OFFSET_I_HIGH_ADDR, 0);
    if(GH3X_ReadReg(RG_BIA_IQ_CALI_OFFSET_I_HIGH_ADDR) != 0)
    {
        ret = BIA_RET_REG_RW_ERROR;
    }

    GH3X_WriteReg(RG_BIA_IQ_CALI_OFFSET_Q_LOW_ADDR, 0);
    if(GH3X_ReadReg(RG_BIA_IQ_CALI_OFFSET_Q_LOW_ADDR) != 0)
    {
        ret = BIA_RET_REG_RW_ERROR;
    }
    GH3X_WriteReg(RG_BIA_IQ_CALI_OFFSET_Q_HIGH_ADDR, 0);
    if(GH3X_ReadReg(RG_BIA_IQ_CALI_OFFSET_Q_HIGH_ADDR) != 0)
    {
        ret = BIA_RET_REG_RW_ERROR;
    }

    //I Q gain config 0
    GH3X_WriteReg(RG_BIA_IQ_CALI_I_LOW_ADDR, 0);
    if(GH3X_ReadReg(RG_BIA_IQ_CALI_I_LOW_ADDR != 0))
    {
        ret = BIA_RET_REG_RW_ERROR;
    }
    GH3X_WriteReg(RG_BIA_IQ_CALI_I_HIGH_ADDR, 1);
    if(GH3X_ReadReg(RG_BIA_IQ_CALI_I_HIGH_ADDR) != 1 )
    {
        ret = BIA_RET_REG_RW_ERROR;
    }

    GH3X_WriteReg(RG_BIA_IQ_CALI_Q_LOW_ADDR, 0);
    if(GH3X_ReadReg(RG_BIA_IQ_CALI_Q_LOW_ADDR) != 0)
    {
        ret = BIA_RET_REG_RW_ERROR;
    }
    GH3X_WriteReg(RG_BIA_IQ_CALI_Q_HIGH_ADDR, 0);
    if(GH3X_ReadReg(RG_BIA_IQ_CALI_Q_HIGH_ADDR) != 0)
    {
        ret = BIA_RET_REG_RW_ERROR;
    }

    //hbd_start
    GH3X_BiaDriverStart();
    return ret;
}


static EM_BIA_RET_STAET GH3X_BiaDownloadExeConfigArr(void)
{
    EM_BIA_RET_STAET ret = BIA_RET_OKAY;
    // Default slot current write in configuration file
    GH3X_WriteReg(RG_BIA_SLT0_4_ISL_ADDR, s_bia_defualt_sltIsl0_4);
    if((GH3X_ReadReg(RG_BIA_SLT0_4_ISL_ADDR)) != s_bia_defualt_sltIsl0_4)
    {
        ret = BIA_RET_REG_RW_ERROR;
    }
    GH3X_WriteReg(RG_BIA_SLT5_7_ISL_ADDR, s_bia_defualt_sltIsl5_7);
    if((GH3X_ReadReg(RG_BIA_SLT5_7_ISL_ADDR)) != s_bia_defualt_sltIsl5_7)
    {
        ret = BIA_RET_REG_RW_ERROR;
    }
    // Write the configuration default value to the RG_BIA_CTRL0 register
    GH3X_WriteReg(RG_BIA_CTRL0, s_bia_reg_ctrl0value);
    if((GH3X_ReadReg(RG_BIA_CTRL0)) != s_bia_reg_ctrl0value)
    {
        ret = BIA_RET_REG_RW_ERROR;
    }
    
    GU16 usBiaRegValue; 
    usBiaRegValue = GH3X_ReadReg(RG_BIA_CTRL0);
//    if(BIA_MODE_Z_CONTACT==g_usBiaMeasureMode)
    {
        GH3X_WriteReg(RG_BIA_CTRL0,BIA_SlotTime_64mS|(usBiaRegValue&(~RG_BIA_SlotTime_MASK)));  //set SlotTime to 64mS
    }
        
    // set FrameLoopNum to 1：After starting the sampling, frame0 is run only once, and then it is run cyclically from frame1  
    usBiaRegValue = GH3X_ReadReg(RG_BIA_REG0);
    GH3X_WriteReg(RG_BIA_REG0,(BIA_FrameLoopNum_1|(usBiaRegValue&(~RG_BIA_FrameLoopNum_MASK)))|RG_BIA_FrameLoopEn);
    if(GH3X_ReadReg(RG_BIA_REG0) != (BIA_FrameLoopNum_1 |(usBiaRegValue&(~RG_BIA_FrameLoopNum_MASK))))
    {
        ret = BIA_RET_REG_RW_ERROR;
    }
    
    // QC charging time 10ms 
    usBiaRegValue = GH3X_ReadReg(RG_BIA_CTRL1);
    GH3X_WriteReg(RG_BIA_CTRL1,BIA_QcTime_10mS|(usBiaRegValue&(~RG_BIA_QcTime_MASK)));
    
    //hbd_start
    GH3X_BiaDriverStart();
    return ret;
}

static void BiaSampleManagerInit(void)
{
    STFrqInfo stFrqInfo;
    memset((&stFrqInfo), 0, sizeof(STFrqInfo));
     
    GU16 usBiaRegValue; 
    usBiaRegValue = GH3X_ReadReg(RG_BIA_CTRL0);
    GU8 uchSlotNum = ((usBiaRegValue&RG_BIA_SlotNum_MASK)>>12) + 1;
    if(uchSlotNum > BIA_FRQ_NUM_LIMIT)
    {
        GH3X_SAMPLE_LOG_PARAM("[%s]:SlotNum=%d,Warning SlotNum bigger than BIA_FRQ_NUM_LIMIT\r\n", __FUNCTION__,uchSlotNum);
        uchSlotNum = BIA_FRQ_NUM_LIMIT;
    }
    stFrqInfo.uchFrqPointNum = uchSlotNum;
    
    GU16 usSLT_FSL_ADDR[BIA_FRQ_NUM_MAX] = {RG_BIA_SLT0_1_FSL_ADDR,RG_BIA_SLT0_1_FSL_ADDR,RG_BIA_SLT2_6_FSL_ADDR,
        RG_BIA_SLT2_6_FSL_ADDR,RG_BIA_SLT2_6_FSL_ADDR,RG_BIA_SLT2_6_FSL_ADDR,RG_BIA_SLT2_6_FSL_ADDR,RG_BIA_SLT7_FSL_ADDR};
    GU8  uchSLT_FSL_RSHIFT[BIA_FRQ_NUM_MAX] = {9,12,0,3,6,9,12,0};
    // Excitation source frequency initialization
    GU16 usFtxArray[8] = {1,5,20,50,100,250,500,1000};
    for(GU8 i = 0;i < uchSlotNum; i++)
    {
        usBiaRegValue = GH3X_ReadReg( usSLT_FSL_ADDR[i]);
        stFrqInfo.usFrqValue[i] = usFtxArray[ ( (usBiaRegValue >> uchSLT_FSL_RSHIFT[i]) & RG_BIA_SLT_FSL_MASK) ]; 
    }
    
    #define BIA_INVALID_IMP_INDEX  0xFF
    STImpedanceChnlMap pstImpedanceNetChnlMapTemp[BIA_FRQ_NUM_LIMIT][BIA_FRAME_NUM_LIMIT];
    memset( (void*)pstImpedanceNetChnlMapTemp, BIA_INVALID_IMP_INDEX, sizeof(STImpedanceChnlMap)*BIA_FRQ_NUM_LIMIT*BIA_FRAME_NUM_LIMIT);
    
    for(GU8 i=0;i<uchSlotNum; i++)
    {
        for(GU8 j=0;j<BIA_FRAME_NUM_LIMIT; j++)
        {
#if __BIA_IMPEDANCE_FIXER__
            pstImpedanceNetChnlMapTemp[i][j].uchFrqIndex = i;            
            if(j< (BIA_FRAME_NUM_LIMIT-2))
            {
                if(j==0) //  NetIndex special settings in offset stage
                {
                   pstImpedanceNetChnlMapTemp[i][j].uchNetIndex = 0; 
                }
                else
                {
                   pstImpedanceNetChnlMapTemp[i][j].uchNetIndex = j-1;
                }
            }
            else
            {
                if(stFrqInfo.usFrqValue[i]<=5)
                {
                    pstImpedanceNetChnlMapTemp[i][BIA_FRAME_NUM_LIMIT-1].uchNetIndex = 4; // Zbody measurement link 5K band special processing
                }
                else
                {
                    pstImpedanceNetChnlMapTemp[i][BIA_FRAME_NUM_LIMIT-2].uchNetIndex = 4; // Zbody measurement link 
                }     
            }
#else
            pstImpedanceNetChnlMapTemp[i][j].uchFrqIndex = i;
            
            if(j==0) //  NetIndex special settings in offset stage
            {
               pstImpedanceNetChnlMapTemp[i][j].uchNetIndex = 0; 
            }
            else
            {
               pstImpedanceNetChnlMapTemp[i][j].uchNetIndex = j-1;
            }  
#endif
            
        }   
    }

    BiaImpedanceAlgoInit(&stImpedanceAglo);
    BiaImpedanceAlgoNetChnlMapCfg(&stImpedanceAglo,&stFrqInfo,&(pstImpedanceNetChnlMapTemp[0][0]));
}


void BiaSampleManagerSampleSwitch( EM_BIA_SAMPLE_STATE uchStateNext)
{
    
   if(BIA_SAMPLE_STATE_OFFSET==uchBiaSampleState && BIA_SAMPLE_STATE_CALI==uchStateNext)
   {
       //// After OFFSET is successful, stop sampling first and then issue the configuration.
       GH3X_BiaDriverStop();//STOP BIA  
       GH3X_BiaDownloadExeConfigArr( );
       uchBiaSampleState = BIA_SAMPLE_STATE_CALI; 
   }
   
   if(BIA_SAMPLE_STATE_CALI==uchBiaSampleState && BIA_SAMPLE_STATE_NET_IMEDANCE==uchStateNext)
   {
       uchBiaSampleState = BIA_SAMPLE_STATE_NET_IMEDANCE; 
   }

}


EM_BIA_RET_STAET GH3X_BiaStart(void)
{
    EM_BIA_RET_STAET emRet = BIA_RET_OKAY;
#if __BIA_IMPEDANCE_FIXER__
    GhUpdateZopenParmFromConfig();
#endif    
    BiaSampleManagerInit( );
    // Start data collecting, modify the configuration to offset configuration, and set the state to BIA_SAMPLE_STATE_OFFSET
    GH3X_BiaDownloadOffsetConfigArr( );
    uchBiaSampleState = BIA_SAMPLE_STATE_OFFSET;  
    return emRet;
}


EM_BIA_RET_STAET GH3X_BiaStop(void)
{
    EM_BIA_RET_STAET emRet = BIA_RET_OKAY;
    uchBiaSampleState = BIA_STATE_INIT;
    GH3X_BiaDriverStop();
    return emRet;
}

#endif
