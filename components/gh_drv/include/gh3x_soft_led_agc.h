/**
  ****************************************************************************************
  * @file    gh3x_soft_led_agc.h
  * @author  GHealth Driver Team
  * @brief   
  ****************************************************************************************
  * @attention  
  #####Copyright (c) 2022 GOODIX
   All rights reserved.
  
  ****************************************************************************************
  */
#ifndef _GH3X_DRV_SOFT_LED_AGC_H_
#define _GH3X_DRV_SOFT_LED_AGC_H_
 
#include "gh3x_drv.h"

extern GU8 g_puchDrvCurrentPreSoftAgc[];

//sub channel
#define GH3X_NEW_AGC_SUB_CHNNL_ANAYLSIS_TIME_SEC 1
#define GH3X_AGC_MainSwitch_ADDR                0x0ba8
// PPG_CFG register physical address
#define GH3X_AGC_PPGCFG0_ADDRESS                (0x0c28)  
#define GH3X_AGC_PPGCFG_ADRS_GAP                (28) 
#define GH3X_AGC_SLOTIDX_CTRL0_ADDRESS          (0x0c00)  
// The serial number of the specific register
#define GH3X_AGC_REG_AGC_EN                     0
#define GH3X_AGC_REG_LED_CURRENT_UP_LIMIT       1
#define GH3X_AGC_REG_LED_CURRENT_DOWN_LIMIT     2
#define GH3X_AGC_REG_LED_CURRENT_DRV0           3
#define GH3X_AGC_REG_LED_CURRENT_DRV1           4
#define GH3X_AGC_REG_LED_CURRENT_DRV2           5
#define GH3X_AGC_REG_LED_CURRENT_DRV3           6
#define GH3X_AGC_REG_TIA_GAIN_RX0               7
#define GH3X_AGC_REG_TIA_GAIN_RX1               8
#define GH3X_AGC_REG_TIA_GAIN_RX2               9
#define GH3X_AGC_REG_TIA_GAIN_RX3               10
#define GH3X_AGC_REG_AGC_SRC                    11
#define GH3X_AGC_REG_BG_CANCEL_RX0              12
#define GH3X_AGC_REG_BG_CANCEL_RX1              13
#define GH3X_AGC_REG_BG_CANCEL_RX2              14
#define GH3X_AGC_REG_BG_CANCEL_RX3              15
#define GH3X_AGC_REG_SR_MULIPLIPER              16
#define GH3X_AGC_PPG_RX_EN                      17


#define GH3X_AGC_ADJUST_EN_ADDR 0x11C0 
#define GH3X_AGC_CFG0_RESTRAIN_THD_L_MSB_16_ADDR 0x11D2
#define GH3X_FASTEST_SAMPLERATE_ADDR            0x000c

#define  GH3X_LED_AGC_DISABLE             (0)
#define  GH3X_LED_AGC_ENABLE              (1)

typedef struct
{
    GU16 uchAmbSampleEn  : 2;                /**< AMB sampling enable */
    GU16 uchAmbSlot      : 4;                /**< slot of amb sampling */
    GU16 uchRes          : 10;                /**< reserved */
} STAmbSlotCtrl;
 
 /**
 * @brief gain limit
 */
typedef struct
{
    GU16 uchGainLimitBg32uA  : 4;          /**< gain limit bg current */
    GU16 uchGainLimitBg64uA  : 4;          
    GU16 uchGainLimitBg128uA : 4;        
    GU16 uchGainLimitBg256uA : 4;
} STSoftAgcGainLimit;

 /**
  * @brief soft agc parameter
  */
typedef struct
{
    GU16 uchSlotxSoftAgcAdjEn;              /**< soft agc enable */
    GU16 uchSlotxSoftBgAdjEn;               /**< soft bg cancel adjust enable */
    STAmbSlotCtrl stAmbSlotCtrl;           /**< amb slot ctrl */
    STSoftAgcGainLimit stSoftAgcGainLimit; /**< soft gain limit */
    GU16 uchSpo2RedSlotCfgEn;              /**< Spo2 Slot flag */
    GU16 uchSpo2IrSlotCfgEn;
    GU16 unAgcTrigThd_H;                   /**< trig threshold(high) of soft agc */
    GU16 unAgcTrigThd_L;                   /**< trig threshold(low) of soft agc */
    GU16 unAgcRestrainThd_H;               /**< restrain threshold(high) of soft agc */
    GU16 unAgcRestrainThd_L;               /**< restrain threshold(low) of soft agc */
} STSoftAgcCfg;

typedef struct
{
    GU8 uchAddrOffset;
    GU8 uchMsb;
    GU8 uchLsb;
} STGh3xAgcReg;


typedef struct
{
    GU8  uchBakRegAgcEn;
    GU8  uchSpo2ChnlIndex;      //0: is not spo2 channel   N(N > 0):  is spo2 channl  
    GU8  uchSlotAdc;
    GU8  uchCurrentTiaGain;
    GU8  uchCurrentDrv;
    GU8  uchTiaGainUpLimit;
    GU8  uchHaveBakRegFlag;
    GU8  uchBakRegAgcSrc;
    GU8  uchBakRegLedCurrentMin;
    GU8  uchBakRegLedCurrentMax;
    GU8  uchBakTiaGain;
} STGh3xNewAgcMainChnlInfo;

typedef struct
{
    GU16 uchHaveAgcAtLastReadPrd;    //0: no agc action at last fifo read period  1: have agc action at last fifo read preiod
    GU16 uchDropFlag;
    GU16 usSampleRate;
    GU16 usAnalysisCnt;   // numble of nalysis data
    GU32 unMaxRawdata;
    GU32 unMinRawdata;
    GU32 unSumAdjust;     //  cacluate drv current and gain
    GU32 unSumTrige;      //  judge whether AGC
    GU32 unRawdataMean;
    GU32 unLastRawdataMean;
} STGh3xNewAgcMainChnlMeanInfo;


typedef struct
{
    GU8  uchIniteFirst;   // inite first time   0:yes   1:not
    GU8  uchSeconsCnt;    // 
    //GU8  uchLastValid;  	// continuously 15 seconds 
} STGh3xNewAgcMainChnlIdaelAdjInfo;


typedef struct
{
    GU8 uchEn;
    GU8 uchSlotAdcNo;
    GU8 uchCurrentGain;
    GU32 unLastRawdata;
    GU32 unLastLastRawdata;
    GU32 unRawdataSum;
    GU16 usAnalysisCnt;
    GU16 usMoreThanUpLimitPointCnt;
    GU16 usMoreThanUpLimitPointCntForBiggerGain;
    GU16 usTrigeUpLimitForBiggerGain;   //  rawdate/256
}STGh3xNewAgcSubChnlRxInfo;

/// led agc adc num max
#define  GH3X_AGC_ADC_NUM_MAX             (4u)

/// led agc gain limit number
#define  GH3X_AGC_LIMIT_NUM_MAX           (4u)

/// raw data base line value
#define  GH3X_DATA_BASE_LINE              (8388608)   //2^23 

/// sys sample rate, 32kHz
#define  GH3X_SYS_SAMPLE_RATE             (32000)
 
#define GH3X_BG_LEVEL_BIT_MASK            (0x0030)
#define GH3X_ADC_BG_CANCEL_32_UA          (0x0)
#define GH3X_ADC_BG_CANCEL_64_UA          (0x1)
#define GH3X_ADC_BG_CANCEL_128_UA         (0x2)
#define GH3X_ADC_BG_CANCEL_256_UA         (0x3)

/// amb slot select
#define GH3X_AGC_AMB_SLOT_DISABLE         (0x0)
#define GH3X_AGC_AMB_SLOT_USER_DEFINED    (0x1)
#define GH3X_AGC_AMB_SLOT_FOLLOW_CFG      (0x2)

/// makeup 3 bytes
#define   GH3X_MAKEUP_3BYTES(uchByte2, uchByte1, uchByte0)     ( ((((GU32)(uchByte2)) << 16) & 0x00FF0000U) |\
                                                                          ((((GU32)(uchByte1)) << 8) & 0x0000FF00U) |\
                                                                      (((GU32)(uchByte0)) & 0x000000FFU))

/// get bits data
#define GH3X_GET_REG_BITS_U16(reg_data, lsb, msb)       (((reg_data) & ((0xFFFF << (lsb)) & \
                                                            (0xFFFF >> (15 - (msb))))) >> (lsb))

/// set bits data
#define GH3X_SET_REG_BITS_U16(reg, lsb, msb, value)     ((reg) = (((reg) & ((0xFFFF << (lsb)) ^ \
                                                                  (0xFFFF >> (15 - (msb))))) | ((value) << (lsb))))

/// get adc gain
#define GH3X_GET_ADC_GAIN(reg_data, adc_no)             GH3X_GET_REG_BITS_U16((reg_data), ((adc_no) * 4), \
                                                                ((adc_no) * 4 + 3))

/// set adc gain
#define GH3X_SET_ADC_GAIN(reg_data, adc_no, gain)       GH3X_SET_REG_BITS_U16((reg_data), ((adc_no) * 4), \
                                                                ((adc_no) * 4 + 3), (gain))

/// get bg cancel i_dacfs
#define GH3X_GET_BGC_I_DACFS(reg_data, adc_no)          GH3X_GET_REG_BITS_U16((reg_data), ((adc_no) << 1), \
                                                                ((adc_no) * 2 + 1))

/// set bg cancel i_dacfs
#define GH3X_SET_BGC_I_DACFS(reg_data, adc_no, i_dacfs) GH3X_SET_REG_BITS_U16((reg_data), ((adc_no) * 2), \
                                                              ((adc_no) * 2 + 1), (i_dacfs))
                                                                                         
#define ABS(x) ((x) > 0 ? (x) : -(x))

/// transfer int to float
#define GH3X_TRANS_INT_TO_FLOAT           (1.0f)

/// agc calc seconds (250ms)
#define GH3X_AGC_CALC_SECONDS             (0.25f)

/// agc bgc donw cnt, 5s
#define GH3X_AGC_BGC_DOWN_CNT             (20)

/// slot tia gain val min
#define GH3X_AGC_GAIN_MIN_VAL             (0)

/// slot tia gain val max, 5s, equal (GH3X_SLOT_TIA_GAIN_VAL_MAX - 1)
#define GH3X_AGC_GAIN_MAX_VAL             (12)

/// Less than thr, include rawdata < thr
#define GH3X_AGC_EXCD_FLAG_LESS           (0x01)

/// greater than thr, include rawdatamix > thr | rawdata > thr
#define GH3X_AGC_EXCD_FLAG_GREATER        (0x06)


#define GH3X_AGC_CHECK_EN_BITS(x)         (((x) & (0x03 << 8)) != 0)      /**< check enable bits of ctrl3 */
#define GH3X_AGC_GET_SRC_BITS(x)          (((x) >> 6) & 0x03)             /**< get agc src bits of ctrl3 */
#define GH3X_AGC_CHECK_BG_2X2_BITS(x)     (((x) & (0x03 << 4)) == 0x03)   /**< check 2x2 bg lvl bits of ctrl3 */
#define GH3X_AGC_SET_SRC_BITS(x, n)       (((x) & 0xFF3Fu) | ((n) << 6u)) /**< set agc src bits of ctrl3 */

#define GH3X_AGC_GET_DONE_FAIL_BIT(x, i)  (((x) >> (i)) & 0x01)           /**< get tune done/fail bit of flag */
#define GH3X_AGC_REG_BIT(x, i)            (((x) >> (i)) & 0x01)           /**< get reg bit value */
#define GH3X_AGC_GET_RESULT_BITS(x, i)    (((x) >> (i)) & 0x07)           /**< get result bits of flag  */
#define GH3X_AGC_RESULT_BITS_SIZE         (3u)                           /**< one result 3 bits size of flag  */


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
void GH3X_LedAgcReset(void);


/**
 * @fn void GH3X_LedAgcInit(void)
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
void GH3X_LedAgcInit(void);

/**
 * @fn     void GH3X_LedAgc_Close(void)
 * 
 * @brief  close agc
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_LedAgc_Close(void);

/**
 * @fn     void GH3X_LedAgc_Open(void)
 * 
 * @brief  open agc
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_LedAgc_Open(void);

#endif  // _GH3X_DRV_SOFT_LED_AGC_H_

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
