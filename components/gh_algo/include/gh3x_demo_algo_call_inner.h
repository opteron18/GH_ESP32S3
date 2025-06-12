/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x_demo_algo_call_inner.h
 *
 * @brief   gh3x algorithm calc define & declaration
 *
 * @version ref gh3x_demo_algo_call_inner.h
 *
 */

#ifndef _GH3X_DEMO_ALGORITHM_CALC_H_
#define _GH3X_DEMO_ALGORITHM_CALC_H_


#include "gh3x_demo_algo_config.h"
#include "gh3x_demo_algo_call.h"
#include "gh3x_demo_algo_hook.h"
#include "gh3x_demo_common.h"

#ifdef GOODIX_DEMO_PLATFORM
#include "gh3x_demo_goodix_platform.h"
#else
#define GOODIX_PLATFROM_NADT_RESULT_HANDLE_ENTITY()
// #define GOODIX_PLATFROM_LOG_ENTITY()
#endif

/// algorithm calculate hook type
typedef GS8 (*pfnAlgorithmCallFunc)(const STFrameInfo * const pstFrameInfo);

extern STFrameInfo ** gpstGH3XAlgoFrameInfo;

#define GOODIX_HR_PARAMS_NUM_MAX           (5)
#define GOODIX_HRV_PARAMS_NUM_MAX          (5)
#define GOODIX_SPO2_PARAMS_NUM_MAX         (11)
#define GOODIX_ECG_PARAMS_NUM_MAX          (5)
#define GOODIX_BT_PARAMS_NUM_MAX           (20)
#define GOODIX_AF_PARAMS_NUM_MAX           (2)
#define GOODIX_BIA_PARAMS_NUM_MAX           (12)
#define GOODIX_GSR_PARAMS_NUM_MAX            (6)

#define GH3X_RAWDATA_BASE_VALUE          (1<<23)

#define  GH3X_GAIN_VALUE_INVALID                      (0xFF)

/// xor val for xor num in encrypt & decrypt
#define   GH3X_CRYP_XOR_XOR_VAL                       (0xFF)

/// fixed val in encrypt & decrypt
#define   GH3X_CRYP_FIXED_VAL                         (0x55)

/// header len in encrypt & decrypt
#define   GH3X_CRYP_HEADER_LEN                        (5u)

/// cryp rawdata len to index
#define   GH3X_ENCRYPT_LEN2INDEX                      (1u)

/* encrypt & decrypt header index */
#define   GH3X_CRYP_HEADER_0                          (0u)     /**< cryp header index 0 */
#define   GH3X_CRYP_HEADER_1                          (1u)     /**< cryp header index 1 */
#define   GH3X_CRYP_HEADER_2                          (2u)     /**< cryp header index 2 */
#define   GH3X_CRYP_HEADER_3                          (3u)     /**< cryp header index 3 */
#define   GH3X_CRYP_HEADER_4                          (4u)     /**< cryp header index 4 */

/// encrypt data min len, min len is 4
#define   GH3X_ENCRYPT_CHECK_MIN_LEN(x)               ((x) > 3)

/// encrypt source add magic bytes, add after source buffer & len must >= 4, magic bytes len align to cryp header len
#define   GH3X_ENCRYPT_SRC_ADD_MAGIC(src, len)        do {\
                                                            (src)[(len)] = ((src)[0] ^ (src)[(len) - 1]);\
                                                            (src)[(len) + 1] = ((src)[1] ^ (src)[(len) - 3]);\
                                                            (src)[(len) + 2] = GH3X_CRYP_FIXED_VAL;\
                                                            (src)[(len) + 3] = ((src)[2] ^ (src)[(len) - 2]);\
                                                            (src)[(len) + 4] = ((src)[3] ^ (src)[(len) - 4]);\
                                                        } while (0)

/// encrypt source verify magic bytes, add after source buffer & len must >= 4
#define   GH3X_ENCRYPT_VERIFY_SRC_MAGIC(src, len)     (((src)[(len)] == ((src)[0] ^ (src)[(len) - 1])) &&\
                                                         ((src)[(len) + 1] == ((src)[1] ^ (src)[(len) - 3])) &&\
                                                         ((src)[(len) + 2] == GH3X_CRYP_FIXED_VAL) &&\
                                                         ((src)[(len) + 3] == ((src)[2] ^ (src)[(len) - 2])) &&\
                                                         ((src)[(len) + 4] == ((src)[3] ^ (src)[(len) - 4])))

/// hba scenario mcaro max val
#define   GH3X_HBA_SCENARIO_CONFIG_MAX_VAL            (HBA_SCENES_WALKING_OUTSIDE)

/* rawdata package for algorithm */
#define   GH3X_ALGO_RAW_GX_INDEX                      (0u)    /**< rawdata gsensor x-asix index */
#define   GH3X_ALGO_RAW_GY_INDEX                      (1u)    /**< rawdata gsensor y-asix index */
#define   GH3X_ALGO_RAW_GZ_INDEX                      (2u)    /**< rawdata gsensor z-asix index */
#define   GH3X_ALGO_RAW_CH0_INDEX                     (3u)    /**< rawdata ch0 data index */
#define   GH3X_ALGO_RAW_CH1_INDEX                     (4u)    /**< rawdata ch1 data index */
#define   GH3X_ALGO_RAW_CH2_INDEX                     (5u)    /**< rawdata ch2 data index */
#define   GH3X_ALGO_RAW_CH3_INDEX                     (6u)    /**< rawdata ch3 data index */
#define   GH3X_ALGO_RAW_CH4_INDEX                     (7u)    /**< rawdata ch4 data index */
#define   GH3X_ALGO_RAW_CH5_INDEX                     (8u)    /**< rawdata ch5 data index */
#define   GH3X_ALGO_RAW_CH6_INDEX                     (9u)    /**< rawdata ch6 data index */
#define   GH3X_ALGO_RAW_CH7_INDEX                     (10u)   /**< rawdata ch7 data index */
#define   GH3X_ALGO_RAW_CH8_INDEX                     (11u)   /**< rawdata ch8 data index */
#define   GH3X_ALGO_RAW_CH9_INDEX                     (12u)   /**< rawdata ch9 data index */
#define   GH3X_ALGO_RAW_CH10_INDEX                    (13u)   /**< rawdata ch10 data index */
#define   GH3X_ALGO_RAW_CH11_INDEX                    (14u)   /**< rawdata ch11 data index */
#define   GH3X_ALGO_RAWDATA_LEN                       (35u)   /**< rawdata packet len for algorithm */

/// algorithm channel max cnt support
#define   GH3X_CHANNEL_MAP_MAX_CNT                    (32u)

/// get slot & adc num from rawdata flag byte, equal channel map formats
#define   GH3X_CHANNEL_MAP_GET(x)                     ((x) & GH3X_SLOT_ADC_NUM_BITS)

/// channel map invalid val
#define   GH3X_CHANNEL_MAP_INVALID_VAL                (0x07)

/// channel map is ecg channel
#define   GH3X_CHANNEL_MAP_ISNOT_ECG_CH(x)            (GH3X_CHECK_BIT_NOT_SET((x), GH3X_CHANNEL_MAP_ADC3))

/// algorithm output GF32 to GS32, multiply by (100)
#define   GH3X_ALGORITHM_GF32_2_GS32(x)               ((GS32)((x) * 100))

/// rawdata is new sampling data
#define   GH3X_IS_NEW_RAWDATA                         (1)

/// rawdata is new sampling data
#define   GH3X_IS_NOT_NEW_RAWDATA                     (0)

/// number of ECG algorithm output
#define  GH3X_ECG_ALGO_OUTPUT_NUM                 (sizeof(STEcgAlgoResult))

/// number of spo2 algorithm output
#define  GH3X_SPO2_ALGO_OUTPUT_NUM                (sizeof(STSpo2AlgoResult))

/// number of hb algorithm output
#define  GH3X_HB_ALGO_OUTPUT_NUM                  (sizeof(STHbAlgoResult))

/// number of hrv algorithm output
#define  GH3X_HRV_ALGO_OUTPUT_NUM                 (sizeof(STHrvAlgoResult))

/// memory for algorithm is not inited
#define ALGO_MEM_NO_INIT                            (0) 

/// memory for algorithm is already inited
#define ALGO_MEM_INIT_OK                            (1)

// algo chnl map num
#define ALGO_CHNL_NUM                               (32)

// algo green chnl pos
#define ALGO_GREEN_CHNL_POS                         (0)

// algo ir chnl pos
#define ALGO_IR_CHNL_POS                            (8)

// algo red chnl pos
#define ALGO_RED_CHNL_POS                           (16)

/* ecg algorithm */

// ECG 采样率
#ifndef ECG_FS
#define ECG_FS 500
#endif
//#define ECG_CHANNEL_NUM 2

#define ECG_OUT_FACTOR                  (100)
#define ECG_SNR_FACTOR                  (10000000)

#define ECG_FRAME_UNUSED                (5)

#define GH3X_REG_IS_VIRTUAL0X7_BIT                (0x7000)
#define GH3X_VREG_FUNCTION_OFFSET                 (0X0300)
#define GH3X_ADT_DRV_CFG_ADDR                     (0x7000)
#define GH3X_ADT_ALG_CFG_ADDR                     (0x70C0)
#define GH3X_REG_ADDR_SIZE                        (0x0002)    /**< reg addr size */
#define GH3X_VREG_ALGO_CHNL_MAP_OFFSET            (0X0200)
#define GH3X_HR_SCENARIO_CFG_L_ADDR               (0x33C0)
#define GH3X_HR_ALGO_CHNL_NUM_ADDR                (0x75C0)
#define GH3X_SPO2_ALGO_CHNL_NUM_ADDR              (0x84C0)
#define GH3X_SPO2_CH3_CORRECTION_FACTOR4_H_ADDR   (0x430E)
#define GH3X_SPO2_CH0_CORRECTION_FACTOR0_L_ADDR   (0x42C0)

#define GH3X_HSM_FLAG_BUFF_LEN (120)

typedef enum
{
    ADT_ALGO_RESULT_DEFULT = 0,
    ADT_ALGO_RESULT_WAER_ON,   /**< detect wear on */
    ADT_ALGO_RESULT_WAER_OFF,
} EMAdtAlgoStatus;

/**
 * @brief gh3x algo function id
 */
typedef enum 
{ 
    GH3X_ALGO_INIT_FUNCTION = 0,
    GH3X_ALGO_CALCULATE_FUNCTION,
//    GH3X_ALGO_FINAL_CALCULATE_FUNCTION,
    GH3X_ALGO_DEINIT_FUNCTION,
    GH3X_ALGO_FUNCTION_NUM_MAX,
} EMAlgoFunctionID;

typedef struct
{
    GS32 nHbaScenario;
    GS32 nHbaCalcOutputTime;
    GS32 nFrequency;
}STAlgoHrParas;

/* hb algorithm */
#if (__USE_GOODIX_HR_ALGORITHM__)
extern GS32 gunHrParamsArr[GOODIX_HR_PARAMS_NUM_MAX];
/// hbd rawdata channel map
extern STAlgoChnlMap gstHbaAlgoChnlMap;
#endif

/* hrv algorithm */
#if (__USE_GOODIX_HRV_ALGORITHM__)
extern GS32 gnHrvParamsArr[GOODIX_HRV_PARAMS_NUM_MAX];
#endif

/* spo2 algorithm */
#if (__USE_GOODIX_SPO2_ALGORITHM__)
extern GS32 gnSpo2ParamssArr[GOODIX_SPO2_PARAMS_NUM_MAX];
/// spo2 rawdata channel map
extern STAlgoChnlMap gstSpo2AlgoChnlMap;
#endif

/* ecg algorithm */
#if (__USE_GOODIX_ECG_ALGORITHM__)
extern GS32 gnEcgParamsArr[GOODIX_ECG_PARAMS_NUM_MAX];
/// ecg frame data cnt
extern GU32 gunEcgFrameCnt;
extern GU8 guchClassificationFlag;
extern GU8 gunEcgTimeSeconds;
#endif

/* bia algorithm */
#if (__USE_GOODIX_BIA_ALGORITHM__)
extern GS32 gnBiaPInfo[GOODIX_BIA_PARAMS_NUM_MAX];
extern STAlgoChnlMap gstBiaAlgoChnlMap;
#endif


/* bt algorithm */
#if (__USE_GOODIX_BT_ALGORITHM__)
extern GS32 gnBtParamsArr[GOODIX_BT_PARAMS_NUM_MAX];
#endif

/* af algorithm */
#if (__USE_GOODIX_AF_ALGORITHM__)
extern GS32 gnAfParamsArr[GOODIX_AF_PARAMS_NUM_MAX];
#endif

extern GU8 guchAlgoGsensorEnable;
extern GU8 guchAlgoCapEnable;
extern GU8 guchAlgoTempEnable;

extern GU8 GH3X_GetAlgoMemStatus(void);

/**
 * @fn     GS8 GH3X_AlgoMemInit(void)
 *
 * @brief  algorithm memory init
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  error code
 * @retval  #GH3X_RET_OK                      return successfully
 * @retval  #GH3X_RET_RESOURCE_ERROR          return algorithm mem init error
 */
GS8 GH3X_AlgoMemInit(void);

/**
 * @fn     void GH3X_AlgoMemDeinit(void)
 *
 * @brief  algorithm memory deinit
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_AlgoMemDeinit(void);

/**
 * @fn     void GH3X_SetNewDataFlag(EMChannelMapId emFuncId, GU8 uchFlag)
 *
 * @brief  Set new sampling data flag
 *
 * @attention   None
 *
 * @param[in]   emFuncId    function id
 * @param[in]   uchFlag      new data or not
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_SetNewDataFlag(GU8 emFuncId, GU8 uchFlag);
extern void GH3XOutputValueStrategyInit(GU32 unFunctionID);
extern void GH3XSleepFlagSet(GU8 uchSleepFlag);
extern GU8 GH3XSleepFlagGet(void);
extern GS32 GH3X_Round(GF32 fValue);
extern GU8 GH3X_BitCount(GU32 n);
GU8 GH3X_GetHrAlgoSupportChnl(void);
GU8 GH3X_GetSpo2AlgoSupportChnl(void);

extern GU8* GH3XAdtVersion (void);
extern GS8 GH3XAdtAlgoInit(const STFrameInfo * const pstFrameInfo);
extern GS8 GH3XAdtAlgoDeinit(const STFrameInfo * const pstFrameInfo);
extern GS8 GH3XAdtAlgoExe(const STFrameInfo * const pstFrameInfo);

extern GS8 GH3XHrAlgoInit(const STFrameInfo * const pstFrameInfo);
extern GS8 GH3XHrAlgoDeinit(const STFrameInfo * const pstFrameInfo);
extern GS8 GH3XHrAlgoExe(const STFrameInfo * const pstFrameInfo);
extern void GH3X_HbaAlgoChnlMapDefultSet(void);
extern void GH3XHrOutputValueStrategyPro(STAlgoData * pstAlgoResult, GU32 lubFrameId);

extern GS8 GH3XHrvAlgoInit(const STFrameInfo * const pstFrameInfo);
extern GS8 GH3XHrvAlgoDeinit(const STFrameInfo * const pstFrameInfo);
extern GS8 GH3XHrvAlgoExe(const STFrameInfo * const pstFrameInfo);

extern GS8 GH3XSpo2AlgoInit(const STFrameInfo * const pstFrameInfo);
extern GS8 GH3XSpo2AlgoDeinit(const STFrameInfo * const pstFrameInfo);
extern GS8 GH3XSpo2AlgoExe(const STFrameInfo * const pstFrameInfo);
extern void GH3X_Spo2AlgoChnlMapDefultSet(void);

extern GS8 GH3XAfAlgoInit(const STFrameInfo * const pstFrameInfo);
extern GS8 GH3XAfAlgoDeinit(const STFrameInfo * const pstFrameInfo);
extern GS8 GH3XAfAlgoExe(const STFrameInfo * const pstFrameInfo);

extern GS8 GH3XAfAlgoInit(const STFrameInfo * const pstFrameInfo);
extern GS8 GH3XAfAlgoDeinit(const STFrameInfo * const pstFrameInfo);
extern GS8 GH3XAfAlgoExe(const STFrameInfo * const pstFrameInfo);

extern GS8 GH3XRespAlgoInit(const STFrameInfo * const pstFrameInfo);
extern GS8 GH3XRespAlgoDeinit(const STFrameInfo * const pstFrameInfo);
extern GS8 GH3XRespAlgoExe(const STFrameInfo * const pstFrameInfo);

extern GS8 GH3XBtAlgoInit(const STFrameInfo * const pstFrameInfo);
extern GS8 GH3XBtAlgoDeinit(const STFrameInfo * const pstFrameInfo);
extern GS8 GH3XBtAlgoExe(const STFrameInfo * const pstFrameInfo);

extern GS8 GH3XHsmAlgoInit(const STFrameInfo * const pstFrameInfo);
extern GS8 GH3XHsmAlgoDeinit(const STFrameInfo * const pstFrameInfo);
extern GS8 GH3XHsmAlgoExe(const STFrameInfo * const pstFrameInfo);

extern GS8 GH3XSoftAdtAlgoInit(const STFrameInfo * const pstFrameInfo);
extern GS8 GH3XSoftAdtAlgoDeinit(const STFrameInfo * const pstFrameInfo);
extern GS8 GH3XSoftAdtAlgoExe(const STFrameInfo * const pstFrameInfo);

extern GS8 GH3XEcgAlgoInit(const STFrameInfo * const pstFrameInfo);
extern GS8 GH3XEcgAlgoDeinit(const STFrameInfo * const pstFrameInfo);
extern GS8 GH3XEcgAlgoExe(const STFrameInfo * const pstFrameInfo);
extern GU8 GH3XEcgAlgoClassification(void);

extern GS8 GH3XBiaAlgoInit(const STFrameInfo * const pstFrameInfo);
extern GS8 GH3XBiaAlgoDeinit(const STFrameInfo * const pstFrameInfo);
extern GS8 GH3XBiaAlgoExe(const STFrameInfo * const pstFrameInfo);

extern GS8 GH3XGSRAlgoInit(const STFrameInfo * const pstFrameInfo);
extern GS8 GH3XGSRAlgoDeinit(const STFrameInfo * const pstFrameInfo);
extern GS8 GH3XGSRAlgoExe(const STFrameInfo * const pstFrameInfo);
#endif /* _GH3X_DRV_ALGORITHM_CALC_H_ */

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
