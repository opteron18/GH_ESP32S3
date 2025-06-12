/**
 ****************************************************************************************
* @file    gh3x_protocol_inner.h
* @author  GHealth Driver Team
* @brief   
****************************************************************************************
* @attention  
#####Copyright (c) 2022 GOODIX
All rights reserved.

****************************************************************************************
*/

#ifndef _GH3X_PROTOCOL_INNER_H_
#define _GH3X_PROTOCOL_INNER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "gh3x_drv.h"
#include "gh3x_protocol.h"

/* protocol serial type macro */
#define GH3X_SERIAL_USE_UART                         (1)     /**< protocol communicate use UART */
#define GH3X_SERIAL_USE_BLE                          (2)     /**< protocol communicate use BLE */

#define GH_PROTOCOL_HEADER_AA                      (0xAA)
#define GH_PROTOCOL_VERSION_11                     (0x11)

#define GH_PROTOCOL_BLE_UART_47                    (0x47)
#define GH_PROTOCOL_BLE_UART_44                    (0x44)
#define GH_PROTOCOL_BLE_TAIL                       (0x0A)

#define RX_STREAM_STATE_WAIT_UART_HEADER_BYTE0       (0u)
#define RX_STREAM_STATE_WAIT_UART_HEADER_BYTE1       (1u)
#define RX_STREAM_STATE_WAIT_UART_HEADER_BYTE2       (2u)
#define RX_STREAM_STATE_WAIT_UART_LEN                (3u)
#define RX_STREAM_STATE_WAIT_UART_DATA               (4u)
#define RX_BYTE_STREAM_STATE_WAIT_CRC8               (5u)

#define GH_PROTOCOL_PAYLOAD_FIXED_INDEX            (0u)        /**< fixed header index */
#define GH_PROTOCOL_PAYLOAD_VER_INDEX              (1u)        /**< version index */
#define GH_PROTOCOL_AA11_HEADER_LEN                (2u)        /**< packet header len */
#define GH_PROTOCOL_DATA_LEN_LEN                   (1u)        /**< packet header len */
#define GH_PROTOCOL_DATA_RES_LEN                   (1u)        /**< packet header len */

/// universal protocol packet max len support
#define GH_PROTOCOL_PACK_INFO_HEADER_LEN           (4u)        /**< packet header len */
#define GH_PROTOCOL_PACK_APART_HEADER_LEN          (1u)        /**< packet header len */
#define GH_PROTOCOL_PACK_TAIL_LEN                  (1u)        /**< packet tail len */

/// packet header&tail len
#define GH_PROTOCOL_PACKET_HEADER_LEN              (GH_PROTOCOL_AA11_HEADER_LEN + \
                                                        GH_PROTOCOL_DATA_LEN_LEN + \
                                                        GH_PROTOCOL_PACK_INFO_HEADER_LEN + \
                                                        GH_PROTOCOL_PACK_APART_HEADER_LEN)

/// packet header&tail len
#define GH_PROTOCOL_PAYLOAD_HEADER_LEN             (GH_PROTOCOL_AA11_HEADER_LEN + \
                                                        GH_PROTOCOL_DATA_RES_LEN +\
                                                        GH_PROTOCOL_DATA_LEN_LEN)

/// universal protocol payload max len
#define GH_PROTOCOL_PAYLOAD_LEN_MAX                (GH_PROTOCOL_BUF_LEN - \
                                                        GH_PROTOCOL_PACKET_HEADER_LEN - \
                                                        GH_PROTOCOL_PACK_TAIL_LEN)

//com id
#define GH_PROTOCOL_COM_ID_0                       (0u)        /**< com id */
#define GH_PROTOCOL_COM_ID_1                       (1u)        /**< com id */
#define GH_PROTOCOL_COM_ID_2                       (2u)        /**< com id */
#define GH_PROTOCOL_COM_ID_3                       (3u)        /**< com id */
#define GH3X_COM_NUM                               (0x4)

/// crc8 clear val
#define  GH3X_CRC8_CLEAR_VAL                         (0xFF)

#define GH3X_RELIABLE_TIMEOUT_COUNT_MAX              (1000 / GH_PAYLOAD_SEND_TIME_THRD)

#define GH3X_RELIABLE_RESEND_COUNT_MAX               (5)

typedef enum
{
    GH_PACK_MODE_QUIRE,
    GH_PACK_MODE_RELIABLE,
    GH_PACK_MODE_UNRELIABLE,
} EMProSendPackMode;

typedef enum
{
    GH_PRO_TYPE_DOUBLE,
    GH_PRO_TYPE_UNSIGNED,
    GH_PRO_TYPE_SIGNED,
    GH_PRO_TYPE_STATUS
} EMProPackType;

typedef enum
{
    GH_APART_PROCESS = 0,
    GH_APART_START,
    GH_APART_FINISH,
} EMApartProSta;

#if 0
typedef enum
{
    UPROTOCOL_CMD_START = 0x00,             /**< start */
    UPROTOCOL_CMD_STOP,                     /**< stop */
    UPROTOCOL_CMD_REPUBLISH,
    UPROTOCOL_CMD_IGNORE = 0xFF,            /**< ignore */
} EMUprotocolParseCmdType;
#endif

typedef enum
{
    GH_CMD_RELIABLE_CONTINUE,
    GH_CMD_RELIABLE_WAIT_COMFIRM,
} EMCmdReliableSta;

typedef enum
{
    GH_PRO_HAL_SEND_FILLING,
    GH_PRO_HAL_SEND_PAYLOAD,
} EMProHalSendType;

typedef enum
{
    GH_STA_REGI,
    GH_STA_QUIRE,
    GH_STA_CONFIRM,
    GH_STA_FAIL,
} EMProPackStatus;

typedef struct
{
    GU8 ubSta : 1;
    GU8 ubCount : 7;
} STReliableSta;

typedef struct
{
    GU8 uchCmd;
    STReliableSta uchComSta;
    GU8 uchTimeoutCount;
    GU8 uchResendFlag;
    GU8 uchResendCount;
    GU16 usTBufRCnt;
    GU16 usTBufRCntRecord;
    GU16 usTBufWCnt;
    GU8* pTBuf;
} STReliableConfig;

typedef struct
{
    GU8 ubComid : 2;
    GU8 ubDatatype : 2;
    GU8 ubWidth : 3;
    GU8 ubApartFlag : 1;
} STProMeta;

typedef struct
{
    GU8 ubFlag : 2;
    GU8 ubPackNum : 3;
    GU8 ubPackCnt : 3;
} STApartMeta;

typedef struct
{
    STProMeta stProMeta;
    GU8 uchCmd;
    GU8 uchDataNum;
    STApartMeta stApartMeta;
    GU8* pSrc;
} STProPackInfo;

typedef struct
{
    GU8 uchHandleStatus;
    GU8 uchSerialMode;
    GU8 uchReliableCmdNum;
    GU8 usRecvHandleIndex;
    GU8 uchReliablePackExistFlag;
    GU8 uchSendPayloadBuffer[GH_PROTOCOL_BUF_LEN];
    GU8 uchRecvPayloadBuffer[GH_PROTOCOL_BUF_LEN];
    GU8 uchUnreliableFifo[GH_PROTOCOL_UNRELIABLE_FIFO_MAX_LEN];
    GU8 uchReliableTempBuffer[GH3X_RELIABLE_TEMP_BUFFER_NUM][GH_PROTOCOL_PAYLOAD_LEN_MAX];
    GU8 uchRXComIDComRecord[GH3X_COM_NUM];
    GU16 usSendPayloadIndex;
    GU16 usRecvPayloadIndex;
    GU16 usUnreliableFifoWriteCnt;
    GU16 usUnreliableFifoReadCnt;
    STReliableConfig stReliableCmdList[GH3X_RELIABLE_TEMP_BUFFER_NUM];
    GU8 uchTXComIDComRecord[GH3X_COM_NUM];
} STProtocolHandle;

extern STProtocolHandle gstProtocolHandle[];
extern pHalSerialSendDataFuncs gpHalSerialSendData[];
extern pCmdHandleFuncs gpCmdHandle[];

//gh3x_protocol_tx.c
extern GU8 GH3X_GetReliableCmdNum(GU8 uchHandleID);
extern void GH3X_ReliableConfigInit(GU8 uchHandleID, GU8 *puchReliableCmdList , GU8 uchReliableCmdNum);
extern GU8 GH3X_GetReliableCmd(GU8 uchHandleID, GU8 uchCnt);
extern GU8 GH3X_CheckCmdReliable(GU8 uchHandleID, GU8 uchCmd);
extern GU8 GH3X_GetCmdReliableFlag(GU8 uchHandleID, GU8 uchCmd);
extern void GH3X_SetCmdReliableFlag(GU8 uchHandleID, GU8 uchCmd, GU8 uchSta);
extern void GH3X_AddCmdReliableTimeOutCount(GU8 uchHandleID);
extern void GH3X_ProtocolTxModuleInit(GU8 uchHandleID);
extern GU8 GH3X_GetUnreliableFifoStatus(GU8 uchHandleID);
extern GU8 GH3X_GetReliableFifoStatus(GU8 uchHandleID, GU8 uchCmd);
extern void GH3X_OutputCirFifoByte(GU8* pSrc, GU16 usByteNum, GU16 *pusCirFifoReadCnt, GU8 *puchCirFifo,  GU16 usFifoMaxLen);
extern void GH3X_OutputUnreliableFifoByte(GU8 uchHandleID, GU8* pSrc, GU16 usByteNum);
extern void GH3X_OutputReliableFifoByte(GU8 uchHandleID, GU8* pSrc, GU16 usByteNum, GU8 uchCmd);
extern EMProHalSendType GH3X_GetReliablePack(GU8 uchHandleID, STProPackInfo *pstPackMeta, GU8 uchCmd);
extern EMProHalSendType GH3X_GetUnreliablePack(GU8 uchHandleID, STProPackInfo *pstPackMeta);
extern EMProHalSendType GH3X_FillPackInPayloadAndSend(GU8 uchHandleID, STProPackInfo *pstPackMeta, EMProHalSendType emSta);
extern EMProtocolCode GH3X_PackPublishHandle(GU8 uchHandleID, GU8 uchByteNum, GU8 uchDataNum, STProPackInfo *pstPackMeta);
extern EMProtocolCode GH3X_CheckReliableFifoStatus(GU8 uchHandleID);
extern EMProtocolCode GH3X_CheckUnreliableFifoStatus(GU8 uchHandleID);

//gh3x_protocol_rx.c
extern void GH3X_ProtocolRxModuleInit(GU8 uchHandleID);
extern EMUprotocolParseCmdType GH3X_ProtocolRecvPayloadHandle(GU8 uchHandleID);
extern void GH3X_SetRxComidCmd(GU8 uchHandleID, GU8 uchComid, GU8 uchCmd);
extern GU8 GH3X_CheckRxComidCmd(GU8 uchHandleID, GU8 uchComid, GU8 uchCmd);
extern void GH3X_SetTxComidCmd(GU8 uchHandleID, GU8 uchComid, GU8 uchCmd);
extern GU8 GH3X_CheckTxComidCmd(GU8 uchHandleID, GU8 uchComid, GU8 uchCmd);
extern GU8 GH3X_GetRxComidCmd(GU8 uchHandleID, GU8 uchComid);
extern void GH3X_ProtocolHandleRecvData(GU8 uchHandleID, GU8 uchRecvByte);

//gh3x_protocol_handle.c
extern GU16 GH3X_CalcPackByteNum(GU8 ubWidth);
extern GU8 GH3X_CalcPackWidth(GU16 uchByteNum);
extern GU8  GH3X_CalcArrayCrc8Val(GU8 uchDataArr[], GU16 usDataIndex, GU16 usDataLen);
extern void GH3X_GenStatusPack(GU8 uchHandleID, STProPackInfo *pstPackMeta, GU8 uchComid, GU8 uchCmd, EMProPackStatus emSta);
extern EMUprotocolParseCmdType GH3X_ProtocolStatusHandle(GU8 uchHandleID, STProPackInfo *pstPack);

#ifdef __cplusplus
}
#endif

#endif /* _GH3X_DRV_DUMP_H_ */

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
