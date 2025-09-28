/**
  ****************************************************************************************
  * @file    gh3x_drv.h
  * @author  GHealth Driver Team
  * @brief   gh3x driver functions
  ****************************************************************************************
  * @attention  
  #####Copyright (c) 2022 GOODIX
   All rights reserved.
  
  ****************************************************************************************
  */
#ifndef _GH3X_DRV_H_
#define _GH3X_DRV_H_

#include <stddef.h>

#ifndef UNUSED_VAR
#define UNUSED_VAR(X) ((void)(X))
#endif  // UNUSED_VAR

/* type redefine */
typedef unsigned char       GU8;    /**< 8bit unsigned integer type */
typedef signed char         GS8;    /**< 8bit signed integer type */
typedef unsigned short      GU16;   /**< 16bit unsigned integer type */
typedef signed short        GS16;   /**< 16bit signed integer type */
typedef int                 GS32;   /**< 32bit signed integer type */
typedef unsigned int        GU32;   /**< 32bit unsigned integer type */
typedef float               GF32;   /**< float type */
typedef double              GD64;   /**< double type */
typedef char                GCHAR;  /**< char type */
typedef unsigned char       GBOOL;  /**< bool type */
typedef long long           GS64;   /**< 64bit signed integer type */
typedef unsigned long long  GU64;   /**< 64bit unsigned signed integer type */

/* communicate interface macro */
#define __GH3X_INTERFACE_I2C__              (1)     /**< interface use i2c */
#define __GH3X_INTERFACE_SPI__              (2)     /**< interface use spi */

/* spi cs type macro */
#define __GH3X_SPI_TYPE_SOFTWARE_CS__       (1)     /** cs is software controled in spi module  **/
#define __GH3X_SPI_TYPE_HARDWARE_CS__       (2)     /** cs is hardtware controled in spi module **/

/* int process type macro */
#define __NORMAL_INT_PROCESS_MODE__         (0)     /** int event handle int mode **/
#define __POLLING_INT_PROCESS_MODE__        (1)     /** int event handle polling mode **/

/* application mode of driver lib */
#define __DRV_LIB_WITHOUT_ALGO__            (1)    /**< not use driver lib interface about algo */
#define __DRV_LIB_WITH_ALGO__               (2)    /**< use driver lib interface about algo */

/* algo section run mode*/
#define __SINGLE_THREAD_MODE__               (0)    /** section run in one thread **/
#define __MULTI_THREAD_MODE__                (1)    /** section run in two thread **/
#define __MULTI_CORE_MODE__                  (2)    /** section run in two core **/

/* log method*/
#define __EXAMPLE_LOG_DISABLE__             (0)
#define __EXAMPLE_LOG_METHOD_0__            (1)    /** need to achieve GH3X_Log, and relay on snprintf in <stdio.h> **/
#define __EXAMPLE_LOG_METHOD_1__            (2)    /** need register printf in <stdio.h> via GH3X_RegisterPrintf **/

/* log lv*/
#define __EXAMPLE_LOG_LV_0__                (0)    /** no log **/
#define __EXAMPLE_LOG_LV_1__                (1)    /** include EXAMPLE log(default) **/
#define __EXAMPLE_LOG_LV_2__                (2)    /** include EXAMPLE and soft adt log **/

#ifdef GOODIX_DEMO_PLATFORM
//#include "flash_log.h"
#define   GH3X_LOG_RECORD(...)              
#else
#define   GH3X_LOG_RECORD(...)   
#endif
/**
 * @brief universal protocol cmd enum
 */
typedef enum
{
    UPROTOCOL_CMD_START = 0x00,             /**< start */
    UPROTOCOL_CMD_STOP,                     /**< stop */
    UPROTOCOL_CMD_EVK_MODE,                 /**< evk work mode */
    UPROTOCOL_CMD_APP_MODE,                 /**< app work mode */
    UPROTOCOL_CMD_MCU_ONLINE_MODE,          /**< mcu online work mode */
    UPROTOCOL_CMD_MCU_OFFLINE_MODE,         /**< mcu offline work mode */
    UPROTOCOL_CMD_TEST_TUNE_MODE,           /**< test tune work mode */
    UPROTOCOL_CMD_PASS_THROUGH_MODE,
    UPROTOCOL_CMD_IRQ_STATUS_REPORT_ACK,    /**< irq status report ack */
    UPROTOCOL_CMD_MPT_MODE,
    UPROTOCOL_CMD_REPUBLISH,
    UPROTOCOL_CMD_IGNORE = 0xFF,            /**< ignore */
} EMUprotocolParseCmdType;

/**
 * @brief i2c addr (7bits) low two bit select enum
 */
enum EMGh3xI2cIdSelect
{
    GH3X_I2C_ID_SEL_1L0L = 0,     /**< i2c ID0 & ID1 pin low */
    GH3X_I2C_ID_SEL_1L0H,         /**< i2c ID0 pin high & ID1 pin low */
    GH3X_I2C_ID_SEL_1H0L,         /**< i2c ID0 pin low & ID1 pin high */
    GH3X_I2C_ID_SEL_1H0H,         /**< i2c ID0 & ID1 pin high */
    GH3X_I2C_ID_INVALID,          /**< invalid val */
};

/**
 * @brief gsensor sensitivity(counts/g) enum
 */
typedef enum
{
    GSENSOR_SENSITIVITY_512_COUNTS_PER_G = 0,   /**< sensitivity = 512/g */
    GSENSOR_SENSITIVITY_1024_COUNTS_PER_G,      /**< sensitivity = 1024/g */
    GSENSOR_SENSITIVITY_2048_COUNTS_PER_G,      /**< sensitivity = 2048/g */
    GSENSOR_SENSITIVITY_4096_COUNTS_PER_G,      /**< sensitivity = 4096/g */
    GSENSOR_SENSITIVITY_8192_COUNTS_PER_G,      /**< sensitivity = 8192/g */
} EMGsensorSensitivity;

/**
 * @brief slot index enable config enum
 */
typedef enum
{
    GH3X_SET_SLOT_ENABLE = 0,        /**< enable slot and last set slot enable */
    GH3X_SET_SLOT_DISABLE,           /**< disable slot by set */
    GH3X_SET_SLOT_DIRECT_ENABLE,     /**< only enable slot by set */
} EMSlotEnableConfigType;

/**
 * @brief irq mode config
 */
typedef enum 
{ 
    IRQ_MODE_LEVEL_HIGH = 0x00,         /**< int pin output high level */
    IRQ_MODE_LEVEL_LOW = 0x01,          /**< int pin output low level */
    IRQ_MODE_EDGE_RISING = 0x02,        /**< int pin output rising edge */
    IRQ_MODE_EDGE_FALLING = 0x03,       /**< int pin output falling edge */
} EMIrqModeConfig;



/**
 * @brief wear detect enable type enum
 */
typedef enum
{
    WEAR_DETECT_DISABLE = 0,   /**< wear detect disable */
    WEAR_DETECT_ENABLE,        /**< wear detect enable */
} EMWearDetectEnableType;

/**
 * @brief wear detect type enum
 */
typedef enum
{
    WEAR_DETECT_WEAR_ON = 0,   /**< detect wear on */
    WEAR_DETECT_WEAR_OFF,      /**< detect wear off */
} EMWearDetectType;

/**
 * @brief wear detect force switch enum
 */
typedef enum
{
    WEAR_DETECT_DONT_FORCE_SWITCH = 0,  /**< don't force switch */
    WEAR_DETECT_FORCE_SWITCH,           /**< force switch  */
} EMWearDetectForceSwitch;

/**
 * @brief skin color status enum
 */
typedef enum
{
    SKIN_COLOR_STATUS_DARK = 0,     /**< dark skin status */
    SKIN_COLOR_STATUS_BROWN,        /**< brown skin status */
    SKIN_COLOR_STATUS_YELLOW,       /**< yellow skin status */
    SKIN_COLOR_STATUS_FAIR,         /**< fair skin status */
} EMSkinColorStatusType;

/**
 * @brief register struct
 */
typedef struct
{
    GU32 usRegAddr;     /**< register address */
    GU16 usRegData;     /**< register val */
} STReg;

typedef struct
{
    GU32 unRegAddr;     /**< register address */
    GU8 uchLsb;     /**< register val */
    GU8 uchMsb;     /**< register val */
    GU16 usRegData;     /**< register val */
} STRegField;

/**
 * @brief init config struct type
 */
typedef struct
{
    const STReg *pstRegConfigArr;   /**< pointer of register config array */
    GU16        usConfigArrLen;     /**< register config array length */
} STInitConfig;

/**
 * @brief slot rawwdata struct type
 */
typedef struct
{
    GU32 (*punRawdata)[4];   /**< pointer of buffer store 4 adc rawdata, if slot no use, can set NULL */
    GU16 usRawdataCnt;       /**< slot rawdata count */
} STSlotRawdata;

/**
 * @brief get rawwdata struct type
 */
typedef struct
{
    GU8 *puchReadBuffer;                        /**< pointer of buffer for read rawdata */
    STSlotRawdata stSlotRawdataArr[8];    /**< 8 slot rawdata array */
} STRawdata;

/**
 * @brief gsensor rawwdata struct type
 */


/**
 * @brief channnel rawwdata struct type
 */
typedef struct
{
    GU32 *punChRawdataArr;              /**< pointer to buffer store channel data, data map e.g. ch0 ch1...chx ch0 */
    GU16 usChRawdataCnt;                /**< channel rawdata count output */
    GU32 *punIncompleteChRawdataArr;    /**< pointer to last incomplete packet buffer */
    GU32  unIncompleteChMark;           /**< last channel rawdata incomplete mark, if chx incomplete bit[x] set 1 */
} STChannelRawdata;

typedef struct
{
    GU8 *puchGhRawdataArr;              
    GU16 *pusGhRawdataCnt;               
    GS16 *psGsRawdataArr;              
    GU16 *pusGsRawdataCnt;            
} STSensorRawdata;

/**
 * @brief soft lead result
 */
typedef struct
{
    GU16 usEvent;                       /**< event */  
    GU8 uchNeedForceReadFifo;           /**< need force read fifo after call GH3X_LeadHandle(...) */
} STSoftLeadResult;

/**
 * @brief HB algorithm output result
 */
typedef struct
{
    GS32 snHbOut;                       /**< hb algorithm output value */
    GS32 snHbConfi;                     /**< hb algorithm output confidence */
    GS32 snHbSnr;                       /**< hb snr */
    GS32 snHbValidLv;                   /**< hb confidence valid level */
} STHbAlgoResult;

/**
 * @brief HRV algorithm output result
 */
typedef struct
{
    GS32 snHrvOut0;                     /**< hrv algorithm output 0 */
    GS32 snHrvOut1;                     /**< hrv algorithm output 1 */
    GS32 snHrvOut2;                     /**< hrv algorithm output 2 */
    GS32 snHrvOut3;                     /**< hrv algorithm output 3 */
    GS32 snHrvNum;                      /**< hrv effective result num of snHrvOut */
    GS32 snHrvConfi;                    /**< hrv algorithm output confidence */
} STHrvAlgoResult;

/**
 * @brief SPO2 algorithm output result
 */
typedef struct
{
    GS32 snSpo2Out;                     /**< spo2 algorithm output value */
    GS32 snSpo2Rvalue;                  /**< R value */
    GS32 snSpo2Confi;                   /**< confidence */
    GS32 snSpo2ValidLevel;              /**< valid level */
} STSpo2AlgoResult;

/**
 * @brief ECG algorithm output result
 */
typedef struct
{
    GS32 snEcgOut;                      /**< ecg algorithm output value */
    GS32 snEcgBpm;                      /**< bpm */
    GS32 snEcgSnr;                      /**< snr */
} STEcgAlgoResult;

/**
 * @brief algorithm result stuct
 */
typedef struct
{
    GS32* pnAlgoCalcResult;     //pointer to algorithm result array
    GU16* pusAlgoResIndex;      //pointer to frame index array corresponding to algorithm result
    GU16  usAlgoResCnt;         //number of algorithm result
    GU8   uchAlgoResSize;       //item number of every algorithm result
} STAlgoCalculateRes;

typedef struct
{
    GU32 unCapData[4];

}STCapRawdata;

typedef struct
{
    GU32 unTempData[4];
}STTempRawdata;

// algo chnl map struct
typedef struct
{
	GU8 uchFlag;
	GU8 uchNum;
	GU8 uchEcgAlgoChnlMap;
	GU8 uchAlgoChnlMap[32];
} STAlgoChnlMap;

typedef struct
{
    GU32 unSlotCfgMask;                       
    GU8 uchFatestRate;          
    GU8 puchDriverFs[4];
    GU16 usSlotIndex[16];
    GU16 usRunningSlotIndex[16];

} STPPGSlotInfo;

typedef struct
{
    GU8  uchLowestChnlSlotcfg;   // slot index of lowest channel 
    GU8  uchLowestChnlAdcNum;   // adc index of lowest channel 
    GU8  uchLowestChnlAppearedFlag;   //0:never appeared   1: appeared
    GU16 usNeedSkipFifoNum;      //the number which we need skip before formal monitor
    GU16 usAllChnlRealTimeSampleCnt;       //real time counter of sample of every slot between two lowest channels sample
    GU16 usEstiAllChnlSampleCntlimit;      //estimate down limit counter of sample of every slot between two lowest channels sample
    GU8  uchInitFlag;
} STGh3xFifoMonitorInfo;

#define GH3X_PPG_CLK                              (32000)

/* byte false or true */
#define BYTE_FALSE                                (0)             /**< byte false */
#define BYTE_TRUE                                 (1)             /**< byte true */

/* return code definitions list */
#define   GH3X_RET_READ_FIFO_CONTINUE             (4)             /**< Fifo not read empty,should read it again */
#define   GH3X_RET_RAW_DATA_PKG_OVER              (3)             /**< already read all of rawdata package data */
#define   GH3X_RET_RAW_DATA_PKG_CONTINUE          (2)             /**< there is rawdata package data left */
#define   GH3X_RET_ALGO_HAS_REFRESHED             (1)             /**< algorithm has refreshed */
#define   GH3X_RET_OK                             (0)             /**< there is no error */
#define   GH3X_RET_GENERIC_ERROR                  (-1)            /**< a generic error happens */
#define   GH3X_RET_PARAMETER_ERROR                (-2)            /**< parameter error */
#define   GH3X_RET_COMM_NOT_REGISTERED_ERROR      (-3)            /**< i2c/spi operation not registered error */
#define   GH3X_RET_COMM_ERROR                     (-4)            /**< i2c/spi communication error */
#define   GH3X_RET_RESOURCE_ERROR                 (-5)            /**< resource full or not available error */
#define   GH3X_RET_NO_INITED_ERROR                (-6)            /**< no inited error */
#define   GH3X_RET_ALGO_NO_SUPPORTED              (-7)            /**< the algorithm no supported error */
#define   GH3X_RET_LOW_TEMP_INVALID               (-8)            /**< No support ECG/BIA/GSR/BP while low temperature */

/* irq status mask. */
#define   GH3X_IRQ_MSK_PORT_ENABLE_BIT            (0x00000001)        /**< port enable */
#define   GH3X_IRQ_MSK_GSR_LEAD_ON_BIT            (0x00000002)        /**< gsr lead on */
#define   GH3X_IRQ_MSK_GSR_LEAD_OFF_BIT           (0x00000004)        /**< gsr lead off */
#define   GH3X_IRQ_MSK_ECG0_LEAD_ON_BIT           (0x00000008)        /**< ecg0 Lead on */
#define   GH3X_IRQ_MSK_ECG0_LEAD_OFF_BIT          (0x00000010)        /**< ecg0 Lead off */
#define   GH3X_IRQ_MSK_ECG1_LEAD_ON_BIT           (0x00000020)        /**< ecg1 Lead on */
#define   GH3X_IRQ_MSK_ECG1_LEAD_OFF_BIT          (0x00000040)        /**< ecg1 Lead off */
#define   GH3X_IRQ_MSK_CH0_FAST_RECOVERY_BIT      (0x00000080)        /**< ch0 fast recovery */
#define   GH3X_IRQ_MSK_CH1_FAST_RECOVERY_BIT      (0x00000100)        /**< ch1 fast recovery */
#define   GH3X_IRQ_MSK_ADC_DONE_BIT               (0x00000200)        /**< adc sampling done */
#define   GH3X_IRQ_MSK_FIFO_WATERMARK_BIT         (0x00000400)        /**< fifo watermark */
#define   GH3X_IRQ_MSK_FIFO_FULL_BIT              (0x00000800)        /**< fifo full */
#define   GH3X_IRQ_MSK_CURR_AGC_FAIL_BIT          (0x00001000)        /**< agc fail */
#define   GH3X_IRQ_MSK_CURR_AGC_OVER_BIT          (0x00002000)        /**< agc over */
#define   GH3X_IRQ_MSK_WEAR_ON_BIT                (0x00004000)        /**< wear on */
#define   GH3X_IRQ_MSK_WEAR_OFF_BIT               (0x00008000)        /**< wear off */
#define   GH3X_IRQ_MSK_SAMPLE_RATE_ERR_BIT        (0x00010000)        /**< sample rate error */
#define   GH3X_IRQ_MSK_CHIP_RESET_BIT             (0x00020000)        /**< chip reset */
#define   GH3X_IRQ_MSK_AFE_FULL_BIT               (0x00040000)        /**< afe full */
#define   GH3X_IRQ_MSK_PACE_CHECK_BIT             (0x00080000)        /**< pace check */
#define   GH3X_IRQ_MSK_LEAD_HEADROOM_ERR_BIT      (0x00100000)        /**< lead headroom error */
#define   GH3X_IRQ_MSK_PPG_AGC_FAIL_BIT           (0x00200000)        /**< ppg gain agc error */
#define   GH3X_IRQ_MSK_TIMER_IRQ_BIT              (0x00400000)        /**< timer irq */
#define   GH3X_IRQ_MSK_BIA_SLOT_OVER_BIT          (0x00800000)        /**< bia slot over */
#define   GH3X_IRQ_MSK_BG_CANCEL_FULL_BIT         (0x01000000)        /**< bg cancel full */
#define   GH3X_IRQ_MSK_PLL_READY_BIT              (0x02000000)        /**< pll ready */
#define   GH3X_IRQ_MSK_RLD_H                      (0x04000000)        /**< RLD_H irq */
#define   GH3X_IRQ_MSK_RLD_L                      (0x08000000)        /**< dfifo watermark*/
//#define   GH3X_IRQ_MSK_DFIFO_FULL_BIT             (0x10000000)        /**< dfifo full */

/* evnet ex*/
#define GH3X_EVENT_EX_BIT_WEAR_LIVING_TYPE  ((GU8)1<<0)     //0: object/no objiect   1: living/nonliving object
#define GH3X_EVENT_EX_BIT_CHIP0_LEAD_ON     ((GU8)1<<1)     //0: None                1: chip0 lead on
#define GH3X_EVENT_EX_BIT_CHIP0_LEAD_OFF    ((GU8)1<<2)     //0: None                1: chip0 lead off
#define GH3X_EVENT_EX_BIT_CHIP1_LEAD_ON     ((GU8)1<<3)     //0: None                1: chip1 lead on
#define GH3X_EVENT_EX_BIT_CHIP1_LEAD_OFF    ((GU8)1<<4)     //0: None                1: chip1 lead off
#define GH3X_EVENT_EX_BIT_RS0               ((GU8)1<<5)
#define GH3X_EVENT_EX_BIT_RS1               ((GU8)1<<6)
#define GH3X_EVENT_EX_BIT_RS2               ((GU8)1<<7)


/*some reg addr*/
#define   GH3X_INT_FIFO_UR_REG_ADDR               (0x050A)        /**< fifo use cnt */

/**
 * @brief channel map for function id
 */
#define GH3X_FUNC_OFFSET_ADT                      (0)    /**<0 ADT */
#define GH3X_FUNC_OFFSET_HR                       (1)    /**<1 HR */
#define GH3X_FUNC_OFFSET_HRV                      (2)    /**<2 HRV */
#define GH3X_FUNC_OFFSET_HSM                      (3)    /**<3 HSM */
#define GH3X_FUNC_OFFSET_FPBP                     (4)    /**<4 FPBP */
#define GH3X_FUNC_OFFSET_PWA                      (5)    /**<5 PWA */
#define GH3X_FUNC_OFFSET_SPO2                     (6)    /**<6 SPO2 */
#define GH3X_FUNC_OFFSET_ECG                      (7)    /**<7 ECG */
#define GH3X_FUNC_OFFSET_PWTT                     (8)    /**<8 PWTT */
#define GH3X_FUNC_OFFSET_SOFT_ADT_GREEN           (9)    /**<9 SOFT ADT GREEN */
#define GH3X_FUNC_OFFSET_BT                       (10)   /**<10 BT */
#define GH3X_FUNC_OFFSET_RESP                     (11)   /**<11 RESP */
#define GH3X_FUNC_OFFSET_AF                       (12)   /**<12 AF ADT */
#define GH3X_FUNC_OFFSET_TEST1                    (13)   /**<13 TEST1 */
#define GH3X_FUNC_OFFSET_TEST2                    (14)   /**<14 TEST2 */
#define GH3X_FUNC_OFFSET_SOFT_ADT_IR              (15)   /**<15 SOFT ADT IR */
#define GH3X_FUNC_OFFSET_BIA                      (16)   /**<16 BIA */
#define GH3X_FUNC_OFFSET_GSR                      (17)   /**<17 GSR */
#define GH3X_FUNC_OFFSET_LEAD                     (18)   /**< LEAD */
#define GH3X_FUNC_OFFSET_MAX                      (19)  

/* Function mode. */
#define   GH3X_NO_FUNCTION                        (0)                                           /**<0x00000000 NO function */
#define   GH3X_FUNCTION_ADT                       ((0x1) << (GH3X_FUNC_OFFSET_ADT))             /**<0x00000001 ADT function */
#define   GH3X_FUNCTION_HR                        ((0x1) << (GH3X_FUNC_OFFSET_HR))              /**<0x00000002 HR function */
#define   GH3X_FUNCTION_HRV                       ((0x1) << (GH3X_FUNC_OFFSET_HRV))             /**<0x00000004 HRV function */
#define   GH3X_FUNCTION_HSM                       ((0x1) << (GH3X_FUNC_OFFSET_HSM))             /**<0x00000008 HSM function */
#define   GH3X_FUNCTION_FPBP                      ((0x1) << (GH3X_FUNC_OFFSET_FPBP))            /**<0x00000010 FPBP function */
#define   GH3X_FUNCTION_PWA                       ((0x1) << (GH3X_FUNC_OFFSET_PWA))             /**<0x00000020 PWA function */
#define   GH3X_FUNCTION_SPO2                      ((0x1) << (GH3X_FUNC_OFFSET_SPO2))            /**<0x00000040 SPO2 function */
#define   GH3X_FUNCTION_ECG                       ((0x1) << (GH3X_FUNC_OFFSET_ECG))             /**<0x00000080 ECG function */
#define   GH3X_FUNCTION_PWTT                      ((0x1) << (GH3X_FUNC_OFFSET_PWTT))            /**<0x00000100 PWTT function */
#define   GH3X_FUNCTION_SOFT_ADT_GREEN            ((0x1) << (GH3X_FUNC_OFFSET_SOFT_ADT_GREEN))  /**<0x00000200 SOFT ADT GREEN function */
#define   GH3X_FUNCTION_BT                        ((0x1) << (GH3X_FUNC_OFFSET_BT))              /**<0x00000400 BT function */
#define   GH3X_FUNCTION_RESP                      ((0x1) << (GH3X_FUNC_OFFSET_RESP))            /**<0x00000800 RESP function */
#define   GH3X_FUNCTION_AF                        ((0x1) << (GH3X_FUNC_OFFSET_AF))              /**<0x00001000 AF function */
#define   GH3X_FUNCTION_TEST1                     ((0x1) << (GH3X_FUNC_OFFSET_TEST1))           /**<0x00002000 TEST1 function */
#define   GH3X_FUNCTION_TEST2                     ((0x1) << (GH3X_FUNC_OFFSET_TEST2))           /**<0x00004000 TEST2 function */
#define   GH3X_FUNCTION_SOFT_ADT_IR               ((0x1) << (GH3X_FUNC_OFFSET_SOFT_ADT_IR))     /**<0x00008000 SOFT ADT IR function */
#define   GH3X_FUNCTION_BIA                       ((0x1) << (GH3X_FUNC_OFFSET_BIA))             /**<0x00008000 BIA function */
#define   GH3X_FUNCTION_GSR                       ((0x1) << (GH3X_FUNC_OFFSET_GSR))             /**<0x00010000 GSR function */
#define   GH3X_FUNCTION_LEAD                      ((0x1) << (GH3X_FUNC_OFFSET_LEAD))            /**<0x00020000 LEAD function */


/* slot index . */
#define   GH3X_SLOT_INDEX_0                       (0x0001)        /**< slot index 0 */
#define   GH3X_SLOT_INDEX_1                       (0x0002)        /**< slot index 1 */
#define   GH3X_SLOT_INDEX_2                       (0x0004)        /**< slot index 2 */
#define   GH3X_SLOT_INDEX_3                       (0x0008)        /**< slot index 3 */
#define   GH3X_SLOT_INDEX_4                       (0x0010)        /**< slot index 4 */
#define   GH3X_SLOT_INDEX_5                       (0x0020)        /**< slot index 5 */
#define   GH3X_SLOT_INDEX_6                       (0x0040)        /**< slot index 6 */
#define   GH3X_SLOT_INDEX_7                       (0x0080)        /**< slot index 7 */
#define   GH3X_SLOT_INDEX_ALL                     (0x00FF)        /**< all slot */

/* led driver index . */
#define   GH3X_LED_DRV_INDEX_0                    (0x0000)        /**< led driver index 0 */
#define   GH3X_LED_DRV_INDEX_1                    (0x0001)        /**< led driver index 1 */

/* channel map to slot & adc for algorithm */
#define   GH3X_CHANNEL_MAP_SLOT0                  (0x00)          /**< channel map to slot 0 */
#define   GH3X_CHANNEL_MAP_SLOT1                  (0x20)          /**< channel map to slot 1 */
#define   GH3X_CHANNEL_MAP_SLOT2                  (0x40)          /**< channel map to slot 2 */
#define   GH3X_CHANNEL_MAP_SLOT3                  (0x60)          /**< channel map to slot 3 */
#define   GH3X_CHANNEL_MAP_SLOT4                  (0x80)          /**< channel map to slot 4 */
#define   GH3X_CHANNEL_MAP_SLOT5                  (0xA0)          /**< channel map to slot 5 */
#define   GH3X_CHANNEL_MAP_SLOT6                  (0xC0)          /**< channel map to slot 6 */
#define   GH3X_CHANNEL_MAP_SLOT7                  (0xE0)          /**< channel map to slot 7 */
#define   GH3X_CHANNEL_MAP_ADC0                   (0x00)          /**< channel map to adc 0 */
#define   GH3X_CHANNEL_MAP_ADC1                   (0x08)          /**< channel map to adc 1 */
#define   GH3X_CHANNEL_MAP_ADC2                   (0x10)          /**< channel map to adc 2 */
#define   GH3X_CHANNEL_MAP_ADC3                   (0x18)          /**< channel map to adc 3 */
#define   GH3X_CHANNEL_MAP(slot, adc)             ((slot) | (adc))    /**< calc channel map */

#define   GH3X_DEMO_WORK_MODE_EVK                 (0)
#define   GH3X_DEMO_WORK_MODE_APP                 (1)
#define   GH3X_DEMO_WORK_MODE_MCU_ONLINE          (2)
#define   GH3X_DEMO_WORK_MODE_MCU_OFFLINE         (3)
#define   GH3X_DEMO_WORK_MODE_TEST_TUNE           (4)
#define   GH3X_DEMO_WORK_MODE_PASS_THROUGH        (5)
#define   GH3X_DEMO_GET_WORK_MODE                 (6)
#define   GH3X_DEMO_WORK_MODE_MPT                 (7)
#define   GH3X_DEMO_WORK_MODE_ENGINEERING         (8)

/* soft event list*/
#define GH3X_SOFT_EVENT_NEED_FORCE_READ_FIFO      ((GU8)1)
#define GH3X_SOFT_EVENT_WEAR_OFF                  ((GU8)2)    //event generated by nadt algo
#define GH3X_SOFT_EVENT_NEED_TRY_READ_FIFO        ((GU8)4)
#define GH3X_SOFT_EVENT_WEAR_ON                   ((GU8)8)    //event generated by nadt algo
#define GH3X_SOFT_EVENT_WEAR_ON_ALGO_ADT          ((GU8)16)   //event generated by adt algo
#define GH3X_SOFT_EVENT_WEAR_OFF_ALGO_ADT         ((GU8)32)   //event generated by adt algo
/* adt confirm status */
#define GH3X_SENSOR_IS_NOT_MOVING                 ((GU8)0)
#define GH3X_SENSOR_IS_MOVING                     ((GU8)1)
/* soft nadt algo adt status */
#define GH3X_SOFT_ALGO_ADT_DEFAULT                 ((GU8)0)
#define GH3X_SOFT_ALGO_ADT_WEAR_ON                 ((GU8)1)    
#define GH3X_SOFT_ALGO_ADT_WEAR_OFF                ((GU8)2)

#define GH3X_ALGO_INFO_RECORD_GSENSOR_NUM               (3)
#define GH3X_ALGO_INFO_RECORD_RAWDARA_NUM               (4)
#define GH3X_ALGO_INFO_RECORD_AGC_INFO_NUM              (4)
#define GH3X_ALGO_INFO_RECORD_FALG_NUM                  (3)
#define GH3X_ALGO_INFO_RECORD_RESULT_NUM                (8)

#define GH3X_ALGO_RESULT_MAX_NUM                        (8)

/// universal protocol packet max len support
#define  GH3X_UPROTOCOL_PACKET_LEN_MAX                  (243)

#define  GH3X_UPROTOCOL_PACKET_HEADER_LEN               (4u)        /**< packet header len */
#define  GH3X_UPROTOCOL_PACKET_TAIL_LEN                 (1u)        /**< packet tail len */


/// packet header&tail len
#define  GH3X_UPROTOCOL_PACKET_HEADER_TAIL_LEN          (GH3X_UPROTOCOL_PACKET_HEADER_LEN + \
                                                            GH3X_UPROTOCOL_PACKET_TAIL_LEN)


/// universal protocol payload max len
#define  GH3X_UPROTOCOL_PAYLOAD_LEN_MAX                 (GH3X_UPROTOCOL_PACKET_LEN_MAX - \
                                                            GH3X_UPROTOCOL_PACKET_HEADER_TAIL_LEN)

/// Overflow protocol payload max len
#define  GH3X_UPROTOCOL_OVERFLOW_PAYLOAD_HEADER_LEN           (10)
#define  GH3X_UPROTOCOL_OVERFLOW_PAYLOAD_LEN_MAX              (220)

/// null val
#define   GH3X_PTR_NULL                                 ((void *) 0)

/* ecg sample event type */
#define  ECG_SAMPLE_EVENT_TYPE_SAMPLE                     (0x01)     /**< sample evt type sample config */
#define  ECG_SAMPLE_EVENT_TYPE_SLOT                       (0x02)     /**< sample evt type slot config */
#define  ECG_SAMPLE_EVENT_TYPE_LEAD                       (0x04)     /**< sample evt type lead irq */
#define  ECG_SAMPLE_EVENT_ALL                             (ECG_SAMPLE_EVENT_TYPE_SAMPLE | ECG_SAMPLE_EVENT_TYPE_SLOT | \
                                                            ECG_SAMPLE_EVENT_TYPE_LEAD)     /**< sample evt all */

/* ecg sample event type */
#define  ECG_SAMPLE_EVENT_INFO_SAMPLE_STOP                (0)      /**< sample evt info sample stop */
#define  ECG_SAMPLE_EVENT_INFO_SAMPLE_START               (1)      /**< sample evt info sample start */
#define  ECG_SAMPLE_EVENT_INFO_LEAD_OFF                   (0)      /**< sample evt info lead off */
#define  ECG_SAMPLE_EVENT_INFO_LEAD_ON                    (1)      /**< sample evt info lead on */


#define GH3X_ENGINEERING_MODE_CHNL_NUM_MAX      (32)

#define GH3X_ENGINEERING_MODE_INT_TIME_10_US    (0)
#define GH3X_ENGINEERING_MODE_INT_TIME_20_US    (1)
#define GH3X_ENGINEERING_MODE_INT_TIME_30_US    (2)
#define GH3X_ENGINEERING_MODE_INT_TIME_39_US    (3)
#define GH3X_ENGINEERING_MODE_INT_TIME_79_US    (4)
#define GH3X_ENGINEERING_MODE_INT_TIME_158_US   (5)
#define GH3X_ENGINEERING_MODE_INT_TIME_316_US   (6)

#define GH3X_ENGINEERING_MODE_TIA_GAIN_10_K     (0)
#define GH3X_ENGINEERING_MODE_TIA_GAIN_25_K     (1)
#define GH3X_ENGINEERING_MODE_TIA_GAIN_50_K     (2)
#define GH3X_ENGINEERING_MODE_TIA_GAIN_75_K     (3)
#define GH3X_ENGINEERING_MODE_TIA_GAIN_100_K    (4)
#define GH3X_ENGINEERING_MODE_TIA_GAIN_250_K    (5)
#define GH3X_ENGINEERING_MODE_TIA_GAIN_500_K    (6)
#define GH3X_ENGINEERING_MODE_TIA_GAIN_750_K    (7)
#define GH3X_ENGINEERING_MODE_TIA_GAIN_1000_K   (8)
#define GH3X_ENGINEERING_MODE_TIA_GAIN_1250_K   (9)
#define GH3X_ENGINEERING_MODE_TIA_GAIN_1500_K   (10)
#define GH3X_ENGINEERING_MODE_TIA_GAIN_1750_K   (11)
#define GH3X_ENGINEERING_MODE_TIA_GAIN_2000_K   (12)

#define GH3X_FRAME_FLAG_NUM                     (8)

extern int MemLog(const char * lpsbLog, ...);
#define MEM_LOG(...) // MemLog(__VA_ARGS__);
   
typedef struct
{
   GU32 unFunctionID;            //Function id     use maroc exp: GH3X_FUNCTION_HR
   GU8  uchSampleRateChangeEn;   // 0: do not need change sample rate   1: need change sample rate
   GU16 usSampleRate;            // sample rate (unit Hz  range 5Hz~1000Hz)
   GU8  uchIntTimeChangeEn;      // 0: do not need change integrator time  1: need change intergrator time
   GU8  uchIntTime;              // use maroc exp: GH3X_ENGINEERING_MODE_INT_TIME_10_US
   GU8  uchTiaGainChangeEn;      // 0: do not need change TIA gain    1: need change TIA gain 
   GU8  uchTiaGain[GH3X_ENGINEERING_MODE_CHNL_NUM_MAX];  //TIA gain for every channel, use maroc GH3X_ENGINEERING_MODE_TIA_GAIN_10_K
   GU8  uchLedCurrentChangeEn;      // 0: do not need change LED current    1: need change change LED current 
   GU8  uchLedDrv0Current[GH3X_ENGINEERING_MODE_CHNL_NUM_MAX];  //LED driver0 current for every channel (unit: mA   range: 0~200) 
   GU8  uchLedDrv1Current[GH3X_ENGINEERING_MODE_CHNL_NUM_MAX];  //LED driver1 current for every channel (unit: mA   range: 0~200)
} STEngineeringModeSampleParam;


STPPGSlotInfo* get_stPPGSlotInfo(void);

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
void GH3X_LedAgcProcess(GU8* puchReadFifoBuffer, GU16 usFifoLen);

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
void GH3X_LedAgcPramWrite(GU16 usRegVal, GU16 usRegPosi);

/**
 * @fn void GH3X_LedAgcInit()
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
void GH3xNewAgcMeanInfoReset(GU8 uchSlotIdx);

typedef struct
{
    GS32 snCounter;
    GS32 sniqCounter;
    long long snniVal;
    long long snnqVal;
    GU8 uchFlagFrPrev;
    GU8 uchFastRecoryHistory;
    long long snniqAmp[5];
} STSoftLeadOffDetInfo;
typedef struct
{
    GS32 snRsXbuf[250/5 + 1];
    GS32 snBufHead;
    GS32 snRsFirState;
    GU32 nRsIcounter;
    GU32 nRrsDcounter;
    GU8 uchFlagFrPrev;
} STRsInfo;

typedef struct
{
    GU8 uchChnlNum; //=0 disable
    GU8 uchMaxChnlNum;
    GU8 uchChnlIndex;
    GU16 usSampleRate;
    GU16 *pusChnlMap;
    GU32 *punRawdata;
    GU32 *punAgcInfo;
    GU64 unChnlBits;
    GU8 uchAgcNum;
}STGh3xData;

typedef struct
{
    GU8 uchFlagBits;
    GU32 *punFlag;
}STFlagInfo;

typedef struct
{
    GU8 uchEnableFlag;
    GS32 *pusData;
}STGsensorData;

typedef struct
{
    GU8 uchEnableFlag;
    GU8 uchFlag;
    GU8 uchResultNum;
    GU32 unResultBits;
    GS32 *pnResults;
}STAlgoData;

typedef struct
{
    GF32 fRawdataFs;
    GF32 fResampleFrameRest;//init to 0 when start sample
    GS16 sResampleFrameCnt; //init to 0 when get fifo data
    GS16 sRawdataFrameCnt;  //init to 0 when get fifo data
    GS16 sRawdataFrameNum;
    GS16 sResampleFrameNum;
    GS16 sAlgoCallCnt;
}STReSampleInfo;

typedef struct
{
    GU8 uchFuncID;
    GU32 *punFrameCnt;
    STFlagInfo *pstFlagInfo;
    STGh3xData *pstGh3xData;
    STGsensorData *pstGsData;
    STAlgoData *pstAlgoData;
    STReSampleInfo *pstReSampleInfo;
}STFrameInfo;

typedef struct
{
    GU32 uiAdcCode;                      //sampling rawdata of ADC
    GU8  ubSlotNo;                       //slot number
    GU8  ubAdcNo;                        //adc number
    GU8  ubFlagLedAdjIsAgc_EcgRecover;   //adj flag of ppg data or fast recover flag of ecg data
    GU8  ubFlagLedAdjAgcUp;            //adj down flag of ppg data   0: down  1:up
}StFifoDataInformation;

/**
 * @fn     GS8 GH3X_RegisterSpiOperationFunc(GU8 (*pSpiWriteFunc)(GU8 uchWriteBytesArr[], GU16 usWriteLen),
 *                                  GU8 (*pSpiReadFunc)(GU8 uchReadBytesArr[], GU16 usMaxReadLen),
 *                                   void (*pSpiCsCtrlFunc)(GU8 uchCsLevelHigh))
 *
 * @brief  Register spi operation function callback
 *
 * @attention   None
 *
 * @param[in]   pSpiWriteFunc       pointer to spi write function
 * @param[in]   pSpiReadFunc        pointer to spi read function
 * @param[in]   pSpiCsCtrlFunc      pointer to spi cs operation function
 * @param[out]  None
 *
 * @return  errcode
 * @retval  #GH3X_RET_OK               return successfully
 * @retval  #GH3X_RET_PARAMETER_ERROR  spi operation function pointer parameter error
 */
GS8 GH3X_RegisterSpiOperationFunc(GU8 (*pSpiWriteFunc)(GU8 uchWriteBytesArr[], GU16 usWriteLen),
                                    GU8 (*pSpiReadFunc)(GU8 uchReadBytesArr[], GU16 usMaxReadLen),
                                    void (*pSpiCsCtrlFunc)(GU8 uchCsLevelHigh));

/**
 * @fn     void GH3X_RegisterHookFunc(void (*pInitHookFunc)(void),
 *                           void (*pStartHookFunc)(void),
 *                           void (*pStopHookFunc)(void),
 *                           void (*pGetRawdataHookFunc)(GU8 *puchReadDataBuffer, GU16 usReadDataLen),
 *                           void (*pGetIoStructInfoHookFunc)(const STFrameInfo * const pstFrameInfo),
 *                           void (*pfnWriteAlgConfigWithVirtualReg)(GU16 usVirtualRegAddr, GU16 usVirtualRegValue))
 *
 * @brief  Register hook function callback
 *
 * @attention   None
 *
 * @param[in]   pInitHookFunc           pointer to init hook function 
 * @param[in]   pStartHookFunc          pointer to start hook function 
 * @param[in]   pStopHookFunc           pointer to stop hook function 
 * @param[in]   pGetRawdataHookFunc     pointer to get rawdata hook function
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_RegisterHookFunc(void (*pInitHookFunc)(void),
                             void (*pStartHookFunc)(void),
                             void (*pStopHookFunc)(void),
                             void (*pGetRawdataHookFunc)(GU8 *puchReadDataBuffer, GU16 usReadDataLen),
                             void (*pResetPinLevelControlFunc)(GU8 uchPinLevel),
                             void (*pPlatformDelayUsFunc)(GU16 usUsec),
                             void (*pPlatformDelayMsFunc)(GU16 usMsec));

/**
 * @fn     void *GH3X_Memset(void* pDest, GCHAR chVal, GU32 unByteSize)
 *
 * @brief  memset() Re-implementation
 *
 * @attention   None
 *
 * @param[in]   chVal       char val for set
 * @param[in]   unByteSize       source buffer len
 * @param[out]  pDest       pointer to destination buffer
 *
 * @return  pointer to destination buffer
 */
void *GH3X_Memset(void* pDest, GCHAR chVal, GU32 unByteSize);

/**
 * @fn     void *GH3X_Memcpy(void *pDest, const void *pSrc, GU32 unByteSize)
 *
 * @brief  memcpy() Re-implementation
 *
 * @attention   None
 *
 * @param[in]   pSrc        pointer to source buffer
 * @param[in]   unByteSize  source buffer byte size
 * @param[out]  pDest       pointer to destination buffer
 *
 * @return  pointer to destination buffer
 */
void *GH3X_Memcpy(void *pDest, const void *pSrc, GU32 unByteSize);

/**
 * @fn     GU32 GH3X_Strlen(const GCHAR *pszSrc)
 *
 * @brief  strlen() Re-implementation
 *
 * @attention   None
 *
 * @param[in]   pszSrc      pointer to string
 * @param[out]  None
 *
 * @return  string len
 */
GU32 GH3X_Strlen(const GCHAR *pszSrc);

/**
 * @fn     GS8 GH3X_ExitLowPowerMode(void)
 *
 * @brief  Exit lowpower mode, in this mode, can read&write reg with gh3x
 *
 * @attention   Power consumption bigger than lowpower mode, detaile ref gh3x datasheet
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  errcode
 * @retval  #GH3X_RET_OK               return successfully
 * @retval  #GH3X_RET_GENERIC_ERROR    exit low power mode error
 */
GS8 GH3X_ExitLowPowerMode(void);

/**
 * @fn     GS8 GH3X_EnterLowPowerMode(void)
 *
 * @brief  Enter lowpower mode, in this mode, can't read&write reg with gh3x.
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  errcode
 * @retval  #GH3X_RET_OK               return successfully
 * @retval  #GH3X_RET_GENERIC_ERROR    enter low power mode error
 */
GS8 GH3X_EnterLowPowerMode(void);

/**
 * @fn     GS8 GH3X_CommunicateConfirm(void)
 *
 * @brief  Communication operation interface confirm
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  errcode
 * @retval  #GH3X_RET_OK               return successfully
 * @retval  #GH3X_RET_COMM_ERROR       gh3x communicate error
 */
GS8 GH3X_CommunicateConfirm(void);

/**
 * @fn     void GH3X_SendCmd(GU8 uchCmd)
 *
 * @brief  Send cmd via i2c or spi
 *
 * @attention   None
 *
 * @param[in]   uchCmd      cmd
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_SendCmd(GU8 uchCmd);

/**
 * @fn     void GH3X_WriteReg(GU16 usRegAddr, GU16 usRegValue)
 *
 * @brief  Write register via i2c or spi
 *
 * @attention   None
 *
 * @param[in]   usRegAddr       register addr
 * @param[in]   usRegValue      register data
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_WriteReg(GU16 usRegAddr, GU16 usRegValue);
void GH3X_WriteRegs(GU32 unRegAddr, GU16 *usRegValueBuffer,GU8 uchRegNum);

/**
 * @fn     GU16 GH3X_ReadReg(GU16 usRegAddr)
 *
 * @brief  Read register via i2c or spi
 *
 * @attention   None
 *
 * @param[in]   usRegAddr      register addr
 * @param[out]  None
 *
 * @return  register data
 */
GU16 GH3X_ReadReg(GU16 usRegAddr);


/**
 * @fn     GU32 GH3X_ReadRegs(GU16 usRegAddr, GU16 *pusRegValueBuffer, GU16 usLen)
 *
 * @brief  Read multi register via i2c or spi
 *
 * @attention   None
 *
 * @param[in]   usRegAddr           register addr
 * @param[in]   pusRegValueBuffer   pointer to register value buffer
 * @param[in]   uchLen              registers len
 * @param[out]  None
 *
 * @return  None
 */
GU32 GH3X_ReadRegs(GU16 usRegAddr, GU16 *pusRegValueBuffer, GU16 usLen);

/**
 * @fn     void GH3X_ReadFifo(GU8 *puchDataBuffer, GU16 usLen)
 *
 * @brief  Read fifo via i2c or spi
 *
 * @attention   None
 *
 * @param[in]   puchDataBuffer      pointer to buffer for fifo rawdata
 * @param[in]   usLen               fifo bytes len
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_ReadFifo(GU8 *puchDataBuffer, GU16 usLen);

/**
 * @fn     void GH3X_WriteRegBitField(GU16 usRegAddr, GU8 uchLsb, GU8 uchMsb, GU16 usValue)
 *
 * @brief  write register bit field via i2c or spi
 *
 * @attention   None
 *
 * @param[in]   usRegAddr      register addr
 * @param[in]   uchLsb         lsb of bit field
 * @param[in]   uchMsb         msb of bit field
 * @param[in]   usRegValue     register data
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_WriteRegBitField(GU32 unRegAddr, GU8 uchLsb, GU8 uchMsb, GU16 usValue);

/**
 * @fn     GU16 GH3X_ReadRegBitField(GU16 usRegAddr, GU8 uchLsb, GU8 uchMsb)
 *
 * @brief  Read register bit field via i2c or spi
 *
 * @attention   None
 *
 * @param[in]   usRegAddr      register addr
 * @param[in]   uchLsb         lsb of bit field
 * @param[in]   uchMsb         msb of bit field
 * @param[out]  None
 *
 * @return  register bit field data
 */
GU16 GH3X_ReadRegBitField(GU16 usRegAddr, GU8 uchLsb, GU8 uchMsb);

void GH3X_FunctionProcess(STSensorRawdata* pstSensorRawdata);

/**
 * @fn     GCHAR *GH3X_GetDriverLibVersion(void)
 *
 * @brief  Get driver version
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  library version string
 */
GCHAR *GH3X_GetDriverLibVersion(void);
GCHAR *GH3X_GetProtocolVersion(void);
GCHAR *GH3X_GetVirtualRegVersion(void);

/**
 * @fn     GS8 GH3X_SoftReset(void)
 *
 * @brief  GH3X_ softreset via i2c/spi, can read&write reg with gh3x after reset 
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  errcode
 * @retval  #GH3X_RET_OK               return successfully
 * @retval  #GH3X_RET_GENERIC_ERROR    exit low power mode error
 */
GS8 GH3X_SoftReset(void);

/**
 * @fn     GS8 GH3X_HardReset(void)
 *
 * @brief  GH3X_ softreset via i2c/spi, can read&write reg with gh3x after reset
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  errcode
 * @retval  #GH3X_RET_OK               return successfully
 * @retval  #GH3X_RET_GENERIC_ERROR    reset pin control function not register
 */
GS8 GH3X_HardReset(void);
GS8 GH3X_PING_SET(GU8 ping_set);
/**
 * @fn     GS8 GH3X_Init(const STInitConfig *pstInitConfigParam)
 *
 * @brief  Init gh3x with configure parameters
 *
 * @attention   None
 *
 * @param[in]   pstInitConfigParam      pointer to gh3x init config param
 * @param[out]  None
 *
 * @return  errcode
 * @retval  #GH3X_RET_OK               return successfully
 * @retval  #GH3X_RET_COMM_ERROR       gh3x communicate error
 */
GS8 GH3X_Init(const STInitConfig *pstInitConfigParam);

/**
 * @fn     void GH3X_SetMaxNumWhenReadFifo(GU16 usMaxNum)
 *
 * @brief  Set max number of rawdata read from fifo every time
 *
 * @attention   None.
 *
 * @param[in]   usMaxNum        max number of rawdata read from fifo
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_SetMaxNumWhenReadFifo(GU16 usMaxNum);

/**
 * @fn     GU32 GH3X_GetSoftEvent(void)
 * @brief  Get soft event
 * @param[in]   None
 * @param[out]  Soft event
 *
 * @return  soft event
 */
GU32 GH3X_GetSoftEvent(void);

/**
 * @fn     void GH3X_SetSoftEvent(GU32 unEvent)
 * @brief  set soft event
 * @param[in]   uchEvent
 * @param[out]  None
 * @return  None
 */
void GH3X_SetSoftEvent(GU32 unEvent);

/**
 * @fn     GU32 GH3X_GetIrqStatus(void)
 *
 * @brief  Get irq status reg val
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  irq status val, ref irq status mask
 */
GU32 GH3X_GetIrqStatus(void);


/**
 * @fn     GU8 GH3X_GetGsensorEnableFlag(void)
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
GU8 GH3X_GetGsensorEnableFlag(void);

/**
 * @fn     GS16 GH3X_ReadFifodata(GU8 *puchReadFifoData, GU16* pusReadFifoDataLen)
 *
 * @brief  Read GH3X_ Fifo Data
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  puchReadFifoData      pointer to GH3X_ fifo data
 * @param[out]  pusReadFifoDataLen          pointer to GH3X_ fifo data length in byte
 *
 * @return  error code
 * @retval  #GH3X_RET_OK                      return read fifo successful
 * @retval  #GH3X_RET_PARAMETER_ERROR         return param error
 * @retval  #GH3X_RET_READ_FIFO_CONTINUE      return fifo is not empty
 */
GS16 GH3X_ReadFifodata(GU8 *puchReadFifoData, GU16* pusReadFifoDataLen);

/**
 * @fn     GS8 GH3X_DecodeRegCfgArr(GU32* punRunMode, const STReg *pstRegConfigArr, GU16 usRegConfigLen)
 *
 * @brief  Analyze reg cfg array to get the run mode.
 *
 * @attention   None
 *
 * @param[in]   pstRegConfigArr             pointer to reg config array
 * @param[in]   usRegConfigLen              reg config array length
 * @param[out]  punRunMode                  pointer to run mode contained in reg config array
 *
 * @retval  #GH3X_RET_OK                      return successfully
 * @retval  #GH3X_RET_PARAMETER_ERROR         return parameter error
 */
GS8 GH3X_DecodeRegCfgArr(GU32* punRunMode, const STReg *pstRegConfigArr, GU16 usRegConfigLen);

/**
 * @fn     void GH3X_NormalizeGsensorSensitivity(GU16 * gsensor_buffer[], GU16 gsensor_point_num) 
 *
 * @brief  normalize gsensor sensitivity to  512LSB/g
 *
 * @attention   None
 *
 * @param[in]    gsensor_buffer          gsensor rawdata buffer
 * @param[in]   gsensor_point_num     gsensor rawdata point num
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_NormalizeGsensorSensitivity(GS16 gsensor_buffer[], GU16 gsensor_buffer_len);

GS8 GH3X_FunctionStart(const STFrameInfo * const pstFrameInfo);

GS8 GH3X_FunctionStop(const STFrameInfo * const pstFrameInfo);
void GH3x_CalFunctionEnBit(const STFrameInfo * const pstFrameInfo,GU64* punSlotBit,GU8* pEcgEnBit,GU8* pBiaEnBit,GU8* pGsrEnBit);
GU32 GH3x_SetSlotEn(GU64 unSlotCfg, GU8 uchStart);
GS32 GH3x_WaitPllReady(GS32 timeOut_ms);
void GH3x_SetIaEn(GU8 uchEcgEn,GU8 uchBiaEn,GU8 uchGsrEn,GU8 uchStart);
void GH3X_SetAdtConfirmPara(GU8 uchMoveThreshold, GU16 usMoveCntThreshold, GU16 usNotMoveCntThreshold);
void GH3X_ResetMoveDetectByGsData(void);
GU8 GetGsConfirmAdtEn(void);
void GH3X_WearEventProcess(GU32* punGotEvent, GU8* puchExentEx);
void GH3X_SetWearStatusReg(GU8 uchWearStatus);
GU32 GH3X_GetProtocolTargetFunction(void);
void GH3X_LeadEventProcess(GU32* punGotEvent);
void GH3X_WriteRegBitFields(const STRegField stRegFields[], GU16 usFieldNum);
void GH3X_SamplingControlHookProcess(GU32 unFuncMode, EMUprotocolParseCmdType emSwitch);
void GH3X_ChangeFifoEndian(GU8 *puchRawdataBuf,int len);
void GH3X_SetDemoWorkMode(GU8 uchDemoWorkMode);
void GH3X_SetProtocolTargetFunction(GU32 unTargetFuncMode);

GU8 GH3X_GetGsTimerStatus(void);
void GH3X_SetGsTimerStatus(GU8 uchStatus);
void SetPPGSlotInfo(void);
void GH3X_SlotControl(GU32 unSlotCfgNumBits, GU8 uchStart);
void GH3X_SetSlotSampleRate(GU32 unslotCfgNum, GU32 unSampleRate);
void GH3X_SetLedCurrent(GU32 unslotCfgNum, double currentmA);
void GhHardAgcDisable(void);
void GH3X_SetSlotIntTime(GU32 unslotCfgNum, GU8 uchIntTime);
void GH3X_SetSlotGain(GU32 unslotCfgNum, GU8 uchAdcNo,GU8 uchGain);
void GH3X_UprotocolSendChipEvent(GU32 unEvent);
GS32 GH3X_u24toint24(GU32 unU24Val);
void Gh3xFifoMonitorInfoInit(void);
void Gh3xFifoMonitorInfoDeInit(void);
GU8 GhPopCount(GU64 x);
GS8 gh_read_tia_gain_error(void);
GS8 gh_read_led_drv_gain_error(void);
GS8 gh_read_dc_cancel_error(void);
GS8 gh_read_bg_cancel_error(void);
GS8 gh_read_cp_uid(void);
/**
 * @fn     void GH3X_SlotEnRegSet(GU8 uchSetValue)
 *
 * @brief  Slot enable reg set
 *
 * @attention  None
 *
 * @param[in]  set value
 * @param[out]  None
 * 
 * @return  None
 */
void GH3X_SlotEnRegSet(GU8 uchSetValue);

/**
 * @fn     GS8 GH3X_FifoWatermarkThrConfig(GU16 usFifoWatermarkThr)
 *
 * @brief  Fifo water mark threshold config
 *
 * @attention   Watermark threshold val must in (2, 800]. if val <= 2, will set 3, if val > 800, set 800;
 *              Be careful that fifo_use_cnt greater than val, gh3x willn't generate fifo_watermark interrupt after!
 *
 * @param[in]   usFifoWatermarkThr         watermark threshold val
 * @param[out]  None
 *
 * @return  errcode
 * @retval  #GH3X_RET_OK               return successfully
 */
GS8 GH3X_FifoWatermarkThrConfig(GU16 usFifoWatermarkThr);

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
GS8 GH3X_LoadNewRegConfigArr(const STReg *pstRegConfigArr, GU16 usRegConfigLen , GU8 uchChipIndex);

/**
 * @fn     GCHAR *GH3X_GetChipVersion(void)
 *
 * @brief  Get chip version, only for protocol, format like v11_ev01(11: chip id, 01 efuse ver)
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  chip version string
 */
GCHAR *GH3X_GetChipVersion(void);

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
void GH3X_WriteVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue);

/**
 * @fn     void GH3X_ClearSoftEvent(GU32 unEvent)
 * @brief  clear soft event
 * @param[in]   uchEvent
 * @param[out]  None
 * @return  None
 */
void GH3X_ClearSoftEvent(GU32 unEvent);

void GH3X_DelayUs(GU16 usUsCnt);
void GH3X_DelayMs(GU16 usUsCnt);
extern GU32 GH3X_GetDemoFuncMode(void);
GU8 GH3X_GetNeedWakeUp(void);
void GH3X_SetNeedWakeUp(GU8 uchNeedWakeUp);
void GH_IntfLock(void);
void GH_IntfUnLock(void);
void GH_SendDataLock(void);
void GH_SendDataUnLock(void);
void GHProtocalLock(void);
void GHProtocalUnLock(void);
GU8 GH3X_BitCount(GU32 n);
void GH3X_UprotocolSendFifoData(GU8 *puchReadRawdataBuffer, GU16 usReadRawdataLen, GU32 unFifoFull);
void GH3X_EventBufWrite(GU32 unEvent);
void GH3X_LoadConfigDoneHook(void);
void GhFifoDataHandle(GU32 unGotEvent, STSensorRawdata* pstSensorRawdata);
void GhUploadChipEvent(GU32 unGotEvent);
void GhLedAgcProcess(GU8* puchReadRawdataBuffer, GU16 usReadRawdataLen);
void GhWearEventHandle(GU32* punGotEvent,GU8* puchEventEx);
void GhConvertFifoEndian(GU8* puchReadRawdataBuffer, GU16 usReadRawdataLen);
GU8 GH3X_ExistPace(const STFrameInfo * const pstFrameInfo);
const STFrameInfo * GetstFrameInfo(GU8 uchFuncOffset);
GS8 gh_ia_filter(void);
GS8 gh_read_efuse(GU8 efuse_start_byte_addr, GU8 efuse_data[], GU16 efuse_data_byte_len);

/*********************************************************************************************/
/*********************************************************************************************/
/*******************************  DO NOT MODIFY FOLLOWING CODE *******************************/
/*********************************************************************************************/
/*********************************************************************************************/

#if defined ( __GNUC__ ) && !defined (__CC_ARM) /* GNU Compiler */
#ifndef __weak
    #define __weak   __attribute__((weak))
  #endif /* __weak */
#ifndef __packed
    #define __packed __attribute__((__packed__))
  #endif /* __packed */
#endif /* __GNUC__ */

/* extern log function */
extern int (*gpPrintfUser)(const char *format, ...);
extern int (*gpSnprintfUser)(char *str, size_t size, const char *format, ...);
extern void GH3X_Log(GCHAR *chLogString);

//sample log enable flag
#define SAMPLE_DEBUG_ENABLE (1)
#if SAMPLE_DEBUG_ENABLE
//sample log open
/// log support len
#define GH3X_LOG_DEBUG_SUP_LEN     (128)
//log define
#define   GH3X_SAMPLE_LOG_PARAM(...)        do {\
                                                    if(gpSnprintfUser)\
                                                    {\
                                                        GCHAR chDebugStr[GH3X_LOG_DEBUG_SUP_LEN] = {0};\
                                                        gpSnprintfUser(chDebugStr, GH3X_LOG_DEBUG_SUP_LEN, \
                                                                "[gh3x_drv]: "__VA_ARGS__);\
                                                        GH3X_Log(chDebugStr);\
                                                    }\
                                                    if(gpPrintfUser)gpPrintfUser("[gh3x_drv]: "__VA_ARGS__);\
                                            } while (0)
#endif
// #define   GH3X_SAMPLE_LOG_PARAM(...)
#define DRVLIB_GOODIX_DEMO_PLATFORM_DEBUG  0
#ifdef GOODIX_DEMO_PLATFORM
#ifdef GH3X_LOG_DEBUG
#undef GH3X_LOG_DEBUG
#endif
#define GH3X_LOG_DEBUG DRVLIB_GOODIX_DEMO_PLATFORM_DEBUG
#endif

/* debug log control, if enable debug, GH3X_Log must define in platforms code */
/// log string with param
#if defined(GH3X_LOG_DEBUG) && (GH3X_LOG_DEBUG > 0) // debug level > 0
#define   GH3X_DEBUG_LOG_PARAM(...)       do {\
                                                if(gpSnprintfUser)\
                                                {\
                                                    GCHAR chDebugStr[GH3X_LOG_DEBUG_SUP_LEN] = {0};\
                                                    gpSnprintfUser(chDebugStr, GH3X_LOG_DEBUG_SUP_LEN, \
                                                            "[gh3x_drv]: "__VA_ARGS__);\
                                                    GH3X_Log(chDebugStr);\
                                                }\
                                                if(gpPrintfUser)gpPrintfUser("[gh3x_drv]: "__VA_ARGS__);\
                                            } while (0)
#else   // debug level <= 0
#define   GH3X_DEBUG_LOG_PARAM(...)       do {\
                                                if(gpSnprintfUser)\
                                                {\
                                                    GCHAR chDebugStr[128] = {0};\
                                                    gpSnprintfUser(chDebugStr, 128, \
                                                            "[gh3x_drv]: "__VA_ARGS__);\
                                                    GH3X_Log(chDebugStr);\
                                                }\
                                                if(gpPrintfUser)gpPrintfUser("[gh3x_drv]: "__VA_ARGS__);\
                                            } while (0)      
#endif

/**< reg addr size */
#define   GH3X_REG_ADDR_SIZE              (0x0002)

/// fifo rawdata size
#define   GH3X_FIFO_RAWDATA_SIZE          (5)

/// fifo rawdata bytes max len
#define   GH3X_FIFO_DATA_BYTES_MAX_LEN    (800*GH3X_FIFO_RAWDATA_SIZE)    // GH3X_FIFO_CNT_CALC(800)

/// fifo rawdataindex size
#define   GH3X_FIFO_RAWDATAINDEX_SIZE     (2)

/// fifo gsensordata size
#define   GH3X_FIFO_GSENSORDATA_SIZE      (6)

/// fifo agcdata size
#define   GH3X_FIFO_AGCDATA_SIZE          (4)

/// fifo cnt calc
#define   GH3X_FIFO_CNT_CALC(x)           ((x) * GH3X_FIFO_RAWDATA_SIZE)

/// g sensor data size
#define   GH3X_GSENSOR_DATA_SIZE          (6)

/// agc data size
#define   GH3X_AGC_DATA_SIZE              (4)

/// agc data cnt calc
#define   GH3X_AGC_CNT_CALC(x)           ((x) * GH3X_AGC_DATA_SIZE)

/// algorithm result size
#define   GH3X_ALGO_RESULT_SIZE           (4)

/// algorithm result calc
#define   GH3X_ALGO_RESULT_CALC(x)           ((x) * GH3X_ALGO_RESULT_SIZE)

/// algorithm result with tag calc.
#define   GH3X_ALGO_RESULT_WITH_TAG_CALC(x)           ((x) * (GH3X_ALGO_RESULT_SIZE + 1))

/* rawdata macro */
#define   GH3X_WORD_TAG_GET_SLOT_NUM(x)              (((x) & 0x1F00) >> 8)


/// rawdata get slot num from dword
#define   GH3X_DWORD_RAWDATA_GET_SLOT_NUM(x)          (((x) & 0xE0000000) >> 29)

/// rawdata get adc num from dword
#define   GH3X_DWORD_RAWDATA_GET_ADC_NUM(x)           (((x) & 0x18000000) >> 27)

/// rawdata get slot&adc num from dword
#define   GH3X_DWORD_RAWDATA_GET_SLOT_ADC_NUM(x)      (((x) & 0xF8000000) >> 27)

/// rawdata get slot num from byte
#define   GH3X_BYTE_RAWDATA_GET_SLOT_NUM(x)           (((x) & 0xE0) >> 5)

/// rawdata get adc num from byte
#define   GH3X_BYTE_RAWDATA_GET_ADC_NUM(x)            (((x) & 0x18) >> 3)

/// slot & adc num bits of rawdata byte
#define   GH3X_SLOT_ADC_NUM_BITS                      (0xF8)

/// rawdata get slot adc num from byte
#define   GH3X_BYTE_RAWDATA_GET_SLOT_ADC_NUM(x)       (((x) & GH3X_SLOT_ADC_NUM_BITS) >> 3)

/// get slot index from channel num
#define   GH3X_GET_SLOT_FROM_CHANNEL_NUM(x)           (((x) & 0x1F) >> 2)

/// get adc index from channel num
#define   GH3X_GET_ADC_FROM_CHANNEL_NUM(x)            ((x) & 0x3)

/// rawdata clear all flag
#define   GH3X_RAWDATA_CLEAR_ALL_FLAG(x)              ((x) & 0x00FFFFFF)

/// rawdata clear data
#define   GH3X_RAWDATA_CLEAR_DATA(x)                  ((x) & 0xFF000000)

/// rawdata get adj flag0
#define   GH3X_RAWDATA_GET_ADJ_FLAG0(x)               (((x) & 0x4000000) >> 26)

/// rawdata get adj flag1
#define   GH3X_RAWDATA_GET_ADJ_FLAG1(x)               (((x) & 0x2000000) >> 25)

/// get rawdata for algorithm
#define   GH3X_GET_RAWDATA_FOR_ALGO(x)                ((x) >> 7)

/// rawdata get fast recovery flag
#define   GH3X_RAWDATA_GET_FAST_RECOVERY_FLAG(x)      (((x) & 0x4000000) >> 26)

/// rawdata real offset
#define   GH3X_RAWDATA_REAL_OFFSET                    (0x00800000)

/// reg addr[11:0] valid
#define   GH3X_GET_REG_REAL_ADRR(x)                   ((x) & 0x0FFF)

/* utils macro */

/// byte true flag val
#define   GH3X_BYTE_FLAG_TRUE                     (1)

/// byte false flag val
#define   GH3X_BYTE_FLAG_FALSE                    (0)

/// GF32 0 val
#define   GH3X_GF32_0                             (0.0f)

/// null val
//#define   GH3X_PTR_NULL                           ((void *) 0)

/// get high 4bits from byte
#define   GH3X_GET_HIGH_4BITS(val)                ((val) & 0xF0)

/// get low 4bits from byte
#define   GH3X_GET_LOW_4BITS(val)                 ((val) & 0x0F)

/// get first 4bits from byte
#define   GH3X_GET_FIRST_4BITS(val)               ((val) & 0x0000000FU)

/// get high byte from word
#define   GH3X_GET_HIGH_BYTE_FROM_WORD(sValue)    ((GU8)(((sValue) >> 8) & 0xFFU))

/// get low byte from word
#define   GH3X_GET_LOW_BYTE_FROM_WORD(sValue)     ((GU8)((sValue) & 0xFFU))

/// get byte3 from dword
#define   GH3X_GET_BYTE3_FROM_DWORD(unValue)      ((GU8)(((unValue) >> 24) & 0x000000FFU))

/// get byte2 from dword
#define   GH3X_GET_BYTE2_FROM_DWORD(unValue)      ((GU8)(((unValue) >> 16) & 0x000000FFU))

/// get byte1 from dword
#define   GH3X_GET_BYTE1_FROM_DWORD(unValue)      ((GU8)(((unValue) >> 8) & 0x000000FFU))

/// get byte0 from dword
#define   GH3X_GET_BYTE0_FROM_DWORD(unValue)      ((GU8)((unValue) & 0x000000FFU))

/// get high word from dword
#define   GH3X_GET_HIGH_WORD_FROM_DWORD(unValue)      ((GU16)(((unValue) >> 16) & 0x0000FFFFU))

/// get low word from dword
#define   GH3X_GET_LOW_WORD_FROM_DWORD(unValue)       ((GU16)((unValue) & 0x0000FFFFU))    

/// makeup word from bytes
#define   GH3X_MAKEUP_WORD(uchHighByte, uchLowByte)              ((GU16)(((((GU16)(uchHighByte)) << 8)& 0xFF00) |\
                                                                            (((GU16)(uchLowByte))& 0x00FF)))

/// makeup dword from bytes
#define   GH3X_MAKEUP_DWORD(uchByte3, uchByte2, uchByte1, uchByte0)     (((((GU32)(uchByte3)) << 24) & 0xFF000000U)|\
                                                                          ((((GU32)(uchByte2)) << 16) & 0x00FF0000U) |\
                                                                          ((((GU32)(uchByte1)) << 8) & 0x0000FF00U) |\
                                                                          (((GU32)(uchByte0)) & 0x000000FFU))

/// makeup dword from words
#define   GH3X_MAKEUP_DWORD2(usHighWord, usLowWord)                  (((((GU32)(usHighWord)) << 16) & 0xFFFF0000U) |\
                                                                            (((GU32)(usLowWord)) & 0x0000FFFFU))

/// macro of hook function call
/* NOLINTNEXTLINE(501) */
#define   HOOK_FUNC_CALL(func, argv)        do { if ((func) != GH3X_PTR_NULL) { func argv; } } while (0)

/// macro of handle function call
/* NOLINTNEXTLINE(501) */
#define   HANDLE_FUNC_CALL(func, argv)      do { if ((func) != GH3X_PTR_NULL) { func argv; } } while (0)

/// macro of set val via ptr
#define   SET_VAL_VIA_PTR(ptr, val)     do { if ((ptr) != GH3X_PTR_NULL) { (*(ptr)) = (val); } } while (0)

/**
 * @brief macro of read pin status, e.g. READ_PIN_STATUS_CALL(gpReadIntPinStatus, uchPinStatus)
 *        func() should retrun 0(low level) or 1(high), if not register func, return 0xFF(invalid)
 */
#define   READ_PIN_STATUS_CALL(func, status)    do {\
                                                    if ((func) != GH3X_PTR_NULL)\
                                                    {\
                                                        (status) = (func)();\
                                                    }\
                                                    else\
                                                    {\
                                                        (status) = 0xFF;\
                                                    }\
                                                } while (0)


/* bit operation macro */

/// macro of set bits
#define   GH3X_SET_BIT(x, b)              ((x) | (b))

/// macro of val set bits
#define   GH3X_VAL_SET_BIT(x, b)          ((x) |= (b))

/// macro of clear bits
#define   GH3X_CLEAR_BIT(x, b)            ((x) & (~(b)))

/// macro of val clear bits
#define   GH3X_VAL_CLEAR_BIT(x, b)        ((x) &= (~(b)))

/// macro of val get bits
#define   GH3X_VAL_GET_BIT(x, b)          ((x) &= (b))

/// macro of check bits set
#define   GH3X_CHECK_BIT_SET(x, b)        (((x) & (b)) == (b))

/// macro of check bits not set
#define   GH3X_CHECK_BIT_NOT_SET(x, b)    (((x) & (b)) != (b))

/// macro of check bits is set
#define   GH3X_CHECK_BIT_IS_SET(x, b)     (((x) & (b)) == (b))

/// macro of get bits is set
#define   GH3X_GET_BIT_IS_SET(x)          ((x) & 0xf000)

/// macro of bit number in one byte
#define   GH3X_BIT_PER_BYTE    (4)

/// macro of bit in reg addr for check
#define   GH3X_REG_MODE_BIT    (3)

/// macro of check bits zero
#define   GH3X_CHECK_REG_MODE_BIT_ZERO(x)    (((x) >> (GH3X_BIT_PER_BYTE * GH3X_REG_MODE_BIT)) == (0))

/// macro of check lsb set
#define   GH3X_CHECK_LSB_SET(x)           (GH3X_CHECK_BIT_SET((x), 0x01))

/// macro of check left shift bit set
#define   GH3X_CHECK_LEFT_BIT_SET(x, l)           (GH3X_CHECK_BIT_SET((x), (1u << (l))))

/// macro of check left shift bit not set
#define   GH3X_CHECK_LEFT_BIT_NOT_SET(x, l)       (GH3X_CHECK_BIT_NOT_SET((x), (1u << (l))))

/// macro of val clear left shift bit
#define   GH3X_VAL_CLEAR_LEFT_BIT(x, l)                ((x) &= (~((1u << (l)))))

/// check ret error
#define   GH3X_RET_ERROR_CHECK(x)             do { \
                                                    if ((x) != GH3X_RET_OK)\
                                                    {\
                                                        GH3X_DEBUG_LOG_PARAM("[%s]:error code = 0x%x\r\n", __FUNCTION__, x);\
                                                        return (x);\
                                                    }\
                                                } while (0)

/// check ret the error
#define   GH3X_RET_ERROR_CHECK_E(x, e)        do { \
                                                    if ((x) == (e))\
                                                    {\
                                                        return (x);\
                                                    }\
                                                } while (0)

/// check ret the error
#define   GH3X_RET_ERROR_CHECK_ER(x, e, r)    do { \
                                                    if ((x) == (e))\
                                                    {\
                                                        return (r);\
                                                    }\
                                                } while (0)

/// val rigth shfit
#define   GH3X_SET_VAL_RIGTH_SHIFT(x, r)      ((x) >>= (r))

/// val rigth shfit 1bit
#define   GH3X_SET_VAL_RIGTH_SHIFT_1BIT(x)    (GH3X_SET_VAL_RIGTH_SHIFT((x), 1u))

/// val left shfit
#define   GH3X_SET_VAL_LEFT_SHIFT(x, l)       ((x) <<= (l))

/// val left shfit 1bit
#define   GH3X_SET_VAL_LEFT_SHIFT_1BIT(x)     (GH3X_SET_VAL_LEFT_SHIFT((x), 1u))

/// get left shfit val
#define   GH3X_GET_LEFT_SHIFT_VAL(l)          (1u << (l))

/// val left shfit
#define   GH3X_VAL_LEFT_SHIFT(x, l)           ((x) << (l))

/// val cal diff value
#define   GH3X_VAL_DIFF_VALUE(a, b)           ((a) > (b) ? ((a) - (b)) : ((b) - (a)))

/// val cal diff status
#define   GH3X_VAL_DIFF_STA(a, b)             ((a) > (b) ? (0) : (1))

/// val cal diff ABS
#define   GH3X_VAL_ABS(x)                     ((x) >= 0 ? (x) : - (x))

/// val zip rawdata send cmd EVEN or ODD
#define   GH3X_VAL_ZIPSENDCMD(x)              ((x) == 1 ? (GH3X_UPROTOCOL_CMD_ZIPEVENRAWDATA) \
                                                 : (GH3X_UPROTOCOL_CMD_ZIPODDRAWDATA))

#define  GH3X_REG_IS_VIRTUAL0X0_BIT               (0x0000)
#define  GH3X_REG_IS_VIRTUAL0X1_BIT               (0x1000)
#define  GH3X_REG_IS_VIRTUAL0X2_BIT               (0x2000)
#define  GH3X_REG_IS_VIRTUAL0X3_BIT               (0x3000)
#define  GH3X_REG_IS_VIRTUAL0X4_BIT               (0x4000)
#define  GH3X_REG_IS_VIRTUAL0X5_BIT               (0x5000)
#define  GH3X_REG_IS_VIRTUAL0X6_BIT               (0x6000)
#define  GH3X_REG_IS_VIRTUAL0X7_BIT               (0x7000)
#define  GH3X_REG_IS_VIRTUAL0X8_BIT               (0x8000)
#define  GH3X_REG_IS_VIRTUAL0X9_BIT               (0x9000)
#define  GH3X_REG_IS_VIRTUAL0XA_BIT               (0xA000)
#define  GH3X_REG_IS_VIRTUAL0XB_BIT               (0xB000)
#define  GH3X_REG_IS_VIRTUAL0XC_BIT               (0xC000)
#define  GH3X_REG_IS_VIRTUAL0XE_BIT               (0xE000)
#define  GH3X_REG_IS_VIRTUAL0XF_BIT               (0xF000)


#endif /* _GH3X_DRV_H_ */

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
