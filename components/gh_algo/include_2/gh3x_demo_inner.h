/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x_demo_inner.h
 *
 * @brief   gh3x driver lib demo inner header file
 *
 * @author  Gooidx Iot Team
 *
 */

#ifndef _GH3X_DEMO_INNER_H_
#define _GH3X_DEMO_INNER_H_
#include "gh3x_demo_config.h"

#include "esp_err.h"

// extern void spi_driver_init();
// extern esp_err_t spi_cs_set_level(uint32_t level);
// extern esp_err_t gh_ESP_LOG(char *string);



#ifdef GOODIX_DEMO_PLATFORM
#define GH_WEAK __weak
#include "gh3x_demo_goodix_platform.h"
#else
#ifdef GH3X_BLANK
#define GH_WEAK __weak
#else
#define GH_WEAK
#endif
#define GOODIX_PLATFROM_SAMPLING_START_HOOK_ENTITY()
#define GOODIX_PLATFROM_SAMPLING_STOP_HOOK_ENTITY()
#define GOODIX_PLATFROM_SPI_WRITE_ENTITY(write_buffer, length)     
#define GOODIX_PLATFROM_PROTOCOL_ANALYZE()
#define GOODIX_PLATFROM_REPORT_EVRENT_ENTITY()
#define GOODIX_PLATFROM_SPI_READ_ENTITY()                          
#define GOODIX_PLATFROM_SPI_CS_CTRL_ENTITY()                        
#define GOODIX_PLATFROM_SPI_RESET_PIN_ENTITY()
#define GOODIX_PLATFROM_INT_HANDLER_CALL_BACK_ENTITY()
#define GOODIX_PLATFROM_INT_GS_START_CACHE_ENTITY()
#define GOODIX_PLATFROM_INT_GS_STOP_CACHE_ENTITY()
#define GOODIX_PLATFROM_INT_GET_GS_DATA_ENTITY()
#define GOODIX_PLATFROM_LOG_ENTITY()                                
#define GOODIX_PLATFROM_DELAY_US_ENTITY()
#define GOODIX_PLATFROM_DELAY_MS_ENTITY()
#define GOODIX_PLATFROM_DELAY_MS_ENTITY()
#define GOODIX_PLATFROM_SERIAL_SEND_ENTITY()
#define GOODIX_PLATFROM_INT_INIT_ENTITY()
#define GOODIX_PLATFROM_RESET_PIN_INIT_ENTITY()
#define GOODIX_PLATFROM_SPI_INIT_ENTITY() 
#define GOODIX_PLATFROM_CREAT_ADT_CONFIRM_ENTITY()
#define GOODIX_PLATFROM_START_TIMER_ENTITY()
#define GOODIX_PLATFROM_STOP_TIMER_ENTITY()
#define GOODIX_PLATFORM_DEL_TIMER_ENTITY()
#define GOODIX_PLATFROM_WEAR_ON_EVENT()
#define GOODIX_PLATFROM_WEAR_OFF_EVENT()
#define GOODIX_PLATFROM_LEAD_ON_EVENT()
#define GOODIX_PLATFROM_LEAD_OFF_EVENT()
#define GOODIX_PLATFROM_HR_RESULT_REPORT_ENTITY()
#define GOODIX_PLATFROM_SPO2_RESULT_REPORT_ENTITY()
#define GOODIX_PLATFROM_SERIAL_TIMER_INIT_ENTITY()
#define GOODIX_PLATFROM_SERIAL_TIMER_STOP_ENTITY()
#define GOODIX_PLATFROM_SERIAL_TIMER_START_ENTITY()
#define GOODIX_PLATFROM_I2C_WRITE_ENTITY(device_id, write_buffer,length)
#define GOODIX_PLATFROM_I2C_INIT_ENTITY()
#define GOODIX_PLATFROM_I2C_READ_ENTITY(device_id, write_buffer, write_length, read_buffer, read_length)
#define GOODIX_PLATFROM_INT_CAP_START_CACHE_ENTITY()         
#define GOODIX_PLATFROM_INT_CAP_STOP_CACHE_ENTITY()         
#define GOODIX_PLATFROM_INT_GET_CAP_DATA_ENTITY()           
#define GOODIX_PLATFROM_INT_TEMP_START_CACHE_ENTITY()       
#define GOODIX_PLATFROM_INT_TEMP_STOP_CACHE_ENTITY()         
#define GOODIX_PLATFROM_INT_GET_TEMP_DATA_ENTITY()          
#define GOODIX_PLATFROM_WRITE_CAP_TO_FLASH_ENTITY()
#define GOODIX_PLATFROM_READ_CAP_FROM_FLASH_ENTITY()
#define GOODIX_PLATFROM_BEFORE_WAKE_UP_HOOK_ENTITY()          
#define GOODIX_PLATFROM_WAKE_UP_HOOK_ENTITY()                
#define GOODIX_PLATFROM_PT_CONFIG_HOOK_ENTITY()
#define GOODIX_PLATFROM_START_WITH_CONFIRM_HOOK_ENTITY()
#define GOODIX_PLATFROM_PRESSURE_PARAS_READ_ENTITY()
#define GOODIX_PLATFROM_PRESSURE_PARAS_WRITE_ENTITY()
#define GOODIX_PLATFROM_PRINTF_ENTITY()
#define GOODIX_PLATFROM_MALLOC_USER_ENTITY()
#define GOODIX_PLATFROM_FREE_USER_ENTITY()
#define GOODIX_PLATFROM_NADT_RESULT_HANDLE_ENTITY()
#define GOODIX_PLATFROM_GOODIX_ALGO_CALL_ENTITY()
#define GOODIX_PLATFROM_TIMEOUT_HOOK_ENTITY()
#define GOODIX_PLATFROM_TIMEOUT_HOOK_ENTITY()
#define GOODIX_PLATFROM_ECG_RESULT_REPORT_ENTITY()
#define GOODIX_PLATFROM_HR_RESULT_REPORT_ENTITY()
#define GOODIX_PLATFROM_SPO2_RESULT_REPORT_ENTITY()
#define GOODIX_PLATFROM_HRV_RESULT_REPORT_ENTITY()

#endif

#if defined ( __GNUC__ ) && !defined (__CC_ARM) /* GNU Compiler */
#ifndef __weak
    #define   __attribute__((weak))
  #endif /* */
#ifndef __packed
    #define __packed __attribute__((__packed__))
  #endif /* __packed */
#endif /* __GNUC__ */

/************************************ extern ************************************/

extern const STFrameInfo * const gpstFrameInfo[GH3X_FUNC_OFFSET_MAX];
extern const STInitConfig gstCfgListArr[__GH3X_CFG_LIST_MAX_NUM__];

/************************************* user *************************************/

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
void hal_gh3x_i2c_init(void);

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
GU8 hal_gh3x_i2c_write(GU8 device_id, const GU8 write_buffer[], GU16 length);

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
GU8 hal_gh3x_i2c_read(GU8 device_id, const GU8 write_buffer[], GU16 write_length, GU8 read_buffer[], GU16 read_length);

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

void hal_gh3x_spi_init(void);

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
GU8 hal_gh3x_spi_write(GU8 write_buffer[], GU16 length);

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
GU8 hal_gh3x_spi_read(GU8 read_buffer[], GU16 length);

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
GU8 hal_gh3x_spi_write_F1_and_read(GU8 read_buffer[], GU16 length);
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
void hal_gh3x_spi_cs_ctrl(GU8 cs_pin_level);
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



void hal_gh3x_reset_pin_init(void);

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

void hal_gh3x_reset_pin_ctrl(GU8 pin_level);
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
void hal_gh3x_int_init(void);

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
void hal_gh3x_int_handler_call_back(void);
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
void hal_gsensor_start_cache_data(void);

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
void hal_gsensor_stop_cache_data(void);

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
void hal_gsensor_drv_get_fifo_data(GS16 gsensor_buffer[], GU16 *gsensor_buffer_index);

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

#if __EXAMPLE_LOG_TYPE__ == __EXAMPLE_LOG_METHOD_0__
void GH3X_Log(GCHAR *logstring);
#endif

#if __EXAMPLE_LOG_TYPE__ == __EXAMPLE_LOG_METHOD_1__
void GH3X_RegisterPrintf(int (**pPrintfUser)(const char *format, ...));
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

void gh3x_bsp_delay_us(GU16 usUsec);

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
void gh3x_bsp_delay_ms(GU16 usMsec);
 
void* GH3X_MallocUser(GU32 unMemSize);

void GH3X_FreeUser(void* MemAddr);


/************************************* hook *************************************/

/**
 * @fn     void gh3x_init_hook_func(void)
 * 
 * @brief  gh3x init hook
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void gh3x_init_hook_func(void);

/**
 * @fn     void gh3x_samplingstart_hook_func(void)
 * 
 * @brief  gh3x start hook
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void gh3x_samplingstart_hook_func(void);

/**
 * @fn     void gh3x_samplingstop_hook_func(void)
 * 
 * @brief  gh3x stop hook
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void gh3x_samplingstop_hook_func(void);

/**
 * @fn     void gh3x_get_rawdata_hook_func(GU8 *read_buffer_ptr, GU16 length)
 * 
 * @brief  gh3x get rawdata hook
 *
 * @attention   None
 *
 * @param[in]   read_buffer_ptr     pointer to read rawdata buffer
 * @param[in]   length              length
 * @param[out]  None
 *
 * @return  None
 */

void gh3x_get_rawdata_hook_func(GU8 *read_buffer_ptr, GU16 length);

/**
 * @fn      void gh3x_algorithm_get_io_data_hook_func(const STFrameInfo * const pstFrameInfo)
 * 
 * @brief  get algorithm input and output data
 *
 * @attention   None        
 *
 * @param[in]   pstFrameInfo
 * @param[out]  None
 *
 * @return  None
 */
void gh3x_algorithm_get_io_data_hook_func(const STFrameInfo * const pstFrameInfo);

#if (__FUNC_TYPE_ECG_ENABLE__)
/**
 * @fn     void GH3X_LeadEventHook(GU32* punGotEvent)
 * 
 * @brief  Lead event hook
 *
 * @attention   None        
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_LeadEventHook(GU32* punGotEvent);
#endif


#if (__SUPPORT_HARD_ADT_CONFIG__)
/**
 * @fn     extern void GH3X_WearEventHook(GU32* punGotEvent, GU8* puchExentEx);
 * 
 * @brief  Wear event hook
 *
 * @attention   None        
 *
 * @param[in]   wear event
 * @param[in]   wear off type  0: no object  1: nonliving object    wear on type   0: object     1: living object
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_WearEventHook(GU32* punGotEvent, GU8* puchExentEx);
#endif

/**
 * @fn     void GH3X_DemoSamplingControl(EMUprotocolParseCmdType emSwitch)
 *
 * @brief  Start/stop sampling of gh3x and sensor
 *
 * @attention   None
 *
 * @param[in]   unFuncMode      function that will be started or stopped
 * @param[in]   emSwitch        stop/start sampling
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_DemoSamplingControl(GU32 unFuncMode, EMUprotocolParseCmdType emSwitch);
void GH3X_SamplingControlHook(GU32 unFuncMode, EMUprotocolParseCmdType emSwitch);
void GH3X_IntTimeOutHook(void);
void GH3X_CreateAdtConfirmTimer(void);
#endif

extern void GH3X_MoveDetectTimerHandler(GU32 unTimeIntervalMs);

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
