/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x_drv_interface.c
 *
 * @brief   gh3x interface functions
 *
 * @version ref gh3x_drv_version.h
 *
 */


#include <stdio.h>
#include "gh3x_drv_version.h"
#include "gh3x_drv.h"
#include "gh3x_drv_interface.h"
#include "gh3x_drv_control.h"
#include "gh3x_drv_registers.h"
#include "gh3x_demo.h"
/// swap size of GU16
#define SWAP_SIZE_OF_GU16               (2)

/// swap index of next byte
#define SWAP_INDEX_OF_NEXT_BYTE         (1)

/* index of interface buffer */
#define INTERFACE_INDEX_0_BUFFER        (0)     /**< index 0 of buffer */
#define INTERFACE_INDEX_1_BUFFER        (1)     /**< index 1 of buffer */
#define INTERFACE_INDEX_2_BUFFER        (2)     /**< index 2 of buffer */
#define INTERFACE_INDEX_3_BUFFER        (3)     /**< index 3 of buffer */
#define INTERFACE_INDEX_4_BUFFER        (4)     /**< index 4 of buffer */
#define INTERFACE_INDEX_5_BUFFER        (5)     /**< index 5 of buffer */
#define INTERFACE_INDEX_6_BUFFER        (6)     /**< index 6 of buffer */
#define INTERFACE_INDEX_7_BUFFER        (7)     /**< index 7 of buffer */

/* spi operation */
#define SPI_CMD_BUFFER_LEN              (1)     /**< spi cmd buffer len */
#define SPI_WRITE_REG_BUFFER_LEN        (7)     /**< spi write reg buffer len */
#define SPI_WRITE_REG_LEN_H             (0x00)  /**< spi write reg len high byte */ 
#define SPI_WRITE_REG_LEN_L             (0x02)  /**< spi write reg len low byte */
#define SPI_READ_REG_BUFFER_LEN         (5)     /**< spi read reg buffer len */
#define SPI_READ_REG_CMD_LEN            (1)     /**< spi read reg buffer len */
#define SPI_READ_REG_DATA_LEN           (2)     /**< spi read reg buffer len */

/* i2c operation */
#define I2C_CMD_BUFFER_LEN              (3)      /**< i2c cmd buffer len */
#define I2C_WRITE_REG_BUFFER_LEN        (4)      /**< i2c write reg buffer len */
#define I2C_WRITE_REGS_BUFFER_LEN       (2 + 20) /**< i2c write regs buffer len */
#define I2C_READ_REG_ADDR_LEN           (2)      /**< i2c read reg buffer len */
#define I2C_READ_REG_DATA_LEN           (2)      /**< i2c read reg buffer len */

/// i2c device addr
static GU8 guchI2cDeviceId = GH3X_I2C_DEVICE_ID_BASE;

/// spi write func ptr
static pfnSpiRWCtrl gpSpiWriteFunc     = GH3X_PTR_NULL;

/// spi read func ptr
static pfnSpiRWCtrl gpSpiReadFunc      = GH3X_PTR_NULL;

/// spi cs ctrl func ptr
static pfnSpiCsCtrl gpSpiCsCtrlFunc    = GH3X_PTR_NULL;

/// i2c write func ptr
static pfnI2cWrite  gpI2cWriteFunc     = GH3X_PTR_NULL;

/// i2c read func ptr
static pfnI2cRead   gpI2cReadFunc      = GH3X_PTR_NULL;

/// gh3x send cmd func ptr
static pfnSendCmd   gpSendCmdFunc  = GH3X_PTR_NULL;

/// gh3x write reg func ptr
static pfnWriteReg  gpWriteRegFunc = GH3X_PTR_NULL;

/// gh3x write regs func ptr
static pfnWriteRegs  gpWriteRegsFunc = GH3X_PTR_NULL;

/// gh3x read reg func ptr
static pfnReadReg   gpReadRegFunc  = GH3X_PTR_NULL;

/// gh3x read regs func ptr
static pfnReadRegs  gpReadRegsFunc = GH3X_PTR_NULL;

/// gh3x read fifo func ptr
static pfnReadFifo  gpReadFifoFunc = GH3X_PTR_NULL;


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
void GH3X_SwapLittleBigEndianData(GU8 *puchDataBuffer, GU16 usDataLength)
{
    GU16 usIndex = 0;

    for (usIndex = 0; usIndex < (usDataLength / SWAP_SIZE_OF_GU16); usIndex++)
    {
        puchDataBuffer[usIndex * SWAP_SIZE_OF_GU16] ^= \
                                                puchDataBuffer[usIndex * SWAP_SIZE_OF_GU16 + SWAP_INDEX_OF_NEXT_BYTE];
        puchDataBuffer[usIndex * SWAP_SIZE_OF_GU16 + SWAP_INDEX_OF_NEXT_BYTE] ^= \
                                                puchDataBuffer[usIndex * SWAP_SIZE_OF_GU16];
        puchDataBuffer[usIndex * SWAP_SIZE_OF_GU16] ^= \
                                                puchDataBuffer[usIndex * SWAP_SIZE_OF_GU16 + SWAP_INDEX_OF_NEXT_BYTE];
    }
}

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
void GH3X_DelayUs(GU16 usUsCnt)
{
    if (gpDelayUsFunc != GH3X_PTR_NULL)
    {
        gpDelayUsFunc(usUsCnt);
    }
    else
    {
        volatile GU8 uchIndex;
        while (usUsCnt > 0)
        {
            for (uchIndex = 0; uchIndex < GH3X_DELAY_ONE_US_CNT; uchIndex++)
            {
                // do nothing, like _nop(); 
            }
            usUsCnt--;
        }
    }
}

/**
 * @fn     void GH3X_DelayMs(GU16 usUsCnt)
 *
 * @brief  Delay N Ms for gh3x
 *
 * @attention   None
 *
 * @param[in]   usUsCnt     count need to delay(Us)
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_DelayMs(GU16 usUsCnt)
{
    if (gpDelayMsFunc != GH3X_PTR_NULL)
    {
        gpDelayMsFunc(usUsCnt);
    }
    else
    {
        volatile GU32 uchIndex;
        while (usUsCnt > 0)
        {
            for (uchIndex = 0; uchIndex < GH3X_DELAY_ONE_US_CNT * 1000; uchIndex++)
            {
                // do nothing, like _nop(); 
            }
            usUsCnt--;
        }
    }
}

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
GS8 GH3X_SoftReset(void)
{
    GS8 chRet = GH3X_RET_GENERIC_ERROR;
    GH3X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
    GH3X_CHIP_RESET(chRet);
    return chRet;
}

/**
 * @fn     GS8 GH3X_HardReset(void)
 *
 * @brief  softreset via i2c/spi, can read&write reg with chip after reset
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
GS8 GH3X_PING_SET(GU8 ping_set){
    gpResetPinLevelControlFunc(ping_set);
    GH3X_DEBUG_LOG_PARAM("CS PIN SET TO %d\r\n",ping_set);
    return GH3X_RET_OK;
}
GS8 GH3X_HardReset(void)
{
    GS8 chRet = GH3X_RET_GENERIC_ERROR;
    GH3X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
    if (gpResetPinLevelControlFunc != GH3X_PTR_NULL)
    {
        gpResetPinLevelControlFunc(0);
        GH3X_DelayUs(GH3X_HARD_RESET_DELAY_X_US); /* hard reset delay 20us. */
        gpResetPinLevelControlFunc(1);
        chRet = GH3X_RET_OK;
    }
    else
    {
        GH3X_DEBUG_LOG_PARAM("reset pin ctrl func has not register!\r\n");
    }
    return chRet;
}

/**
 * @fn     static void GH3X_SpiSendCmd(GU8 uchCmd)
 *
 * @brief  Send cmd via spi
 *
 * @attention   None
 *
 * @param[in]   uchCmd      spi cmd
 * @param[out]  None
 *
 * @return  None
 */
static void GH3X_SpiSendCmd(GU8 uchCmd)
{
    GU8 uchBuffArr[SPI_CMD_BUFFER_LEN];

    gpSpiCsCtrlFunc(GH3X_SPI_CS_CTRL_LOW);
    uchBuffArr[INTERFACE_INDEX_0_BUFFER] = uchCmd;
    gpSpiWriteFunc(uchBuffArr, SPI_CMD_BUFFER_LEN);
    gpSpiCsCtrlFunc(GH3X_SPI_CS_CTRL_HIGH);
    GH3X_DelayUs(GH3X_SPI_END_DELAY_X_US);
}

/**
 * @fn     static void GH3X_SpiSendCmdHardwareCs(GU8 uchCmd)
 *
 * @brief  Send cmd via spi without cs control
 *
 * @attention   None
 *
 * @param[in]   uchCmd      spi cmd
 * @param[out]  None
 *
 * @return  None
 */
static void GH3X_SpiSendCmdHardwareCs(GU8 uchCmd)
{
    GU8 uchBuffArr[SPI_CMD_BUFFER_LEN];

    uchBuffArr[INTERFACE_INDEX_0_BUFFER] = uchCmd;
    gpSpiWriteFunc(uchBuffArr, SPI_CMD_BUFFER_LEN);
}

/**
 * @fn     static void GH3X_SpiWriteRegs(GU32 unRegAddr, GU16 *usRegValueBuffer,GU8 uchRegNum)
 *
 * @brief  Write regs
 *
 * @attention   None
 *
 * @param[in]   usRegAddr       register addr
 * @param[in]   usRegValue      register data
 * @param[in]   uchRegNum       the number of regs
 *
 * @return  None
 */
static void GH3X_SpiWriteRegs(GU32 unRegAddr, GU16 *usRegValueBuffer,GU8 uchRegNum)
{
    GU8* uchBuffArr=(GU8*)usRegValueBuffer;
    for(GU8 i = 0; i < uchRegNum; i++)
    {
        GU8 uchSwap;
        uchSwap = uchBuffArr[i*sizeof(GU16)];
        uchBuffArr[i*sizeof(GU16)] = uchBuffArr[i*sizeof(GU16) + 1];
        uchBuffArr[i*sizeof(GU16) + 1] = uchSwap;
    }
    GU8 uchSpiBuffArr[GH3X_SPI_BURST_WRITE_BASE_LEN+GH3X_ADDR_BYTE_NUM + uchRegNum*sizeof(GU16)];
    
    GH3X_Memcpy(&(uchSpiBuffArr[GH3X_SPI_BURST_WRITE_BASE_LEN+GH3X_ADDR_BYTE_NUM]),uchBuffArr,uchRegNum*sizeof(GU16));
    uchBuffArr = uchSpiBuffArr;
    gpSpiCsCtrlFunc(GH3X_SPI_CS_CTRL_LOW);
    int set_order=0;
    uchBuffArr[set_order++] = GH3X_SPI_CMD_WRITE;
    for(int i=0;i<GH3X_ADDR_BYTE_NUM;i++)
    {
        uchBuffArr[set_order++]=(unRegAddr>>(8*(GH3X_ADDR_BYTE_NUM-1-i)))&0xFF;
    }
    uchBuffArr[set_order++] = GH3X_GET_HIGH_BYTE_FROM_WORD(uchRegNum*sizeof(GU16));
    uchBuffArr[set_order++] = GH3X_GET_LOW_BYTE_FROM_WORD(uchRegNum*sizeof(GU16));

    gpSpiWriteFunc(uchBuffArr, uchRegNum*sizeof(GU16) + set_order);
    GH3X_DelayUs(GH3X_SPI_WAIT_DELAY_X_US);
    gpSpiCsCtrlFunc(GH3X_SPI_CS_CTRL_HIGH);
    GH3X_DelayUs(GH3X_SPI_END_DELAY_X_US);
}

/**
 * @fn     static void GH3X_SpiWriteReg(GU16 usRegAddr, GU16 usRegValue)
 *
 * @brief  Write register via spi
 *
 * @attention   None
 *
 * @param[in]   usRegAddr       register addr
 * @param[in]   usRegValue      register data
 * @param[out]  None
 *
 * @return  None
 */
static void GH3X_SpiWriteReg(GU32 unRegAddr, GU16 usRegValue)
{
    GU16 usRegValueBuffer[1 + GH3X_SPI_BURST_WRITE_BASE_LEN+GH3X_ADDR_BYTE_NUM] = {usRegValue};
    GH3X_SpiWriteRegs(unRegAddr,usRegValueBuffer,1);
}

/**
 * @fn     static void GH3X_SpiWriteReg(GU16 usRegAddr, GU16 usRegValue)
 *
 * @brief  Write register via spi without cs control
 *
 * @attention   None
 *
 * @param[in]   usRegAddr       register addr
 * @param[in]   usRegValue      register data
 * @param[out]  None
 *
 * @return  None
 */
static void GH3X_SpiWriteRegsHardwareCs(GU32 unRegAddr, GU16 *usRegValueBuffer,GU8 uchRegNum)
{
    
    GU8* uchBuffArr=(GU8*)usRegValueBuffer;
    for(GU8 i = 0; i < uchRegNum; i++)
    {
        GU8 uchSwap;
        uchSwap = uchBuffArr[i*sizeof(GU16)];
        uchBuffArr[i*sizeof(GU16)] = uchBuffArr[i*sizeof(GU16) + 1];
        uchBuffArr[i*sizeof(GU16) + 1] = uchSwap;
    }
    GU8 uchSpiBuffArr[GH3X_SPI_BURST_WRITE_BASE_LEN+GH3X_ADDR_BYTE_NUM + uchRegNum*sizeof(GU16)];
    
    GH3X_Memcpy(&(uchSpiBuffArr[GH3X_SPI_BURST_WRITE_BASE_LEN+GH3X_ADDR_BYTE_NUM]),uchBuffArr,uchRegNum*sizeof(GU16));
    uchBuffArr = uchSpiBuffArr;
    //gpSpiCsCtrlFunc(GH3X_SPI_CS_CTRL_LOW);
    int set_order=0;
    uchBuffArr[set_order++] = GH3X_SPI_CMD_WRITE;
    for(int i=0;i<GH3X_ADDR_BYTE_NUM;i++)
    {
        uchBuffArr[set_order++]=(unRegAddr>>(8*(GH3X_ADDR_BYTE_NUM-1-i)))&0xFF;
    }
    uchBuffArr[set_order++] = GH3X_GET_HIGH_BYTE_FROM_WORD(uchRegNum*sizeof(GU16));
    uchBuffArr[set_order++] = GH3X_GET_LOW_BYTE_FROM_WORD(uchRegNum*sizeof(GU16));

    gpSpiWriteFunc(uchBuffArr, uchRegNum*sizeof(GU16) + set_order);
    GH3X_DelayUs(GH3X_SPI_WAIT_DELAY_X_US);
    //gpSpiCsCtrlFunc(GH3X_SPI_CS_CTRL_HIGH);
    GH3X_DelayUs(GH3X_SPI_END_DELAY_X_US);
	
}

/**
 * @fn     static void GH3X_SpiWriteRegHardwareCs(GU32 unRegAddr, GU16 usRegValue)
 *
 * @brief  Write regs without cs control
 *
 * @attention   None
 *
 * @param[in]   usRegAddr       register addr
 * @param[in]   usRegValue      register data
 * @param[in]   uchRegNum       the number of regs
 *
 * @return  None
 */
static void GH3X_SpiWriteRegHardwareCs(GU32 unRegAddr, GU16 usRegValue)
{
    GU16 usRegValueBuffer[1 + GH3X_SPI_BURST_WRITE_BASE_LEN+GH3X_ADDR_BYTE_NUM] = {usRegValue};
    GH3X_SpiWriteRegsHardwareCs(unRegAddr,usRegValueBuffer,1);
}


/**
 * @fn     static void GH3X_SpiReadRegs(GU16 usRegAddr, GU16 *pusRegValueBuffer, GU8 uchLen)
 *
 * @brief  Read multi register via spi
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
int first_byte_flag = 1;
static void GH3X_SpiReadRegs(GU32 unRegAddr, GU16 *pusRegValueBuffer, GU8 ucRegNum)
{
    GU8 uchBuffArr[SPI_READ_REG_BUFFER_LEN] = {0};
    // if(first_byte_flag){
    //     EXAMPLE_LOG("first_WRITE_byte_flag!\n");
    // }
    gpSpiCsCtrlFunc(GH3X_SPI_CS_CTRL_LOW);
    int set_order=0;
    uchBuffArr[set_order++] = GH3X_SPI_CMD_WRITE;
    for(int i=0;i<GH3X_ADDR_BYTE_NUM;i++)
    {
        uchBuffArr[set_order++]=(unRegAddr>>(8*(GH3X_ADDR_BYTE_NUM-1-i)))&0xFF;
    }
    gpSpiWriteFunc(uchBuffArr, set_order);
    // if(first_byte_flag){
    //     EXAMPLE_LOG("write byte (len=%d):", set_order);
    //     for(int j=0; j<set_order; j++){
    //         EXAMPLE_LOG("0x%02X ", uchBuffArr[j]);  // 按字节打印十六进制值
    //     }
    // }
    GH3X_DelayUs(GH3X_SPI_WAIT_DELAY_X_US);
    gpSpiCsCtrlFunc(GH3X_SPI_CS_CTRL_HIGH);
    GH3X_DelayUs(GH3X_SPI_END_DELAY_X_US);
    gpSpiCsCtrlFunc(GH3X_SPI_CS_CTRL_LOW);
    uchBuffArr[INTERFACE_INDEX_0_BUFFER] = GH3X_SPI_CMD_READ;
    // if(first_byte_flag){
    //     GH3X_DEBUG_LOG_PARAM("write byte (len=%d):", set_order);
    //     for(int j=0; j<set_order; j++){
    //         GH3X_DEBUG_LOG_PARAM("0x%02X ", uchBuffArr[j]);  // 按字节打印十六进制值
    //     }
    // }
    gpSpiWriteFunc(uchBuffArr, SPI_READ_REG_CMD_LEN);
    gpSpiReadFunc((GU8 *)pusRegValueBuffer, ucRegNum * SPI_READ_REG_DATA_LEN);
    GH3X_SwapLittleBigEndianData((GU8 *)pusRegValueBuffer, (GU16)((GU16)ucRegNum * SPI_READ_REG_DATA_LEN));
    GH3X_DelayUs(GH3X_SPI_WAIT_DELAY_X_US);
    gpSpiCsCtrlFunc(GH3X_SPI_CS_CTRL_HIGH);
    GH3X_DelayUs(GH3X_SPI_END_DELAY_X_US);
}
 

/**
 * @fn     static GU16 GH3X_SpiReadReg(GU16 usRegAddr)
 *
 * @brief  Read register via spi
 *
 * @attention   None
 *
 * @param[in]   usRegAddr      register addr
 * @param[out]  None
 *
 * @return  register data
 */
static GU16 GH3X_SpiReadReg(GU32 unRegAddr)
{
    GU16 usRegValueBuffer[1];
    GH3X_SpiReadRegs( unRegAddr, usRegValueBuffer,1);
    return usRegValueBuffer[0];
}

static void GH3X_SpiReadRegsHardwareCs(GU32 unRegAddr, GU16 *pusRegValueBuffer, GU8 ucRegNum)
{
    GU8 uchBuffArr[SPI_READ_REG_BUFFER_LEN] = {0};

    //gpSpiCsCtrlFunc(GH3X_SPI_CS_CTRL_LOW);
    int set_order=0;
    uchBuffArr[set_order++] = GH3X_SPI_CMD_WRITE;
    for(int i=0;i<GH3X_ADDR_BYTE_NUM;i++)
    {
        uchBuffArr[set_order++]=(unRegAddr>>(8*(GH3X_ADDR_BYTE_NUM-1-i)))&0xFF;
    }
    gpSpiWriteFunc(uchBuffArr, set_order);
    GH3X_DelayUs(GH3X_SPI_WAIT_DELAY_X_US);
    
    gpSpiReadFunc((GU8 *)pusRegValueBuffer, ucRegNum * SPI_READ_REG_DATA_LEN);
    GH3X_SwapLittleBigEndianData((GU8 *)pusRegValueBuffer, (GU16)((GU16)ucRegNum * SPI_READ_REG_DATA_LEN));
    GH3X_DelayUs(GH3X_SPI_WAIT_DELAY_X_US);
 
}

/**
 * @fn     static GU16 GH3X_SpiReadRegHardwareCs(GU32 unRegAddr)
 *
 * @brief  Read fifo via spi without cs control
 *
 * @attention   None
 *
 * @param[in]   puchDataBuffer      pointer to buffer for fifo rawdata
 * @param[in]   usLen               fifo bytes len
 * @param[out]  None
 *
 * @return  None
 */
static GU16 GH3X_SpiReadRegHardwareCs(GU32 unRegAddr)
{
    GU16 usRegValueBuffer[1];
    GH3X_SpiReadRegsHardwareCs( unRegAddr, usRegValueBuffer,1);
    return usRegValueBuffer[0];
}

/**
 * @fn     static void GH3X_SpiReadFifo(GU8 *puchDataBuffer, GU16 usLen)
 *
 * @brief  Read fifo via spi
 *
 * @attention   None
 *
 * @param[in]   puchDataBuffer      pointer to buffer for fifo rawdata
 * @param[in]   usLen               fifo bytes len
 * @param[out]  None
 *
 * @return  None
 */
static void GH3X_SpiReadFifo(GU8 *puchDataBuffer, GU16 usLen)
{
    GU8 uchBuffArr[SPI_READ_REG_BUFFER_LEN] = {0};
    int set_order=0;
    uchBuffArr[set_order++] = GH3X_SPI_CMD_WRITE;
    for(int i=0;i<GH3X_ADDR_BYTE_NUM;i++)
    {
        uchBuffArr[set_order++] = (GH3X_FIFO_REG_ADDR>>(8*(GH3X_ADDR_BYTE_NUM-1-i)))&0xFF;

    }
    
    gpSpiCsCtrlFunc(GH3X_SPI_CS_CTRL_LOW);
    gpSpiWriteFunc(uchBuffArr, set_order);
    GH3X_DelayUs(GH3X_SPI_WAIT_DELAY_X_US);
    gpSpiCsCtrlFunc(GH3X_SPI_CS_CTRL_HIGH);
    
    GH3X_DelayUs(GH3X_SPI_END_DELAY_X_US);
    
    gpSpiCsCtrlFunc(GH3X_SPI_CS_CTRL_LOW);
    uchBuffArr[INTERFACE_INDEX_0_BUFFER] = GH3X_SPI_CMD_READ;
    gpSpiWriteFunc(uchBuffArr, SPI_READ_REG_CMD_LEN);
    
    gpSpiReadFunc(puchDataBuffer, usLen);
    GH3X_DelayUs(GH3X_SPI_WAIT_DELAY_X_US);
    gpSpiCsCtrlFunc(GH3X_SPI_CS_CTRL_HIGH);
    GH3X_DelayUs(GH3X_SPI_END_DELAY_X_US);
}

/**
 * @fn     static void GH3X_SpiReadFifoHardwareCs(GU8 *puchDataBuffer, GU16 usLen)
 *
 * @brief  Read fifo via spi without cs control
 *
 * @attention   None
 *
 * @param[in]   puchDataBuffer      pointer to buffer for fifo rawdata
 * @param[in]   usLen               fifo bytes len
 * @param[out]  None
 *
 * @return  None
 */
static void GH3X_SpiReadFifoHardwareCs(GU8 *puchDataBuffer, GU16 usLen)
{
    GU8 uchBuffArr[SPI_READ_REG_BUFFER_LEN] = {0};
    int set_order=0;
    uchBuffArr[set_order++] = GH3X_SPI_CMD_WRITE;
    for(int i=0;i<GH3X_ADDR_BYTE_NUM;i++)
    {
        uchBuffArr[set_order++] = (GH3X_FIFO_REG_ADDR>>(8*(GH3X_ADDR_BYTE_NUM-1-i)))&0xFF;

    }
    
    gpSpiWriteFunc(uchBuffArr, set_order);
    GH3X_DelayUs(GH3X_SPI_WAIT_DELAY_X_US);
    GH3X_DelayUs(GH3X_SPI_END_DELAY_X_US);
    
    uchBuffArr[INTERFACE_INDEX_0_BUFFER] = GH3X_SPI_CMD_READ;
    gpSpiWriteFunc(uchBuffArr, SPI_READ_REG_CMD_LEN);
    
    gpSpiReadFunc(puchDataBuffer, usLen);
    GH3X_DelayUs(GH3X_SPI_WAIT_DELAY_X_US);
    GH3X_DelayUs(GH3X_SPI_END_DELAY_X_US);
	
}

/**
 * @fn     static void GH3X_I2cSendCmd(GU8 uchCmd)
 *
 * @brief  Send cmd via i2c
 *
 * @attention   None
 *
 * @param[in]   uchCmd      i2c cmd
 * @param[out]  None
 *
 * @return  None
 */
static void GH3X_I2cSendCmd(GU8 uchCmd)
{
    GU8 uchBuffArr[I2C_CMD_BUFFER_LEN] = {0};

    uchBuffArr[INTERFACE_INDEX_0_BUFFER] = GH3X_GET_HIGH_BYTE_FROM_WORD(GH3X_I2C_CMD_ADDR);
    uchBuffArr[INTERFACE_INDEX_1_BUFFER] = GH3X_GET_LOW_BYTE_FROM_WORD(GH3X_I2C_CMD_ADDR);
    uchBuffArr[INTERFACE_INDEX_2_BUFFER] = uchCmd;
    gpI2cWriteFunc(guchI2cDeviceId, uchBuffArr, I2C_CMD_BUFFER_LEN);
}

/**
 * @fn     static void GH3X_I2cWriteReg(GU32 unRegAddr, GU16 usRegValue)
 *
 * @brief  Write register via i2c
 *
 * @attention   None
 *
 * @param[in]   usRegAddr       register addr
 * @param[in]   usRegValue      register data
 * @param[out]  None
 *
 * @return  None
 */
static void GH3X_I2cWriteReg(GU32 unRegAddr, GU16 usRegValue)
{
    GU8 uchBuffArr[I2C_WRITE_REG_BUFFER_LEN] = {0};

    uchBuffArr[INTERFACE_INDEX_0_BUFFER] = GH3X_GET_HIGH_BYTE_FROM_WORD(unRegAddr);
    uchBuffArr[INTERFACE_INDEX_1_BUFFER] = GH3X_GET_LOW_BYTE_FROM_WORD(unRegAddr);
    uchBuffArr[INTERFACE_INDEX_2_BUFFER] = GH3X_GET_HIGH_BYTE_FROM_WORD(usRegValue);
    uchBuffArr[INTERFACE_INDEX_3_BUFFER] = GH3X_GET_LOW_BYTE_FROM_WORD(usRegValue);
    gpI2cWriteFunc(guchI2cDeviceId, uchBuffArr, I2C_WRITE_REG_BUFFER_LEN);
}

/**
 * @fn     static void GH3X_I2cWriteRegs(GU32 unRegAddr, GU16* pusRegValues, GU16 usRegNum)
 *
 * @brief  Write register via i2c
 *
 * @attention   None
 *
 * @param[in]   usRegAddr         register addr
 * @param[in]   pusRegValues      register data
 * @param[in]   usRegNum          register data len
 * @param[out]  None
 *
 * @return  None
 */
static void GH3X_I2cWriteRegs(GU32 unRegAddr, GU16 *usRegValueBuffer,GU8 uchRegNum)
{
    GU8 uchBuffArr[I2C_WRITE_REGS_BUFFER_LEN] = {0};
    GU16 usMaxRegNum = (I2C_WRITE_REGS_BUFFER_LEN - I2C_READ_REG_ADDR_LEN) / I2C_READ_REG_DATA_LEN;
    uchBuffArr[INTERFACE_INDEX_0_BUFFER] = GH3X_GET_HIGH_BYTE_FROM_WORD(unRegAddr);
    uchBuffArr[INTERFACE_INDEX_1_BUFFER] = GH3X_GET_LOW_BYTE_FROM_WORD(unRegAddr);
    uchRegNum = (uchRegNum > usMaxRegNum) ? usMaxRegNum : uchRegNum;

    for(GU16 i = 0; i < uchRegNum; i++)
    {
        uchBuffArr[INTERFACE_INDEX_2_BUFFER + 2 * i] = GH3X_GET_HIGH_BYTE_FROM_WORD(usRegValueBuffer[i]);
        uchBuffArr[INTERFACE_INDEX_3_BUFFER + 2 * i] = GH3X_GET_LOW_BYTE_FROM_WORD(usRegValueBuffer[i]);
    }

    gpI2cWriteFunc(guchI2cDeviceId, uchBuffArr, I2C_READ_REG_ADDR_LEN + I2C_READ_REG_DATA_LEN * uchRegNum);
}

/**
 * @fn     static GU16 GH3X_I2cReadReg(GU32 unRegAddr)
 *
 * @brief  Read register via i2c
 *
 * @attention   None
 *
 * @param[in]   usRegAddr      register addr
 * @param[out]  None
 *
 * @return  register data
 */
static GU16 GH3X_I2cReadReg(GU32 unRegAddr)
{
    GU8 uchCmdBuffArr[I2C_READ_REG_ADDR_LEN] = {0};
    GU8 uchReadBuffArr[I2C_READ_REG_DATA_LEN] = {0};

    uchCmdBuffArr[INTERFACE_INDEX_0_BUFFER] = GH3X_GET_HIGH_BYTE_FROM_WORD(unRegAddr);
    uchCmdBuffArr[INTERFACE_INDEX_1_BUFFER] = GH3X_GET_LOW_BYTE_FROM_WORD(unRegAddr);
    gpI2cReadFunc(guchI2cDeviceId, uchCmdBuffArr, I2C_READ_REG_ADDR_LEN, uchReadBuffArr, I2C_READ_REG_DATA_LEN);
    return GH3X_MAKEUP_WORD(uchReadBuffArr[INTERFACE_INDEX_0_BUFFER], uchReadBuffArr[INTERFACE_INDEX_1_BUFFER]);
}

/**
 * @fn     static void GH3X_I2cReadRegs(GU16 usRegAddr, GU16 *pusRegValueArr, GU8 uchLen)
 *
 * @brief  Read multi register via i2c
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
static void GH3X_I2cReadRegs(GU32 unRegAddr, GU16 *pusRegValueArr, GU8 uchLen)
{
    GU8 uchBuffArr[I2C_READ_REG_ADDR_LEN] = {0};

    uchBuffArr[INTERFACE_INDEX_0_BUFFER] = GH3X_GET_HIGH_BYTE_FROM_WORD(unRegAddr);
    uchBuffArr[INTERFACE_INDEX_1_BUFFER] = GH3X_GET_LOW_BYTE_FROM_WORD(unRegAddr);
    gpI2cReadFunc(guchI2cDeviceId, uchBuffArr, I2C_READ_REG_ADDR_LEN, \
                        (GU8 *)pusRegValueArr, uchLen * I2C_READ_REG_DATA_LEN);
    GH3X_SwapLittleBigEndianData((GU8 *)pusRegValueArr, (GU16)uchLen * I2C_READ_REG_DATA_LEN);
}

/**
 * @fn     static void GH3X_I2cReadFifo(GU8 *puchDataBuffer, GU16 usLen)
 *
 * @brief  Read fifo via i2c
 *
 * @attention   None
 *
 * @param[in]   puchDataBuffer      pointer to buffer for fifo rawdata
 * @param[in]   usLen               fifo bytes len
 * @param[out]  None
 *
 * @return  None
 */
static void GH3X_I2cReadFifo(GU8 *puchDataBuffer, GU16 usLen)
{
    GU8 uchCmdBuffArr[I2C_READ_REG_ADDR_LEN] = {0};

    uchCmdBuffArr[INTERFACE_INDEX_0_BUFFER] = GH3X_GET_HIGH_BYTE_FROM_WORD(GH3X_FIFO_REG_ADDR);
    uchCmdBuffArr[INTERFACE_INDEX_1_BUFFER] = GH3X_GET_LOW_BYTE_FROM_WORD(GH3X_FIFO_REG_ADDR);
    gpI2cReadFunc(guchI2cDeviceId, uchCmdBuffArr, I2C_READ_REG_ADDR_LEN, puchDataBuffer, usLen);
}

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
void GH3X_SendCmd(GU8 uchCmd)
{
    if (gpSendCmdFunc != GH3X_PTR_NULL)
    {
        gpSendCmdFunc(uchCmd);
    }
    else
    {
        GH3X_DEBUG_LOG_PARAM("communicate interface register error!\r\n");
    }
}

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
void GH3X_WriteReg(GU16 usRegAddr, GU16 usRegValue)
{
    if (gpWriteRegFunc != GH3X_PTR_NULL)
    {
        gpWriteRegFunc(usRegAddr, usRegValue);
    }
    else
    {
        GH3X_DEBUG_LOG_PARAM("communicate interface register error!\r\n");
    }
}

/**
 * @fn     void GH3X_WriteRegs(GU16 usRegAddr, GU16 usRegValue)
 *
 * @brief  Write registers via i2c or spi
 *
 * @attention   None
 *
 * @param[in]   usRegAddr       register addr
 * @param[in]   usRegValue      register data
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_WriteRegs(GU32 unRegAddr, GU16 *usRegValueBuffer,GU8 uchRegNum)
{
    if (gpWriteRegsFunc != GH3X_PTR_NULL)
    {
        gpWriteRegsFunc(unRegAddr,usRegValueBuffer,uchRegNum);
    }
    else
    {
        GH3X_DEBUG_LOG_PARAM("communicate interface register error!\r\n");
    }
}

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
GU16 GH3X_ReadReg(GU16 usRegAddr)
{
    GU16 usRegData = 0;
    if (gpReadRegFunc != GH3X_PTR_NULL)
    {
        usRegData = gpReadRegFunc(usRegAddr);
    }
    else
    {
        GH3X_DEBUG_LOG_PARAM("communicate interface register error!\r\n");
    }
    return usRegData;
}

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
GU32 GH3X_ReadRegs(GU16 usRegAddr, GU16 *pusRegValueBuffer, GU16 usLen)
{
    GU32 unRegData = 0;
    if (gpReadRegsFunc != GH3X_PTR_NULL)
    {
        gpReadRegsFunc(usRegAddr, pusRegValueBuffer, usLen);
    }
    else
    {
        GH3X_DEBUG_LOG_PARAM("communicate interface register error!\r\n");
    }
    return unRegData;
}

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
void GH3X_ReadFifo(GU8 *puchDataBuffer, GU16 usLen)
{
    if (gpReadFifoFunc != GH3X_PTR_NULL)
    {
        gpReadFifoFunc(puchDataBuffer, usLen);
    }
    else
    {
        GH3X_DEBUG_LOG_PARAM("communicate interface register error!\r\n");
    }
}

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
void GH3X_WriteRegBitField(GU32 unRegAddr, GU8 uchLsb, GU8 uchMsb, GU16 usValue)
{
    GU16 usMakData = ((((GU16)0x0001) << (uchMsb - uchLsb + 1)) - 1) << uchLsb;
    GU16 usRegData = 0;
    if ((gpReadRegFunc != GH3X_PTR_NULL) && (gpWriteRegFunc != GH3X_PTR_NULL))
    {
        usRegData = gpReadRegFunc(unRegAddr);
        GH3X_VAL_CLEAR_BIT(usRegData, usMakData);
        GH3X_VAL_SET_BIT(usRegData, (usValue << uchLsb) & usMakData);
        gpWriteRegFunc(unRegAddr, usRegData);
    }
    else
    {
        GH3X_DEBUG_LOG_PARAM("communicate interface register error!\r\n");
    }
}
void GH3X_WriteRegBitFields(const STRegField stRegFields[], GU16 usFieldNum)
{
    for(int i = 0; i < usFieldNum ;i++)
    {
        GH3X_WriteRegBitField(stRegFields[i].unRegAddr, stRegFields[i].uchLsb, stRegFields[i].uchMsb, stRegFields[i].usRegData);
    }
}

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
GU16 GH3X_ReadRegBitField(GU16 usRegAddr, GU8 uchLsb, GU8 uchMsb)
{
    GU16 usMakData = ((((GU16)0x0001) << (uchMsb - uchLsb + 1)) - 1) << uchLsb;
    GU16 usRegData = 0;
    if (gpReadRegFunc != GH3X_PTR_NULL)
    {
        usRegData = gpReadRegFunc(usRegAddr);
        GH3X_VAL_GET_BIT(usRegData, usMakData);
        usRegData >>= uchLsb;
    }
    else
    {
        GH3X_DEBUG_LOG_PARAM("communicate interface register error!\r\n");
    }
    return usRegData;
}

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
                                    void (*pSpiCsCtrlFunc)(GU8 uchCsLevelHigh))
{
    GS8 chRet = GH3X_RET_PARAMETER_ERROR;
    GH3X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
    if ((pSpiWriteFunc != GH3X_PTR_NULL) && (pSpiReadFunc != GH3X_PTR_NULL) && (pSpiCsCtrlFunc != GH3X_PTR_NULL))
    {
        gpSpiWriteFunc  = pSpiWriteFunc;
        gpSpiReadFunc   = pSpiReadFunc;
        gpSpiCsCtrlFunc = pSpiCsCtrlFunc;

        gpSendCmdFunc  = GH3X_SpiSendCmd;
        gpWriteRegFunc = GH3X_SpiWriteReg;
        gpWriteRegsFunc = GH3X_SpiWriteRegs;
        gpReadRegFunc  = GH3X_SpiReadReg;
        gpReadRegsFunc = GH3X_SpiReadRegs;
        gpReadFifoFunc = GH3X_SpiReadFifo;
        chRet = GH3X_RET_OK;
    }
    else
    {
        GH3X_DEBUG_LOG_PARAM("spi register param error!\r\n");
    }
    return chRet;
}

/**
 * @fn     GS8 GH3X_RegisterSpiHwCsOperationFunc(GU8 (*pSpiWriteFunc)(GU8 uchWriteBytesArr[], GU16 usWriteLen),
 *                                   GU8 (*pSpiWriteF1ReadFunc)(GU8 uchReadBytesArr[], GU16 usMaxReadLen))
 *
 * @brief  Register spi operation function callback without cs control
 *
 * @attention   None
 *
 * @param[in]   pSpiWriteFunc       pointer to spi write function
 * @param[in]   pSpiWriteF1ReadFunc        pointer to spi read function
 * @param[out]  None
 *
 * @return  errcode
 * @retval  #GH3X_RET_OK               return successfully
 * @retval  #GH3X_RET_PARAMETER_ERROR  spi operation function pointer parameter error
 */
GS8 GH3X_RegisterSpiHwCsOperationFunc(GU8 (*pSpiWriteFunc)(GU8 uchWriteBytesArr[], GU16 usWriteLen),
                                    GU8 (*pSpiWriteF1ReadFunc)(GU8 uchReadBytesArr[], GU16 usMaxReadLen))
{
    GS8 chRet = GH3X_RET_PARAMETER_ERROR;
    GH3X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
    if ((pSpiWriteFunc != GH3X_PTR_NULL) && (pSpiWriteF1ReadFunc != GH3X_PTR_NULL) )
    {
        gpSpiWriteFunc  = pSpiWriteFunc;
        gpSpiReadFunc   = pSpiWriteF1ReadFunc;

        gpSendCmdFunc  = GH3X_SpiSendCmdHardwareCs;
        gpWriteRegFunc = GH3X_SpiWriteRegHardwareCs;
        gpWriteRegsFunc = GH3X_SpiWriteRegsHardwareCs;
        gpReadRegFunc  = GH3X_SpiReadRegHardwareCs;
        gpReadRegsFunc = GH3X_SpiReadRegsHardwareCs;
        gpReadFifoFunc = GH3X_SpiReadFifoHardwareCs;
        chRet = GH3X_RET_OK;
    }
    else
    {
        GH3X_DEBUG_LOG_PARAM("spi register param error!\r\n");
    }
    return chRet;
}

/**
 * @fn     GS8 GH3X_RegisterI2cOperationFunc(GU8 uchI2cIdLowTwoBitsSelect,
 *                              GU8 (*pI2cWriteFunc)(GU8 uchDeviceId, const GU8 uchWriteBytesArr[], GU16 usWriteLen),
 *                              GU8 (*pI2cReadFunc)(GU8 uchDeviceId, const GU8 uchCmddBytesArr[], GU16 usCmddLen,
 *                                                      GU8 uchReadBytesArr[], GU16 usMaxReadLen))
 *
 * @brief  Register i2c operation function callback
 *
 * @attention   None
 *
 * @param[in]   uchI2cIdLowTwoBitsSelect    i2c low two bits addr selected, ref EMGh3xI2cIdSelect
 * @param[in]   pI2cWriteFunc               pointer to i2c write function
 * @param[in]   pI2cReadFunc                pointer to i2c read function
 * @param[out]  None
 *
 * @return  errcode
 * @retval  #GH3X_RET_OK               return successfully
 * @retval  #GH3X_RET_PARAMETER_ERROR  i2c operation function pointer parameter error
 */
GS8 GH3X_RegisterI2cOperationFunc(GU8 uchI2cIdLowTwoBitsSelect,
                                GU8 (*pI2cWriteFunc)(GU8 uchDeviceId, const GU8 uchWriteBytesArr[], GU16 usWriteLen),
                                GU8 (*pI2cReadFunc)(GU8 uchDeviceId, const GU8 uchCmddBytesArr[], GU16 usCmddLen,
                                                        GU8 uchReadBytesArr[], GU16 usMaxReadLen))
{
    GS8 chRet = GH3X_RET_PARAMETER_ERROR;
    GH3X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
    if ((pI2cWriteFunc != GH3X_PTR_NULL) && (pI2cReadFunc != GH3X_PTR_NULL)
        && (uchI2cIdLowTwoBitsSelect < GH3X_I2C_ID_INVALID))
    {
        guchI2cDeviceId = GH3X_I2C_DEVICE_ID_BASE | (uchI2cIdLowTwoBitsSelect << 1);
        gpI2cWriteFunc  = pI2cWriteFunc;
        gpI2cReadFunc   = pI2cReadFunc;

        gpSendCmdFunc  = GH3X_I2cSendCmd;
        gpWriteRegFunc = GH3X_I2cWriteReg;
        gpWriteRegsFunc = GH3X_I2cWriteRegs;
        gpReadRegFunc  = GH3X_I2cReadReg;
        gpReadRegsFunc = GH3X_I2cReadRegs;
        gpReadFifoFunc = GH3X_I2cReadFifo;
        chRet = GH3X_RET_OK;
    }
    else
    {
        GH3X_DEBUG_LOG_PARAM("i2c register param error!\r\n");
    }
    return chRet;
}

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
