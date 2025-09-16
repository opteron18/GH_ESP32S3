/**
  ****************************************************************************************
  * @file    gh3x_protocol_cmd.h
  * @author  GHealth Driver Team
  * @brief   
  ****************************************************************************************
  * @attention  
  #####Copyright (c) 2022 GOODIX
   All rights reserved.
  
  ****************************************************************************************
  */

#ifndef _GH3X_PROTOCOL_CMD_H_
#define _GH3X_PROTOCOL_CMD_H_

#include "gh3x_drv.h"

/* macro of reg rw cmd */
#define  UPROTOCOL_REG_RW_READ_MODE                     (0x00)      /**< read mode */
#define  UPROTOCOL_REG_RW_WRITE_MODE                    (0x01)      /**< write mode */
#define  UPROTOCOL_REG_RW_MODE_INDEX                    (0u)        /**< mode */
#define  UPROTOCOL_REG_RW_CNT_INDEX                     (1u)        /**< count */
#define  UPROTOCOL_REG_RW_ADDR_H_INDEX                  (2u)        /**< addr_h */
#define  UPROTOCOL_REG_RW_ADDR_L_INDEX                  (3u)        /**< addr_l */
#define  UPROTOCOL_REG_RW_DATA_INDEX                    (4u)        /**< data */
#define  UPROTOCOL_REG_RW_HEADER_LENGTH                 (4u)        /**< header len */


/* macro of get version cmd */
#define  UPROTOCOL_GET_VER_TYPE_INDEX                   (0u)        /**< type index */
#define  UPROTOCOL_GET_VER_STRING_LEN_INDEX             (1u)        /**< version string len index */
#define  UPROTOCOL_GET_VER_STRING_INDEX                 (2u)        /**< version string index */
#define  UPROTOCOL_GET_VER_DEFAULT_VER_STRING           "no_ver"    /**< default version string */
#define  UPROTOCOL_GET_VER_CACHE_VER_MAX_LEN            (64)        /**< cache algo ver string, max len */
#define  UPROTOCOL_GET_VER_CACHE_VER_PAYLOAD_INDEX      (GH3X_UPROTOCOL_PAYLOAD_LEN_MAX - \
                                                         UPROTOCOL_GET_VER_CACHE_VER_MAX_LEN) /**< cache index */

/* macro of get max len */
#define  UPROTOCOL_GET_MAX_LEN_RESP_VAL_INDEX            (0u)      /**< len index */
#define  UPROTOCOL_GET_MAX_LEN_RESP_LEN                  (1u)      /**< respond len */

/* macro of get max len */
#define  UPROTOCOL_GET_MAX_LEN_RESP_VAL_INDEX            (0u)      /**< len index */
#define  UPROTOCOL_GET_MAX_LEN_RESP_LEN                  (1u)      /**< respond len */


/* macro of load reg list */
#define  UPROTOCOL_LOAD_REG_LIST_SIZEOF                 (4u)      /**< sizeof reg */
#define  UPROTOCOL_LOAD_REG_LIST_RESP_STATUS_INDEX      (0u)      /**< respond status inex */
#define  UPROTOCOL_LOAD_REG_LIST_RESP_LEN               (1u)      /**< respond len */
#define  UPROTOCOL_LOAD_REG_LIST_RESP_CFG_SUCCESS       (0u)      /**< config success */
#define  UPROTOCOL_LOAD_REG_LIST_RESP_CFG_FAIL          (1u)      /**< config fail */


/* macro of fifo thr config */
#define  UPROTOCOL_FIFO_THR_CFG_H_INDEX                 (0u)      /**< config val high byte inex */
#define  UPROTOCOL_FIFO_THR_CFG_L_INDEX                 (1u)      /**< config val low byte inex */
#define  UPROTOCOL_FIFO_THR_CFG_RESP_STATUS_INDEX       (0u)      /**< respond status inex */
#define  UPROTOCOL_FIFO_THR_CFG_RESP_LEN                (1u)      /**< respond len */
#define  UPROTOCOL_FIFO_THR_CFG_RESP_CFG_SUCCESS        (0u)      /**< config success */
#define  UPROTOCOL_FIFO_THR_CFG_RESP_CFG_FAIL           (1u)      /**< config fail */

/* macro of report event set */
#define  UPROTOCOL_REPORT_EVENT_SET_INDEX                (0u)      /**< report event set inex */
#define  UPROTOCOL_REPORT_EVENT_SET_RESP_LEN             (1u)      /**< respond len */
#define  UPROTOCOL_REPORT_EVENT_SET_RESP_INDEX           (0u)      /**< report event set respond inex */
#define  UPROTOCOL_REPORT_EVENT_SET_SUCCESS              (0u)      /**< set success */
#define  UPROTOCOL_REPORT_EVENT_SET_FAIL                 (1u)      /**< set fail */
#define  UPROTOCOL_REPORT_EVENT_ID_INIT                  (0xFF)    /**< set fail */

/* macro of chip ctrl */
#define  UPROTOCOL_CHIP_CTRL_VAL_INDEX                  (0u)      /**< ctrl val inex */
#define  UPROTOCOL_CHIP_CTRL_RESP_STATUS_INDEX          (0u)      /**< respond status inex */
#define  UPROTOCOL_CHIP_CTRL_RESP_LEN                   (1u)      /**< respond len */
#define  UPROTOCOL_CHIP_CTRL_RESP_CFG_SUCCESS           (0u)      /**< config success */
#define  UPROTOCOL_CHIP_CTRL_RESP_CFG_FAIL              (1u)      /**< config fail */
#define  UPROTOCOL_CHIP_CTRL_RESP_NULL                  (0xFF)    /**< function ptr null */
#define  UPROTOCOL_CHIP_CTRL_TYPE_HARD_RESET            (0x5A)    /**< hard reset type */
#define  UPROTOCOL_CHIP_CTRL_TYPE_SOFT_RESET            (0xC2)    /**< soft reset type */
#define  UPROTOCOL_CHIP_CTRL_TYPE_WAKEUP                (0xC3)    /**< wakeup type */
#define  UPROTOCOL_CHIP_CTRL_TYPE_SLEEP                 (0xC4)    /**< sleep type */

/* macro of chip event report */
#define  UPROTOCOL_CHIP_EVENT_REPORT_ACK_INDEX           (0u)      /**< event report ack inex */
#define  UPROTOCOL_CHIP_EVENT_REPORT_INDEX               (0u)      /**< event report inex */
#define  UPROTOCOL_CHIP_EVENT_REPORT_ID_INDEX            (2u)      /**< event report id inex */
#define  UPROTOCOL_CHIP_EVENT_REPORT_LEN                 (3u)      /**< event report data length */

/* macro of debug status */
#define  UPROTOCOL_DEBUG_STATUS_TYPE_INDEX              (0u)      /**< type inex */
#define  UPROTOCOL_DEBUG_STATUS_RESP_BYTES_INDEX        (1u)      /**< respond bytes len inex */
#define  UPROTOCOL_DEBUG_STATUS_RESP_DATA_INDEX         (2u)      /**< respond data inex */
#define  UPROTOCOL_DEBUG_STATUS_RESP_LEN                (6u)      /**< respond len */
#define  UPROTOCOL_DEBUG_STATUS_RESP_BYTES_8            (1u)      /**< respond bytes len 8 bits */
#define  UPROTOCOL_DEBUG_STATUS_RESP_BYTES_16           (2u)      /**< respond bytes len 16 bits */
#define  UPROTOCOL_DEBUG_STATUS_RESP_BYTES_32           (4u)      /**< respond bytes len 32 bits */
#define  UPROTOCOL_DEBUG_STATUS_READ_INT_PIN            (0x00)    /**< read int pin type */
#define  UPROTOCOL_DEBUG_STATUS_READ_RST_PIN            (0x01)    /**< read reset pin type */
#define  UPROTOCOL_DEBUG_STATUS_READ_SPCS_PIN           (0x02)    /**< read spcs pin type */
#define  UPROTOCOL_DEBUG_STATUS_READ_SPDO_PIN           (0x03)    /**< read spdo pin type */
#define  UPROTOCOL_DEBUG_STATUS_COMM_CONFIRM            (0x04)    /**< communicate confirm type */
#define  UPROTOCOL_DEBUG_STATUS_READ_WORK_STATUS        (0x10)    /**< read work status type */

/* macro of get version cmd */
#define  UPROTOCOL_GET_VER_TYPE_FW_VER                  (0x01)      /**< get firmware version type */
#define  UPROTOCOL_GET_VER_TYPE_VIRTUAL_REG_VER         (0x03)      /**< get virtual reg version type */
#define  UPROTOCOL_GET_VER_TYPE_BOOTLOADER_VER          (0x04)      /**< get BOOTLOADER version type */
#define  UPROTOCOL_GET_VER_TYPE_PROTOCOL_VER            (0x05)      /**< get protocol version type */
#define  UPROTOCOL_GET_VER_TYPE_FUNC_SUPPORT            (0x06)      /**< get driver func support type */
#define  UPROTOCOL_GET_VER_TYPE_DRV_VER                 (0x07)      /**< get driver version type */
#define  UPROTOCOL_GET_VER_TYPE_CHIP_VER                (0x08)      /**< get chip version type */
#define  UPROTOCOL_GET_VER_TYPE_BLE_VER                 (0x09)      /**< get ble version type */
#define  UPROTOCOL_GET_VER_TYPE_DEMO_VER                (0x0A)      /**< get algo call demo version type */

#define  UPROTOCOL_GET_VER_TYPE_ALGO_VER                (0x20)      /**< algo version type */
#define  UPROTOCOL_GET_VER_TYPE_ALGO_VER_END            (UPROTOCOL_GET_VER_TYPE_ALGO_VER + GH3X_FUNC_OFFSET_MAX)      /**< get algorithm version type END*/

#endif /* _GH3X_DRV_DUMP_H_ */

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
