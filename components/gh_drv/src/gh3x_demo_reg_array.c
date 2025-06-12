/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 * 
 * @file    gh3x_demo_regarray.c
 * 
 * @brief   gh3x driver lib reg config array
 * 
 * @author  Gooidx Iot Team
 * 
 */

#include "gh3x_demo_config.h"
#include "gh3x_demo_inner.h"

/// HR  SPO2
const STReg gh3x_reglist0[] =
{
{0x000c,0x001f},//RG_FASTEST_SAMPLE_RATE:31, 
{0x0020,0x8006},//RG_ADT_CTRL_CFG_NUM0:6 : ppg_cfg_6, RG_ADT_CTRL_ADC_NUM0:0:ADC0, RG_ADT_CTRL_CFG_NUM1:0 : ppg_cfg_0, RG_ADT_CTRL_ADC_NUM1:0:ADC0, 
{0x0022,0x01ff},//DATA_CHECK_EN:ppg_check_en|ppg_bg_check_en|temp_check_en|ecg_check_en|lead_check_en|pres_check_en|bia_check_en|gsr_check_en|precheck_check_en, 
{0x0026,0x0001},//RG_DATA_PACKET_EN:0: ppg, 
{0x0030,0x80f0},//FIFO_WATER_LINE:240, 
{0x0340,0x0233},//LEDDRV0_FS:3: 200mA, LEDDRV1_FS:3: 200mA, 
{0x0344,0x0233},//LEDDRV2_FS:3: 200mA, LEDDRV3_FS:3: 200mA, 
{0x0518,0x0001},//RG_PROX_EN:1, 
{0x051a,0xec3e},//WEAR_ON_THR:8907838, 
{0x051c,0x0087},//WEAR_ON_THR:8907838, 
{0x051e,0x5f0f},//WEAR_OFF_THR:8806159, 
{0x0520,0x0086},//WEAR_OFF_THR:8806159, 
{0x052c,0x0900},//COMFIRM TIME:0, COMFIRM TIME:9, 
{0x052e,0x0102},//RG_WEARON_AS0_SEL:2: PPG0, RG_WEARON_AS1_SEL:0: logic1, RG_WEARON_OS0_SEL:0: logic0, RG_WEARON_OS1_SEL:0: logic0, RG_WEARON_LOGIC_SEL:1: or, 
{0x0530,0x0102},//RG_WEAROFF_AS0_SEL:2: PPG0, RG_WEAROFF_AS1_SEL:0: logic1, RG_WEAROFF_OS0_SEL:0: logic0, RG_WEAROFF_OS1_SEL:0: logic0, RG_WEAROFF_LOGIC_SEL:1: or, 
{0x0602,0xcc00},//RG_FIFO_FULL_EN:1, RG_FIFO_OV_EN:1, RG_LED_TUNE_FAIL_EN:0, RG_LED_TUNE_DONE_EN:0, RG_WEARON_EN:1, RG_WEAROFF_EN:1, 
{0x0b90,0x0001},//RG_LED_ADJ_MODE:1: mode 1, 
{0x0c00,0x0100},//RG_SLOT_INDEX0:0 : ppg_cfg_0, RG_SLOT_INDEX1:1 : ppg_cfg_1, 
{0x0c02,0x0302},//RG_SLOT_INDEX2:2 : ppg_cfg_2, RG_SLOT_INDEX3:3 : ppg_cfg_3, 
{0x0c28,0x27b3},//slot cfg num:0,SAMPLE_RATE_DIVIDER:39, FIFO_EN:1, DCMT_EN:1, DCMT_MA_EN:1, DCMT_DNSAMPLE_EN:0, RX0_EN:1, RX1_EN:1, 
{0x0c2a,0x0609},//slot cfg num:0,RX0_PD_SEL:0:PD0|3:PD3, RX1_PD_SEL:1:PD1|2:PD2, 
{0x0c2c,0x0000},//slot cfg num:0,RX2_PD_SEL:, RX3_PD_SEL:, 
{0x0c2e,0x6105},//slot cfg num:0,BG CANCEL ENABLE:1, DC CANCEL ENABLE:0, ADC_INT:78.8us, LED_AGC_SRC:ADC0, LED_AGC_EN:drv0, 
{0x0c30,0x5555},//slot cfg num:0,RX0_TIA:250k, RX1_TIA:250k, RX2_TIA:250k, RX3_TIA:250k, 
{0x0c3a,0x0026},//slot cfg num:0,DRV0_CURRENT(mA):29.8039, DRV1_CURRENT(mA):0, 
{0x0c3e,0x0901},//slot cfg num:0,LED_SELECT:LED_0|LED_12, 
{0x0c40,0x1abf},//slot cfg num:0,ADJ_UP_LIMIT:150, ADJ_DOWN_LIMIT:20, 
{0x0c44,0x27b3},//slot cfg num:1,SAMPLE_RATE_DIVIDER:39, FIFO_EN:1, DCMT_EN:1, DCMT_MA_EN:1, DCMT_DNSAMPLE_EN:0, RX0_EN:1, RX1_EN:1, 
{0x0c46,0x0609},//slot cfg num:1,RX0_PD_SEL:0:PD0|3:PD3, RX1_PD_SEL:1:PD1|2:PD2, 
{0x0c48,0x0000},//slot cfg num:1,RX2_PD_SEL:, RX3_PD_SEL:, 
{0x0c4a,0x6105},//slot cfg num:1,BG CANCEL ENABLE:1, DC CANCEL ENABLE:0, ADC_INT:78.8us, LED_AGC_SRC:ADC0, LED_AGC_EN:drv0, 
{0x0c4c,0x5555},//slot cfg num:1,RX0_TIA:250k, RX1_TIA:250k, RX2_TIA:250k, RX3_TIA:250k, 
{0x0c56,0x0026},//slot cfg num:1,DRV0_CURRENT(mA):29.8039, DRV1_CURRENT(mA):0, 
{0x0c5a,0x0601},//slot cfg num:1,LED_SELECT:LED_4|LED_8, 
{0x0c5c,0x1abf},//slot cfg num:1,ADJ_UP_LIMIT:150, ADJ_DOWN_LIMIT:20, 
{0x0c60,0x27b3},//slot cfg num:2,SAMPLE_RATE_DIVIDER:39, FIFO_EN:1, DCMT_EN:1, DCMT_MA_EN:1, DCMT_DNSAMPLE_EN:0, RX0_EN:1, RX1_EN:1, 
{0x0c62,0x0102},//slot cfg num:2,RX0_PD_SEL:1:PD1, RX1_PD_SEL:0:PD0, 
{0x0c64,0x0000},//slot cfg num:2,RX2_PD_SEL:, RX3_PD_SEL:, 
{0x0c66,0x6808},//slot cfg num:2,BG CANCEL ENABLE:1, DC CANCEL ENABLE:0, ADC_INT:315.1us, LED_AGC_SRC:ADC0, LED_AGC_EN:drv3, 
{0x0c68,0x1111},//slot cfg num:2,RX0_TIA:15.625k, RX1_TIA:15.625k, RX2_TIA:15.625k, RX3_TIA:15.625k, 
{0x0c74,0x1a00},//slot cfg num:2,DRV3_CURRENT(mA):20.3922, 
{0x0c76,0x0440},//slot cfg num:2,LED_SELECT:LED_11, 
{0x0c78,0x1abf},//slot cfg num:2,ADJ_UP_LIMIT:150, ADJ_DOWN_LIMIT:20, 
{0x0c7c,0x27b3},//slot cfg num:3,SAMPLE_RATE_DIVIDER:39, FIFO_EN:1, DCMT_EN:1, DCMT_MA_EN:1, DCMT_DNSAMPLE_EN:0, RX0_EN:1, RX1_EN:1, 
{0x0c7e,0x0102},//slot cfg num:3,RX0_PD_SEL:1:PD1, RX1_PD_SEL:0:PD0, 
{0x0c80,0x0000},//slot cfg num:3,RX2_PD_SEL:, RX3_PD_SEL:, 
{0x0c82,0x6808},//slot cfg num:3,BG CANCEL ENABLE:1, DC CANCEL ENABLE:0, ADC_INT:315.1us, LED_AGC_SRC:ADC0, LED_AGC_EN:drv3, 
{0x0c84,0x1111},//slot cfg num:3,RX0_TIA:15.625k, RX1_TIA:15.625k, RX2_TIA:15.625k, RX3_TIA:15.625k, 
{0x0c90,0x1a00},//slot cfg num:3,DRV3_CURRENT(mA):20.3922, 
{0x0c92,0x0240},//slot cfg num:3,LED_SELECT:LED_7, 
{0x0c94,0x1abf},//slot cfg num:3,ADJ_UP_LIMIT:150, ADJ_DOWN_LIMIT:20, 
{0x0fc0,0x0528},//RG_INTER_SLOT_TMR:40, RG_LAST_SLOT_TMR:5, 
{0x1000,0x0002},//work mode:2, 
{0x1006,0xaa87},//timestamp low 16 bit  2024-02-27 17:25:27, 
{0x1008,0x65dd},//timestamp high 16 bit, 
{0x1144,0x0001},//*GSENSOR_CTRL:1: Enable, 
{0x11c0,0x000f},//*AGC_ADJUST_EN:0: Slot0 |1: Slot1 |2: Slot2 |3: Slot3 , 
{0x11c6,0x4567},//*GAIN_LIMIT_BG_32_UA:7, *GAIN_LIMIT_BG_64_UA:6, *GAIN_LIMIT_BG_128_UA:5, *GAIN_LIMIT_BG_256_UA:4, 
{0x11c8,0x000c},//*AGC_SPO2REDSLOTCFGEN:12, 
{0x1282,0x0019},//*FS:25, 
{0x1284,0x0019},//*FS:25, 
{0x128c,0x0019},//*FS:25, 
{0x1402,0x0004},//*HR_CHMAP_CNT:4, 
{0x1404,0x0010},//*HR_CHMAP_CH0:16, 
{0x1406,0x0014},//HR_CHMAP_CH01:20, 
{0x1408,0x0110},//HR_CHMAP_CH02:272, 
{0x140a,0x0114},//HR_CHMAP_CH03:276, 
{0x1504,0x0004},//*HRV_CHMAP_CNT:4, 
{0x1506,0x0010},//*HRV_CHMAP_CH0:16, 
{0x1508,0x0014},//HRV_CHMAP_CH01:20, 
{0x150a,0x0110},//HRV_CHMAP_CH02:272, 
{0x150c,0x0114},//HRV_CHMAP_CH03:276, 
{0x190c,0x0004},//*SPO2_CHMAP_CNT:4, 
{0x190e,0x0230},//*SPO2_CHMAP_CH0:560, 
{0x1910,0x0234},//SPO2_CHMAP_CH01:564, 
{0x1912,0x0320},//SPO2_CHMAP_CH02:800, 
{0x1914,0x0324},//SPO2_CHMAP_CH03:804, 
{0x75c0,0x0004},//*ALGO_CHNL_NUM:4, 
{0x75c4,0x0100},//*ALGO_GREEN_CHNL0:0, *ALGO_GREEN_CHNL1:1, 
{0x75c6,0x0302},//*ALGO_GREEN_CHNL2:2, *ALGO_GREEN_CHNL3:3, 
{0x84c0,0x0001},//*ALGO_CHNL_NUM:1, 
{0x84cc,0xff02},//*ALGO_IR_CHNL0:2, *ALGO_IR_CHNL1:255, 
{0x84d4,0xff00},//*ALGO_RED_CHNL0:0, *ALGO_RED_CHNL1:255, 
{0xffff,0x0001},//virtual reg, 



};

#if (__GH3X_CFG_LIST_MAX_NUM__ > 1)
//SPO2
const STReg gh3x_reglist1[] =
{
{0x0,0x000},
};
#endif /* (__GH3X_CFG_LIST_MAX_NUM__ > 1) */

#if (__GH3X_CFG_LIST_MAX_NUM__ > 2)
const STReg gh3x_reglist2[] =
{
    {0,0},
};
#endif /* (__GH3X_CFG_LIST_MAX_NUM__ > 2) */

#if (__GH3X_CFG_LIST_MAX_NUM__ > 3)
const STReg gh3x_reglist3[] =
{
    {0,0},
};
#endif /* (__GH3X_CFG_LIST_MAX_NUM__ > 3) */

#if (__GH3X_CFG_LIST_MAX_NUM__ > 4)
const STReg gh3x_reglist4[] =
{
    {0,0},
};
#endif /* (__GH3X_CFG_LIST_MAX_NUM__ > 4) */

#if (__GH3X_CFG_LIST_MAX_NUM__ > 5)
const STReg gh3x_reglist5[] =
{
    {0,0},
};
#endif /* (__GH3X_CFG_LIST_MAX_NUM__ > 5) */

#if (__GH3X_CFG_LIST_MAX_NUM__ > 6)
const STReg gh3x_reglist6[] =
{
    {0,0},
};
#endif /* (__GH3X_CFG_LIST_MAX_NUM__ > 6) */

#if (__GH3X_CFG_LIST_MAX_NUM__ > 7)
const STReg gh3x_reglist7[] =
{
    {0,0},
};
#endif /* (__GH3X_CFG_LIST_MAX_NUM__ > 7) */

const STInitConfig gstCfgListArr[__GH3X_CFG_LIST_MAX_NUM__] = 
{
    {gh3x_reglist0,sizeof(gh3x_reglist0) / sizeof(gh3x_reglist0[0])},
#if (__GH3X_CFG_LIST_MAX_NUM__ > 1)
    {gh3x_reglist1,sizeof(gh3x_reglist1) / sizeof(gh3x_reglist1[0])},
#endif /* (__GH3X_CFG_LIST_MAX_NUM__ > 1) */
#if (__GH3X_CFG_LIST_MAX_NUM__ > 2)
    {gh3x_reglist2,sizeof(gh3x_reglist2) / sizeof(gh3x_reglist2[0])},
#endif /* (__GH3X_CFG_LIST_MAX_NUM__ > 2) */
#if (__GH3X_CFG_LIST_MAX_NUM__ > 3)
    {gh3x_reglist3,sizeof(gh3x_reglist3) / sizeof(gh3x_reglist3[0])},
#endif /* (__GH3X_CFG_LIST_MAX_NUM__ > 3) */
#if (__GH3X_CFG_LIST_MAX_NUM__ > 4)
    {gh3x_reglist4,sizeof(gh3x_reglist4) / sizeof(gh3x_reglist4[0])},
#endif /* (__GH3X_CFG_LIST_MAX_NUM__ > 4) */
#if (__GH3X_CFG_LIST_MAX_NUM__ > 5)
    {gh3x_reglist5,sizeof(gh3x_reglist5) / sizeof(gh3x_reglist5[0])},
#endif /* (__GH3X_CFG_LIST_MAX_NUM__ > 5) */
#if (__GH3X_CFG_LIST_MAX_NUM__ > 6)
    {gh3x_reglist6,sizeof(gh3x_reglist6) / sizeof(gh3x_reglist6[0])},
#endif /* (__GH3X_CFG_LIST_MAX_NUM__ > 6) */
#if (__GH3X_CFG_LIST_MAX_NUM__ > 7)
    {gh3x_reglist7,sizeof(gh3x_reglist7) / sizeof(gh3x_reglist7[0])},
#endif /* (__GH3X_CFG_LIST_MAX_NUM__ > 7) */
};


