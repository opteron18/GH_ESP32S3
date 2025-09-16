/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x_drv_interface.h
 *
 * @brief   gh3x interface define & declaration
 *
 * @version ref gh3x_drv_version.h
 *
 */

#ifndef _GH3X_DRV_INTERFACE_H_
#define _GH3X_DRV_INTERFACE_H_


#include "gh3x_drv_config.h"
#include "gh3x_drv.h"


/* function ptr typedef */

typedef GU8 (*pfnI2cWrite)(GU8 uchDeviceId, const GU8 uchWriteBytesArr[], GU16 usWriteLen); /**< i2c write type */
typedef GU8 (*pfnI2cRead)(GU8 uchDeviceId, const GU8 uchCmddBytesArr[], GU16 usCmddLen,
                          GU8 uchReadBytesArr[], GU16 usMaxReadLen);                        /**< i2c read type */
typedef void (*pfnDelayTime)(GU16 usUsec);                                                    /**< delay us type */
typedef GU8 (*pfnSpiRWCtrl)(GU8 uchBufferArrArr[], GU16 usLen);                             /**< spi rw type */
typedef void (*pfnSpiCsCtrl)(GU8 uchCsLevel);                                               /**< spi cs ctrl type */
typedef void (*pfnSendCmd)(GU8 uchCmd);                                                     /**< send cmd type */
typedef void (*pfnWriteReg)(GU32 unAddr, GU16 usValue);                                     /**< write reg type */
typedef void (*pfnWriteRegs)(GU32 unRegAddr, GU16 *usRegValueBuffer,GU8 uchRegNum);                                     /**< write reg type */
typedef GU16 (*pfnReadReg)(GU32 unAddr);                                                    /**< read reg type */
typedef void (*pfnReadRegs)(GU32 unRegAddr, GU16 *pusRegValueBuffer, GU8 uchLen);           /**< read regs type */
typedef void (*pfnReadFifo)(GU8 *puchDataBuffer, GU16 usLen);                               /**< read fifo type */



/// i2c ctrl interface, base device addr
#define   GH3X_I2C_DEVICE_ID_BASE         (0x28)

/// spi seq write cmd val
#define   GH3X_SPI_CMD_WRITE              (0xF0)

/// spi seq read cmd val
#define   GH3X_SPI_CMD_READ               (0xF1)

/// spi cs ctrl high level val
#define   GH3X_SPI_CS_CTRL_HIGH           (0x01)

/// spi cs ctrl low level val
#define   GH3X_SPI_CS_CTRL_LOW            (0x00)

/// spi seq wait delay, 4us
#define   GH3X_SPI_WAIT_DELAY_X_US        (4)

/// spi seq end delay, 2us
#define   GH3X_SPI_END_DELAY_X_US         (3)

/// send cmd delay, 500us
#define   GH3X_CMD_DELAY_X_US             (500)

/// sleep cmd val
#define   GH3X_CMD_SLEEP                  (0xC4)

/// resume cmd val
#define   GH3X_CMD_RESUME                 (0xC3)

/// reset cmd val
#define   GH3X_CMD_RESET                  (0xC2)

/// use for delay 1us, must test in more platforms, only test in stm32f412
#define   GH3X_DELAY_ONE_US_CNT           (9u)



#define   GH3X_SPI_BURST_WRITE_BASE_LEN    (3)
#define   GH3X_ADDR_BYTE_NUM               (2)

/**
 * @fn     void GH3X_SwapLittleBigEndianData(GU8 *puchDataBuffer, GU16 usDataLength)
 *
 * @brief  Swap little big endian data
 *
 * @attention   None
 *
 * @param[in]   puchDataBuffer   pointer to data buffer.
 * @param[in]   usDataLength    data buffer len.
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_SwapLittleBigEndianData(GU8 *puchDataBuffer, GU16 usDataLength);

/**
 * @fn     void GH3X_DelayUs(GU16 usUsCnt)
 *
 * @brief  Delay N Us for gh3x
 *
 * @attention   None
 *
 * @param[in]   usUsCnt     count need to delay(Us)
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_DelayUs(GU16 usUsCnt);

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
void GH3X_WriteRegs(GU32 unRegAddr, GU16 *usRegValueBuffer,GU8 uchRegNum);
void GH3X_DelayMs(GU16 usUsCnt);

#endif /* _GH3X_DRV_INTERFACE_H_ */

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
