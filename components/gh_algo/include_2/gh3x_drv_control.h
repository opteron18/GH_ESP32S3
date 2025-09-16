/**
  ****************************************************************************************
  * @file    gh3x_drv_control.h
  * @author  GHealth Driver Team
  * @brief   
  ****************************************************************************************
  * @attention  
  #####Copyright (c) 2022 GOODIX
   All rights reserved.
  
  ****************************************************************************************
  */

#ifndef _GH3X_DRV_CONTROL_H_
#define _GH3X_DRV_CONTROL_H_


#include "gh3x_drv_config.h"
#include "gh3x_drv.h"


/**
 * @brief status flag enum
 */
enum EMGh3xStatus
{
    GH3X_STATUS_NO_INIT = 0,  /**< no init status */
    GH3X_STATUS_INITED,       /**< inited status */
    GH3X_STATUS_STARTED,      /**< started status */
    GH3X_STATUS_INVALID,      /**< invaild status */
};


/* function ptr typedef */

typedef void (*pfnNormalHookFunc)(void);                                                 /**< normal hook type */
typedef void (*pfnGetRawdataHookFunc)(GU8 *puchDataBuffer, GU16 usLen);                  /**< get rawdata hook type */
typedef GU8 (*pfnReadPinStatus)(void);                                                   /**< read pin status type */
typedef void (*pfnSetPinLevel)(GU8 uchPinLevel);                                         /**< set pin status type */
typedef void (*pfnGetIoStructInfo)(const STFrameInfo * const pstFrameInfo);

/// size_t size
#define  GH3X_UPROTOCOL_SIZE_T             (4u)

/// align num
#define  GH3X_UPROTOCOL_ALIGN_LEFTSHIFT    (30u)

/// chip version string len
#define  GH3X_CHIP_VER_STR_LEN             (24)

/// send wakeup(resume) cmd
#define   GH3X_SEND_WAKEUP_CMD()          do { GH3X_SendCmd(GH3X_CMD_RESUME); } while (0)

/// send sleep cmd
#define   GH3X_SEND_DSLEEP_CMD()          do { GH3X_SendCmd(GH3X_CMD_SLEEP); } while (0)

/// send reset cmd
#define   GH3X_SEND_RESET_CMD()           do { GH3X_SendCmd(GH3X_CMD_RESET); } while (0)

/// wait send cmd done
#define   GH3X_SEND_WAIT_CMD_DONE()       do { GH3X_DelayUs(GH3X_CMD_DELAY_X_US); } while (0)

/// chip wakeup
#define   GH3X_CHIP_WAKEUP(ret)       do {\
                                            GH3X_SEND_WAKEUP_CMD();\
                                            GH3X_SEND_WAIT_CMD_DONE();\
                                            (ret) = GH3X_RET_OK;\
                                        } while (0)

/// chip sleep
#define   GH3X_CHIP_SLEEP(ret)        do {\
                                            GH3X_SEND_DSLEEP_CMD();\
                                            GH3X_SEND_WAIT_CMD_DONE();\
                                            (ret) = GH3X_RET_OK;\
                                        } while (0)

/// chip reset
#define   GH3X_CHIP_RESET(ret)        do {\
                                            GH3X_SEND_RESET_CMD();\
                                            (ret) = GH3X_RET_OK;\
                                        } while (0)

/* reg addr control bit macro */

/// reg addr[11:0] valid
#define   GH3X_GET_REG_REAL_ADRR(x)       ((x) & 0x0FFF)

/// set chip inited flag into reg
#define   GH3X_SET_CHIP_INIED()    do {\
                                        GU16 usChipIniedRegValTmp =\
                                            GH3X_ReadReg(GH3X_INSTRUCTIONS_CHIP_INIED_REG_ADDR);\
                                            GH3X_WriteReg(GH3X_INSTRUCTIONS_CHIP_INIED_REG_ADDR, \
                                                usChipIniedRegValTmp | GH3X_INSTRUCTIONS_CHIP_INIED_REG_VAL);\
                                     } while (0)

/// read chip inited flag from reg
#define   GH3X_IS_CHIP_INIED()    ((GH3X_ReadReg(GH3X_INSTRUCTIONS_CHIP_INIED_REG_ADDR) &\
                                        GH3X_INSTRUCTIONS_CHIP_INIED_REG_VAL) == \
                                        GH3X_INSTRUCTIONS_CHIP_INIED_REG_VAL)

/// store fifo power status
#define  GH3X_PMU_FIFO_POWER_OFF()    do {\
                                            if (guchPmuFifoModuleCtrlCacheStatus == 0)\
                                            {\
                                                if ((guchInitStatus == GH3X_STATUS_INITED) || \
                                                    (guchInitStatus == GH3X_STATUS_NO_INIT)) \
                                                {\
                                                    gusPmuFifoModuleCtrlVal = GH3X_ReadReg(GH3X_PMU_CTRL4_REG_ADDR);\
                                                    GH3X_WriteReg(GH3X_PMU_CTRL4_REG_ADDR, (gusPmuFifoModuleCtrlVal | GH3X_PMU_CTRL4_FIFO_DISABLE_MASK));\
                                                    guchPmuFifoModuleCtrlCacheStatus = 1;\
                                                    GH3X_DEBUG_LOG_PARAM("save pmu fifo module status!\r\n");\
                                                }\
                                            }\
                                        } while (0)

/// set chip inited flag into reg
#define  GH3X_PMU_FIFO_POWER_RECOVERY()       do {\
                                                    if (guchPmuFifoModuleCtrlCacheStatus == 1)\
                                                    {\
                                                        GH3X_WriteReg(GH3X_PMU_CTRL4_REG_ADDR, gusPmuFifoModuleCtrlVal);\
                                                        GH3X_DEBUG_LOG_PARAM("write pmu: 0x%X\r\n",gusPmuFifoModuleCtrlVal);\
                                                        guchPmuFifoModuleCtrlCacheStatus = 0;\
                                                        GH3X_DEBUG_LOG_PARAM("recovery pmu fifo module status!\r\n");\
                                                    }\
                                                } while (0)

/// wear detect force switch delay
#define   GH3X_WEAR_DETECT_SWITCH_WAIT_X_US           (150)

/// hard reset delay
#define   GH3X_HARD_RESET_DELAY_X_US                  (100)

/// fifo watermark thr min val
#define   GH3X_FIFO_WATERMARK_THR_MIN                 (3)

/// fifo watermark thr max val
#define   GH3X_FIFO_WATERMARK_THR_MAX                 (800)

/// irq width val
#define   GH3X_IRQ_WIDTH_VAL_MAX                      (2047)

/* osc cali */

/// osc calibration code lock flag
#define   GH3X_OSC_CALI_CODE_LOACKED                  (0x0001)

/// osc calibration Ctrl disable
#define   GH3X_OSC_CALI_CTRL_DIS_VAL                  (0x0000)

/// hsi osc calibration delay
#define   GH3X_HSI_CALI_DELAY_VAL                     (1000u)

/// hsi osc calibration theroy count
#define   GH3X_HSI_CALI_THR_CNT_VAL                   (0x0196)

/// hsi osc calibration Ctrl enable
#define   GH3X_HSI_CALI_CTRL_EN_VAL                   (0x0271)

/// lsi osc calibration theroy count
#define   GH3X_LSI_CALI_THR_CNT_VAL                   (0x1F40)

/// lsi osc calibration ctrl coarse enable
#define   GH3X_LSI_CALI_CTRL_C_EN_VAL                 (0x09D7)

/// lsi osc calibration ctrl fine enable
#define   GH3X_LSI_CALI_CTRL_F_EN_VAL                 (0x19D3)

/// lsi osc calibration error val max
#define   GH3X_LSI_CALI_ERR_MAX_VAL                   (0x3FFF)

/// lsi osc calibration error val fixed val
#define   GH3X_LSI_CALI_ERR_FIXED_VAL                 (0x8000)

/// lsi osc calibration fine error max
#define   GH3X_LSI_CALI_FINE_VAL_MAX                  (255u)

/// lsi osc calibration fine tune max cnt
#define   GH3X_LSI_CALI_FINE_TUNE_MAX                 (9u)

/// lsi osc calibration fine div num
#define   GH3X_LSI_CALI_FINE_DIV_NUM                  (2u)

/// lsi osc calibration delay
#define   GH3X_LSI_COR_CALI_DELAY_VAL                 (50000u)

/// lsi osc calibration delay
#define   GH3X_LSI_FINE_CALI_DELAY_VAL                (5000u)

/**< rawdata buffer size in byte */
#define   GH3X_RAWDATA_BUFFER_SIZE                    (200*4)

/* extern variables declaration */

/// init hook func ptr
extern pfnNormalHookFunc gpInitHookFunc;

/// start hook func ptr
extern pfnNormalHookFunc gpStartHookFunc;

/// stop hook func ptr
extern pfnNormalHookFunc gpStopHookFunc;

/// get rawdata hook func ptr
extern pfnGetRawdataHookFunc gpGetRawdataHookFunc;

/// set reset pin level spi
extern pfnSetPinLevel gpResetPinLevelControlFunc;

/// delay us func ptr
extern pfnDelayTime gpDelayUsFunc;

/// delay Ms func ptr
extern pfnDelayTime gpDelayMsFunc;

/// fifo power ctrl fifo control reg cache var
extern GU16 gusPmuFifoModuleCtrlVal;

/// for fifo power ctrl cache status
extern GU8 guchPmuFifoModuleCtrlCacheStatus;

/// gh3x status
extern GU8 guchInitStatus;

/// max rawdata num read from fifo every time
extern GU16 gusMaxNumReadFromFifo;

/// function started bitmap, use for sampling control
extern GU32 gunFuncStartedBitmap;

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

/* util function, memcpy & memset & strlen */

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
 * @fn     GU16 GH3X_FindGu16MinVal(GU16 *pusBuffer, GU8 uchLen)
 *
 * @brief  Find min val
 *
 * @attention   len must > 0
 * 
 * @param[in]   pusBuffer        pointer to buffer
 * @param[in]   uchLen           buffer length
 * @param[out]  None
 *
 * @return  min val, if len = 0, return 0
 */
GU16 GH3X_FindGu16MinVal(GU16 *pusBuffer, GU8 uchLen);

/**
 * @fn     GU16 GH3X_FindGu16MaxVal(GU16 *pusBuffer, GU8 uchLen)
 *
 * @brief  Find max val
 *
 * @attention   len must > 0
 * 
 * @param[in]   pusBuffer        pointer to buffer
 * @param[in]   uchLen           buffer length
 * @param[out]  None
 *
 * @return  max val, if len = 0, return 0
 */
GU16 GH3X_FindGu16MaxVal(GU16 *pusBuffer, GU8 uchLen);

/**
 * @fn     void GH3X_FindGu16MaxMinVal(GU16 *pusMaxVal, GU16 *pusMinVal, GU16 *pusBuffer, GU8 uchLen)
 *
 * @brief  Find min val & max val
 *
 * @attention   len must > 0, ptr not null
 * 
 * @param[in]   pusBuffer        pointer to buffer
 * @param[in]   uchLen           buffer length
 * @param[out]  pusMaxVal        pointer to max val
 * @param[out]  pusMinVal        pointer to min val
 *
 * @return  None
 */
void GH3X_FindGu16MaxMinVal(GU16 *pusMaxVal, GU16 *pusMinVal, GU16 *pusBuffer, GU8 uchLen);

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

/**
 * @fn     void GH3X_RegisterHookFunc(void (*pInitHookFunc)(void),
 *                            void (*pStartHookFunc)(void),
 *                           void (*pStopHookFunc)(void),
 *                            void (*pGetRawdataHookFunc)(GU8 *puchReadDataBuffer, GU16 usReadDataLen))
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
 * @fn     GS8 GH3X_StartSampling(void)
 *
 * @brief  Start gh3x sampling
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  errcode
 * @retval  #GH3X_RET_OK                  return successfully
 * @retval  #GH3X_RET_NO_INITED_ERROR     gh3x has not inited
 * @retval  #GH3X_RET_GENERIC_ERROR       gh3x has started, need restart should call GH3X_StopSampling then start
 */
GS8 GH3X_StartSampling(void);

/**
 * @fn     GS8 GH3X_StopSampling(void)
 *
 * @brief  Stop gh3x sampling
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 * 
 * @return  errcode
 * @retval  #GH3X_RET_OK                  return successfully
 * @retval  #GH3X_RET_NO_INITED_ERROR     gh3x has not inited
 */
GS8 GH3X_StopSampling(void);

/**
 * @fn     GU8 GH3X_ConfirmLeadStatus(GU8 uchCmp, GU8 uchIq, GU8 uchLogic)
 *
 * @brief  confirm lead status
 *
 * @attention   None
 *
 * @param[in]   uchCmp
 * @param[out]  uchIq
 * @param[out]  uchLogic
 * @return  ret
 */
GU8 GH3X_ConfirmLeadStatus(GU8 uchCmp, GU8 uchIq, GU8 uchLogic);

/**
 * @fn     void GH3X_ConfirmIrqStatus(GU32* unGotEvent)
 *
 * @brief  Handle some irq event situation
 *
 * @attention   None
 *
 * @param[in]   unGotEvent
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_ConfirmIrqStatus(GU32* unGotEvent);

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
 * @fn     GS8 GH3X_FunctionStart(const STFrameInfo * const pstFrameInfo)
 *
 * @brief  Start functions sample
 *
 * @attention   None
 *
 * @param[in]   pstFrameInfo    frame struct
 *
 * @return  error code
 */
GS8 GH3X_FunctionStart(const STFrameInfo * const pstFrameInfo);

/**
 * @fn     GS8 GH3X_FunctionStop(const STFrameInfo * const pstFrameInfo)
 *
 * @brief  Stop functions sample
 *
 * @attention   None
 *
 * @param[in]   pstFrameInfo    frame struct
 *
 * @return  error code
 */
GS8 GH3X_FunctionStop(const STFrameInfo * const pstFrameInfo);



/**
 * @fn     void GH3xSetFunctionChnlNum(const STFrameInfo * const pstFrameInfo, GU8 uchChnlNum)
 *
 * @brief  config Function ChnlNum
 *
 * @attention   None
 *
 * @param[in]   STFrameInfo    frame struct, uchChnlNum  channel in function
 *
 * @return  None
 */
void GH3xSetFunctionChnlNum(const STFrameInfo * const pstFrameInfo, GU8 uchChnlNum);


/**
 * @fn     void GH3xSetFunctionChnlMap(const STFrameInfo * const pstFrameInfo, GU8 uchChnlNum)
 *
 * @brief  config Function ChnlMap
 *
 * @attention   None
 *
 * @param[in]   STFrameInfo    frame struct, uchChnlId channel Id in function, uchChnlTag  tag in channel
 *
 * @return  None
 */
void GH3xSetFunctionChnlMap(const STFrameInfo * const pstFrameInfo, GU8 uchChnlId, GU16 uchChnlTag);



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
 * @fn     void GH3X_ClearSoftEvent(GU32 unEvent)
 * @brief  clear soft event
 * @param[in]   uchEvent
 * @param[out]  None
 * @return  None
 */
void GH3X_ClearSoftEvent(GU32 unEvent);
GU32 GH3xCalFunctionSlotBit(const STFrameInfo * const pstFrameInfo);

#endif /* _GH3X_DRV_CONTROL_H_ */

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
