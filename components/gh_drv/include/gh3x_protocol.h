/**
 ****************************************************************************************
* @file    gh3x_protocol.h
* @author  GHealth Driver Team
* @brief   
****************************************************************************************
* @attention  
#####Copyright (c) 2022 GOODIX
All rights reserved.

****************************************************************************************
*/


#ifndef _GH3X_PROTOCOL_H_
#define _GH3X_PROTOCOL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "gh3x_drv.h"

#define PRO_LOG_DEBUG                                   (0)

//basic config
#define GH_PROTOCOL_BUF_LEN                             (220)

//unreliable fifo max len
#define GH_PROTOCOL_UNRELIABLE_FIFO_MAX_LEN             (GH_PROTOCOL_BUF_LEN * 50)

/// reliable temp buffer num
#define  GH3X_RELIABLE_TEMP_BUFFER_NUM                  (0x0A)

//send timeout repeat
#define GH_PAYLOAD_SEND_TIME_THRD                       (10)
#define GH_PAYLOAD_SEND_TIMEOUT_SECONDS                 (5)

//handle id
#define GH_PROTOCOL_HANDLE_ID_0                         (0)
#define GH_PROTOCOL_HANDLE_ID_1                         (1)
#define GH_PROTOCOL_HANDLE_NUM                          (1)

//width set define
#define GH_WIDTH_8BIT                                   (3)
#define GH_WIDTH_16BIT                                  (4)
#define GH_WIDTH_32BIT                                  (5)
#define GH_WIDTH_64BIT                                  (6)

/* cmd id */
#define GH_PROTOCOL_CMD_NOP                             (0x00)     /**< nop operation */
#define GH_PROTOCOL_CMD_PROTOCOL_INIT                   (0x01)     /**< protocol init */
#define GH_PROTOCOL_CMD_TEST_CONNECT                    (0X02)     /**< test connect */
#define GH_PROTOCOL_CMD_REG_READ                        (0x03)     /**< register RW */
#define GH_PROTOCOL_CMD_REG_WRITE                       (0x04)     /**< register RW */
#define GH_PROTOCOL_CMD_REG_BIT_WRITE                   (0x05)     /**< register RW */
#define GH_PROTOCOL_CMD_REG_PLL_READ                    (0x06)     /**< register RW */
#define GH_PROTOCOL_CMD_CHNL_MAP                        (0x09)     /**< chnlmap */
#define GH_PROTOCOL_CMD_INFO_DATA                       (0x0A)     /**< info data */
#define GH_PROTOCOL_CMD_FRAME_DATA                      (0x0B)     /**< info data */
#define GH_PROTOCOL_CMD_START_CTRL                      (0x0C)     /**< start ctrl */
#define GH_PROTOCOL_CMD_BATTERY_CUR                     (0x0D)     /**< battery current */
#define GH_PROTOCOL_CMD_FW_UPDATE                       (0x0F)     /**< fw update */
#define GH_PROTOCOL_CMD_WORK_MODE                       (0x10)     /**< work mode */
#define GH_PROTOCOL_CMD_CHIP_EVENT_REPORT               (0x16)     /**< int event report */
#define GH_PROTOCOL_CMD_CHIP_CTRL                       (0x17)     /**< chip ctrl */
#define GH_PROTOCOL_CMD_GET_VER                         (0x19)     /**< get ver */
#define GH_PROTOCOL_CMD_CHIP_CONN_STA                   (0x1A)     /**< get chip connect status */
#define GH_PROTOCOL_CMD_SWITCH_SLOT_FUNC                (0x1C)     /**< switch on or off some function */
#define GH_PROTOCOL_CMD_HSM_RESULT_UPDATE               (0x29)     /**< switch on or off some function */
#define GH_UPROTOCOL_CMD_RAWDATA_FIFO_UPDATE            (0x2A)     /**< rawdata fifo update function */

#define FRAME_TYPE_OFFSET_INFO                            (0x0)      /**< frame info */
#define FRAME_TYPE_OFFSET_GSENSOR                         (0x1)      /**< frame data */
#define FRAME_TYPE_OFFSET_FLAG                            (0x2)      /**< frame data */
#define FRAME_TYPE_OFFSET_RAWDATA                         (0x3)      /**< frame data */
#define FRAME_TYPE_OFFSET_AGCINFO                         (0x4)      /**< frame data */
#define FRAME_TYPE_OFFSET_ALGOREAULTS                     (0x5)      /**< frame data */


#define GH_PROTOCOL_CMD_LOAD_REG_LIST                   (0xA1)     /**< get load reg list */

#define PACK_ID_OFFSET_GSENSOR                          (0)
#define PACK_ID_OFFSET_FLAG                             (1)
#define PACK_ID_OFFSET_RAWDATA                          (2)
#define PACK_ID_OFFSET_AGCINFO                          (3)
#define PACK_ID_OFFSET_ALGORESULT                       (4)

typedef enum
{
    GH_PRO_HAL_SUCCESS,
    GH_PRO_HAL_FAILED,
    GH_PRO_HAL_FIFO_EMPTY,
    GH_PRO_HAL_ANALYZING,
    GH_PRO_HAL_RELIABLE_ACK,
    GH_PRO_HAL_QUIRE_COMFIRM,
    GH_PRO_HAL_QUIRE_FAIL,
    GH_PRO_HAL_PAYLOAD_ERR,
} EMProtocolCode;

typedef enum
{
    GH_TYPE_DOUBLE,
    GH_TYPE_UNSIGNED,
    GH_TYPE_SIGNED,
    GH_TYPE_STATUS,
} EMPackType;

typedef int (*pHalSerialSendDataFuncs)(char* uchTxDataBuf, int usBufLen);
typedef void (*pCmdHandleFuncs)(GU8 uchHandleID, GU8 uchCmd, GU8 emDataType, GU8 ubWidth, GU8 uchDataNum, void* pSrc);

EMProtocolCode GH3X_ProtocolInit(GU8 uchHandleID, 
                                int (*pHalSerialSendDataHookFuncs)(char* pchTxDataBuf,
                                                                   int nBufLen),
                                void (*pCmdHandleFuncs)(GU8 uchHandleID,
                                                        GU8 uchCmd,
                                                        GU8 emDataType,
                                                        GU8 ubWidth,
                                                        GU8 uchDataNum,
                                                        void* pSrc));

EMProtocolCode GH3X_ProtocolPublishPack(GU8 uchHandleID,
                                        GU8 ubComid,
                                        GU8 uchCmd,
                                        GU8 emDataType,
                                        GU8 ubWidth,
                                        GU8 uchDataNum,
                                        void *pSrc);

EMProtocolCode GH3X_ProtocolGetPack(GU8 uchHandleID);

EMProtocolCode GH3X_ProtocolPayloadReply(GU8 uchHandleID,
                                         char* pchPayload,
                                         int nPayloadLen);

#ifdef __cplusplus
}
#endif

#endif /* _GH3X_DRV_DUMP_H_ */

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
