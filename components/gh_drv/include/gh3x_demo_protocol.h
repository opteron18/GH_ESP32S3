#ifndef _GH3X_DEMO_PROTOCOL_H_
#define _GH3X_DEMO_PROTOCOL_H_

#include "gh3x_drv.h"

typedef struct
{
    GU8 ubGh3xDataEn : 1;
    GU8 ubGsDataEn : 1;
    GU8 ubFlagInfo : 1;
    GU8 ubAlgoData : 1;
    GU8 ubAgcInfo : 1;
    GU8 ubFrameId : 1;
    GU8 ubFuncID : 1;
    GU8 reserved[3];
} __attribute__((packed)) STRawdataPackHeader;

typedef struct
{
    STRawdataPackHeader packHeader;
    GS32 function_id;
    GS32 Frame_id;
    GS32 *punAgcdata;
    GS32 unAgcdataSize;
    GS32 *punAlgodata;
    GS32 unAlgodataBits;
    GS32 *punFlagdata;
    GS32 unFlagdataBits;
    GS32 *punGsdata;
    GS32 unGsdataSize;
    GS32 *punRawdata;
    GS32 unRawdataSize;
} STRawdataFrameInfo;

typedef struct
{
    GS32 *punDiffRawData;
    GS32 *punLastRawData;
    GU8 uchStartFlag;
    GU8 uchEndFlag;
    GU8 *puchBytesBuffer;
    GU32 unBytesBufferLen;
    GU32 unBytesBufferUsed;
} STRawdataFrameBytesInfo;

/**
 * @fn     int GH3X_HalSerialSendData(GU8* uchTxDataBuf, GU16 usBufLen)
 *
 * @brief  Serial send data
 *
 * @attention   None
 *
 * @param[in]   uchTxDataBuf        pointer to data buffer to be transmitted
 * @param[in]   usBufLen            data buffer length
 * @param[out]  None
 *
 * @return  None
 */
int GH3X_HalSerialSendData(char* uchTxDataBuf, int usBufLen);

/**
 * @fn     void GH3X_ProtocolProcess(GU8* uchTxDataBuf, GU16 usBufLen)
 *
 * @brief  Serial recv data
 *
 * @attention   None
 *
 * @param[in]   uchTxDataBuf        pointer to data buffer to be transmitted
 * @param[in]   usBufLen            data buffer length
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_ProtocolProcess(GU8* uchTxDataBuf, GU16 usBufLen);

/**
 * @fn     void GH3X_SerialSendTimerStop(void)
 *
 * @brief  Serial send timer stop
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_SerialSendTimerStop(void);
/**
 * @fn     void GH3X_SerialSendTimerStart(void)
 *
 * @brief  Serial send timer start
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_SerialSendTimerStart(void);

/**
 * @fn     void GH3X_SerialSendTimerHandle(void)
 *
 * @brief  Serial send timer handle
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_SerialSendTimerHandle(void);

/**
 * @fn     void GH3X_SendProtocolPack((GU8 uchHandleID,
 *                          GU8 uchCmd,
 *                          EMPackType emDataType,
 *                          GU8 uchByteNum,
 *                          GU8 uchDataNum,
 *                          void *pSrc)
 *
 * @brief  Copy protocol data to buffer that will be sent out to APP/EVK.
 *
 * @attention   None
 *
 * @param[in]   puchProtocolDataBuffer      point to protocol data
 * @param[in]   usProtocolDataLen           protocol data length
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_SendProtocolPack(GU8 uchHandleID,
                        GU8 uchCmd,
                        GU8 emDataType,
                        GU8 uchByteNum,
                        GU8 uchDataNum,
                        void *pSrc);
/**
 * @fn     void GH3X_SendProtocolPack((GU8 uchHandleID,
 *                          GU8 uchCmd,
 *                          EMPackType emDataType,
 *                          GU8 uchByteNum,
 *                          GU8 uchDataNum,
 *                          void *pSrc)
 *
 * @brief  Copy protocol data to buffer that will be sent out to APP/EVK.
 *
 * @attention   None
 *
 * @param[in]   puchProtocolDataBuffer      point to protocol data
 * @param[in]   usProtocolDataLen           protocol data length
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_SendProtocolPackWithoutSignal(GU8 uchHandleID,
                        GU8 uchCmd,
                        GU8 emDataType,
                        GU8 uchByteNum,
                        GU8 uchDataNum,
                        void *pSrc);

/**
 * @fn     void GH3X_SerialSendTimerHandleWithoutSignal(void)
 *
 * @brief  Serial send timer handle
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_SerialSendTimerHandleWithoutSignal(void);

/**
 * @fn      void GH3X_SerialSendTimerInit(GU8 uchPeriodMs)
 *
 * @brief  GH3X_SerialSendTimerInit
 *
 * @attention   None
 *
 * @param[in]   uchPeriodMs    timer period (ms)
 * @param[out]  None
 *
 * @return  None
 */
void GH3X_SerialSendTimerInit(GU8 uchPeriodMs);

void GH3X_UprotocolSendFrameDataBytes(const STFrameInfo *const pstFrameInfo);
void GH3X_UprotocolSendFrameData(const STFrameInfo *const pstFrameInfo);
void GH3X_UprotocolSendFifoData(GU8 *puchReadRawdataBuffer, GU16 usReadRawdataLen, GU32 unFifoFull);
void GH3X_UprotocolSendChipEvent(GU32 unEvent);
GU32 GH3X_GetProtocolTargetFunction(void);
void GH3X_EventBufWrite(GU32 unEvent);

#endif
