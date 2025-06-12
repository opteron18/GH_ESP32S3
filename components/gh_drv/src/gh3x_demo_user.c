/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 * 
 * @file    gh3x_demo_user.c
 * 
 * @brief   gh3x driver lib demo code that user defined
 * 
 * @author  
 * 
 */

/* includes */
#include "stdint.h"
#include "string.h"
#include "gh3x_drv.h"
#include "gh3x_demo_config.h"
#include "gh3x_demo_inner.h"
#include "gh3x_demo.h"
#include "esp_rom_sys.h"
#include "esp_log.h"

extern esp_err_t gh_spi_send(uint8_t *tx_data, size_t length);
extern esp_err_t gh_spi_receive(uint8_t *rx_data, size_t length);
extern void spi_driver_init();
extern esp_err_t spi_cs_set_level(uint32_t level);
extern void gh3_int_init(void);

#if ( __GH3X_INTERFACE__ == __GH3X_INTERFACE_I2C__ )

/* i2c interface */
/**
 * @fn     void hal_gh3x_i2c_init(void)
 * 
 * @brief  hal i2c init for gh3x
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
GH_WEAK void hal_gh3x_i2c_init(void)
{

    /* code implement by user */
    GOODIX_PLATFROM_I2C_INIT_ENTITY();

}

/**
 * @fn     uint8_t hal_gh3x_i2c_write(uint8_t device_id, const uint8_t write_buffer[], uint16_t length)
 * 
 * @brief  hal i2c write for gh3x
 *
 * @attention   device_id is 8bits, if platform i2c use 7bits addr, should use (device_id >> 1)
 *
 * @param[in]   device_id       device addr
 * @param[in]   write_buffer    write data buffer
 * @param[in]   length          write data len
 * @param[out]  None
 *
 * @return  status
 * @retval  #1      return successfully
 * @retval  #0      return error
 */
GH_WEAK GU8 hal_gh3x_i2c_write(GU8 device_id, const GU8 write_buffer[], GU16 length)
{
    uint8_t ret = 1;

    /* code implement by user */

    GOODIX_PLATFROM_I2C_WRITE_ENTITY(device_id, write_buffer,length);
    return ret;
}

/**
 * @fn     uint8_t hal_gh3x_i2c_read(uint8_t device_id, const uint8_t write_buffer[], uint16_t write_length,
 *                            uint8_t read_buffer[], uint16_t read_length)
 * 
 * @brief  hal i2c read for gh3x
 *
 * @attention   device_id is 8bits, if platform i2c use 7bits addr, should use (device_id >> 1)
 *
 * @param[in]   device_id       device addr
 * @param[in]   write_buffer    write data buffer
 * @param[in]   write_length    write data len
 * @param[in]   read_length     read data len
 * @param[out]  read_buffer     pointer to read buffer
 *
 * @return  status
 * @retval  #1      return successfully
 * @retval  #0      return error
 */
GH_WEAK GU8 hal_gh3x_i2c_read(GU8 device_id, const GU8 write_buffer[], GU16 write_length, GU8 read_buffer[], GU16 read_length)
{
    uint8_t ret = 1;

    /* code implement by user */

    GOODIX_PLATFROM_I2C_READ_ENTITY(device_id, write_buffer, write_length, read_buffer, read_length);
    return ret;
}

#else // __GH3X_INTERFACE__ == __GH3X_INTERFACE_SPI__

/* spi interface */
/**
 * @fn     void hal_gh3x_spi_init(void)
 * 
 * @brief  hal spi init for gh3x
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */

GH_WEAK void hal_gh3x_spi_init(void)
{
    /* init spi and cs pin */
    spi_driver_init();
    // GOODIX_PLATFROM_SPI_INIT_ENTITY();
}

/**
 * @fn     GU8 hal_gh3x_spi_write(GU8 write_buffer[], GU16 length)
 * 
 * @brief  hal spi write for gh3x
 *
 * @attention   if __GH3X_SPI_TYPE__ == __GH3X_SPI_TYPE_SOFTWARE_CS__  , user need generate timming: write_buf[0](W) + write_buf[1](W) + ...
 * @attention   if __GH3X_SPI_TYPE__ == __GH3X_SPI_TYPE_HARDWARE_CS__  , user need generate timming: CS LOW  + write_buf[0](W) + write_buf[1](W) + ... + CS HIGH
 *
 * @param[in]   write_buffer    write data buffer
 * @param[in]   length          write data len
 * @param[out]  None
 *
 * @return  status
 * @retval  #1      return successfully
 * @retval  #0      return error
 */
GH_WEAK GU8 hal_gh3x_spi_write(GU8 write_buffer[], GU16 length)
{
    GU8 ret = 1;
    // #define TAG "hal_gh3x_spi_write"
    // ESP_LOGI(TAG, "write_buffer: %p, length: %u", write_buffer, length);
    gh_spi_send(write_buffer, length);
    // GOODIX_PLATFROM_SPI_WRITE_ENTITY(write_buffer, length);

    return ret;
}


#if (__GH3X_SPI_TYPE__ == __GH3X_SPI_TYPE_SOFTWARE_CS__) 
/**
 * @fn     GU8 hal_gh3x_spi_read(GU8 read_buffer[], GU16 length)
 * 
 * @brief  hal spi read for gh3x
 *
 * @attention   user need generate timming: read_buf[0](R) + write_buf[1](R) + ... 
 *
 * @param[in]   read_length     read data len
 * @param[out]  read_buffer     pointer to read buffer
 *
 * @return  status
 * @retval  #1      return successfully
 * @retval  #0      return error
 */
GH_WEAK GU8 hal_gh3x_spi_read(GU8 read_buffer[], GU16 length)
{
    GU8 ret = 1;
    gh_spi_receive(read_buffer, length);
    // GOODIX_PLATFROM_SPI_READ_ENTITY();

    return ret;
}
#elif (__GH3X_SPI_TYPE__ == __GH3X_SPI_TYPE_HARDWARE_CS__)
/**
 * @fn     GU8 hal_gh3x_spi_write_F1_and_read(GU8 read_buffer[], GU16 length)
 * 
 * @brief  hal spi write F1 and read for gh3x
 *
 * @attention    user need generate timming: CS LOW + F1(W) + read_buf[0](R) + read_buf[1](R) + ... + CS HIGH
 *
 * @param[in]   write_buf     write data
 * @param[in]   length     write data len
 *
 * @return  status
 * @retval  #1      return successfully
 * @retval  #0      return error
 */
GH_WEAK GU8 hal_gh3x_spi_write_F1_and_read(GU8 read_buffer[], GU16 length)
{
    GU8 ret = 1;
    
     GOODIX_PLATFROM_SPI_READ_ENTITY();
    
    return ret;
}
#endif

/**
 * @fn     void hal_gh3x_spi_cs_ctrl(GU8 cs_pin_level)
 * 
 * @brief  hal spi cs pin ctrl for gh3x
 *
 * @attention   pin level set 1 [high level] or 0 [low level]
 *
 * @param[in]   cs_pin_level     spi cs pin level
 * @param[out]  None
 *
 * @return  None
 */
#if (__GH3X_SPI_TYPE__ == __GH3X_SPI_TYPE_SOFTWARE_CS__) 
GH_WEAK void hal_gh3x_spi_cs_ctrl(GU8 cs_pin_level)
{
    spi_cs_set_level(cs_pin_level);
    // GOODIX_PLATFROM_SPI_CS_CTRL_ENTITY();

}
#endif

#endif

#if __SUPPORT_HARD_RESET_CONFIG__

/**
 * @fn     void hal_gh3x_int_init(void)
 * 
 * @brief  gh3x int init
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */

extern esp_err_t esp_spi_reset_pin_init(void);

GH_WEAK void hal_gh3x_reset_pin_init(void)
{
    esp_spi_reset_pin_init();
    // GOODIX_PLATFROM_RESET_PIN_INIT_ENTITY();
}

/**
 * @fn     void hal_gh3x_reset_pin_ctrl(GU8 pin_level)
 * 
 * @brief  hal reset pin ctrl for gh3x
 *
 * @attention   pin level set 1 [high level] or 0 [low level]
 *
 * @param[in]   pin_level     reset pin level
 * @param[out]  None
 *
 * @return  None
 */

extern esp_err_t esp_spi_reset_pin_ctrl(uint32_t level);
GH_WEAK void hal_gh3x_reset_pin_ctrl(GU8 pin_level)
{
    esp_spi_reset_pin_ctrl(pin_level);
//    GOODIX_PLATFROM_SPI_RESET_PIN_ENTITY();
}

#endif

/**
 * @fn     void hal_gh3x_int_init(void)
 * 
 * @brief  gh3x int init
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
GH_WEAK void hal_gh3x_int_init(void)
{
    gh3_int_init();
    // GOODIX_PLATFROM_INT_INIT_ENTITY();
}

#if (__INTERRUPT_PROCESS_MODE__ != __POLLING_INT_PROCESS_MODE__)
/**
 * @fn     void hal_gh3x_int_handler_call_back(void)
 * 
 * @brief  call back of gh3x int handler
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
GH_WEAK void hal_gh3x_int_handler_call_back(void)
{   
    // gh3_int_init();
    // GOODIX_PLATFROM_INT_HANDLER_CALL_BACK_ENTITY();
}
#endif

/**
 * @fn     void hal_gsensor_start_cache_data(void)
 * 
 * @brief  Start cache gsensor data for gh3x
 *
 * @attention   This function will be called when start gh3x sampling.
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
GH_WEAK void hal_gsensor_start_cache_data(void)
{
    GOODIX_PLATFROM_INT_GS_START_CACHE_ENTITY();
}

/**
 * @fn     void hal_gsensor_stop_cache_data(void)
 * 
 * @brief  Stop cache gsensor data for gh3x
 *
 * @attention   This function will be called when stop gh3x sampling.
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
GH_WEAK void hal_gsensor_stop_cache_data(void)
{
        
    GOODIX_PLATFROM_INT_GS_STOP_CACHE_ENTITY();
}

/**
 * @fn     void hal_gsensor_drv_get_fifo_data(GS16 gsensor_buffer[], GU16 *gsensor_buffer_index)
 * 
 * @brief  get gsensor fifo data
 *
 * @attention   When read fifo data of GH3X_, will call this function to get corresponding cached gsensor data.
 *
 * @param[in]   None
 * @param[out]  gsensor_buffer          pointer to gsensor data buffer
 * @param[out]  gsensor_buffer_index    pointer to number of gsensor data(every gsensor data include x,y,z axis data)
 *
 * @return  None
 */
GH_WEAK void hal_gsensor_drv_get_fifo_data(GS16 gsensor_buffer[], GU16 *gsensor_buffer_index)
{
/**************************** WARNNING  START***************************************************/
/*  (*gsensor_buffer_index) can not be allowed bigger than __GSENSOR_DATA_BUFFER_SIZE__  ****************/
/* Be care for copying data to gsensor_buffer, length of gsensor_buffer is __GSENSOR_DATA_BUFFER_SIZE__ *****/
/**************************** WARNNING END*****************************************************/
    GOODIX_PLATFROM_INT_GET_GS_DATA_ENTITY();
/**************************** WARNNING: DO NOT REMOVE OR MODIFY THIS CODE   ---START***************************************************/
    if((*gsensor_buffer_index) > (__GSENSOR_DATA_BUFFER_SIZE__))
    {
        while(1);   // Fatal error !!!
    }
/**************************** WARNNING: DO NOT REMOVE OR MODIFY THIS CODE   ---END***************************************************/
}

#if (__EXAMPLE_LOG_TYPE__)
/**
 * @fn     void GH3X_Log(char *logstring)
 * 
 * @brief  for debug version, log
 *
 * @attention   this function must define that use debug version lib
 *
 * @param[in]   logstring      pointer to log string
 * @param[out]  None
 *
 * @return  None
 */
extern esp_err_t gh_ESP_LOG(char *string);

#if __EXAMPLE_LOG_TYPE__ == __EXAMPLE_LOG_METHOD_0__
GH_WEAK void GH3X_Log(GCHAR *log_string)
{
    gh_ESP_LOG(log_string);
    // GOODIX_PLATFROM_LOG_ENTITY();
}
//void GH3X2X_Log(GCHAR *log_string)
//{
//    GOODIX_PLATFROM_LOG_ENTITY();
//}
#endif

#if __EXAMPLE_LOG_TYPE__ == __EXAMPLE_LOG_METHOD_1__
GH_WEAK void GH3X_RegisterPrintf(int (**pPrintfUser)(const char *format, ...))
{
    //(*pPrintfUser) = printf;   //use printf in <stdio.h>  or use equivalent function in your platform
    GOODIX_PLATFROM_PRINTF_ENTITY();
}
#endif
#endif

/**
 * @fn     void gh3x_bsp_delay_us(GU16 usUsec)
 * 
 * @brief  Delay in us,user should register this function into driver lib
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */

GH_WEAK void gh3x_bsp_delay_us(GU16 usUsec)
{
    esp_rom_delay_us(usUsec);
    // GOODIX_PLATFROM_DELAY_US_ENTITY();
}

/**
 * @fn     void gh3x_bsp_delay_ms(GU16 usMsec)
 * 
 * @brief  Delay in ms
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
GH_WEAK void gh3x_bsp_delay_ms(GU16 usMsec)
{
    esp_rom_delay_us(usMsec*1000);
    // GOODIX_PLATFROM_DELAY_MS_ENTITY();
}

#if (__GH_MULTI_SENSOR_EVENT_PRO_CONIG__)


GH_WEAK void GhCreateMultiSensorTimer(GU32 unPeriodMs)
{
    GOODIX_PLATFROM_CREAT_ADT_CONFIRM_ENTITY();
}



/**
 * @fn     void GhStartMultiSensorTimer(void)
 * 
 * @brief  Start time of adt confirm to get g sensor
 *
 * @attention   None        
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
GH_WEAK void GhStartMultiSensorTimer(void)
{
    EXAMPLE_LOG("GhStartMultiSensorTimer\r\n");
    GOODIX_PLATFROM_START_TIMER_ENTITY();
}


/**
 * @fn     void GhStopMultiSensorTimer(void)
 * 
 * @brief  Stop time of adt confirm
 *
 * @attention   None        
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
GH_WEAK void GhStopMultiSensorTimer(void)
{
    EXAMPLE_LOG("GhStopMultiSensorTimer\r\n");
    GOODIX_PLATFROM_STOP_TIMER_ENTITY();
}

GH_WEAK void GhDeleteMultiSensorTimer(void)
{
    GOODIX_PLATFORM_DEL_TIMER_ENTITY();
}


const STGhMultiSensorTimerOps g_stGhMultiSensorTimerOps =
{
    .TimerCreate = GhCreateMultiSensorTimer,
    .TimerStart = GhStartMultiSensorTimer,
    .TimerStop = GhStopMultiSensorTimer,
    .TimerDelete = GhDeleteMultiSensorTimer,
};
#endif

GH_WEAK int GH3X_HalSerialSendData(char* uchTxDataBuf, int usBufLen)
{
    return 0;
}

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
