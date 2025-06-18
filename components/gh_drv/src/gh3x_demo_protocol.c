/**
 ****************************************************************************************
* @file    gh3x_demo_protocol.c
* @author  GHealth Driver Team
* @brief   
****************************************************************************************
* @attention  
#####Copyright (c) 2022 GOODIX
All rights reserved.

****************************************************************************************
*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include "stdint.h"
#include "string.h"

#include "gh3x_drv.h"
#include "gh3x_demo_inner.h"
#include "gh3x_demo_config.h"
#include "gh3x_demo_protocol.h"
#include "gh_rpccore.h"

#define GH_BYTES_STREAM_DIFF_EN 1

extern GU32 gunFifoFullCnt;
GS32 zigzag_decode(GU32 x);



#if __SUPPORT_GHEALTH_PROTOCOL__
#define BYTES_BUFFER_SIZE 2048
GU8 g_uchbytesBuffer[BYTES_BUFFER_SIZE] = {0};
void GH3X_UprotocolSendFifoData(GU8 *puchReadRawdataBuffer, GU16 usReadRawdataLen, GU32 unFifoFull)
{
    static GU32 unFifoId = 0;
    GU8 *puchSenfBuffer = puchReadRawdataBuffer;
    GU16 usIndex = 0;
    const GU16 usSendMaxLen = 220;
    if(unFifoFull != 0)
    {
        unFifoId += 1;
    }
    while(usIndex < usReadRawdataLen)
    {
        GU16 usSendLen = (usReadRawdataLen - usIndex) > usSendMaxLen ? usSendMaxLen : (usReadRawdataLen - usIndex);
        RPCPoint rpcpoint = {puchSenfBuffer + usIndex,usSendLen};
        GHRPC_publish("F", "<u8*><u32>", rpcpoint, unFifoId++);
        usIndex += usSendLen;
    }

}

GS32 GH3X_UprotocolGetNumBits(GU32 num)
{
    GU32 bits = 0;

    while (num)
    {
        num >>= 1;
        bits++;
        if (bits == 32)
        {
            break;
        }
    }
    return bits;
}

GU32 zigzag_encode(GS32 x)
{
    return (GU32)((x>>31) ^ (x << 1));
}

void GH3X_UprotocolWriteBytes(GU8 *buffer, GS32 *pos, GS32 *data, GS32 dataSize)
{
    GU32 num = 0;
    GS32 num_bits = 0;
    GS32 num_bytes = 0;
    GU8 cur_byte = 0;
    GS32 cnt = dataSize, idx = 0;

    while (cnt--)
    {

        num = zigzag_encode(data[idx]);
        num_bits = GH3X_UprotocolGetNumBits(num);
        num_bytes = (num_bits + 6) / 7;
        if (num_bytes == 0)
        {
            num_bytes = 1;
        }
        while (num_bytes)
        {
            cur_byte = num & 0x7f;
            num >>= 7;

            if(num_bytes > 1)
            {
                cur_byte |= 0x80;
            }
            buffer[*pos] = cur_byte;
            (*pos)++;
            num_bytes--;
        }
        idx++;
    }

    return;
}

void GH3X_UprotocolReadBytes(GU8 *buffer, GS32 *pos, GS32 *dataOut, GS32 dataCnt)
{
    GU32 num = 0;
    GU8 cur_byte = 0;
    GS32 shift = 0;

    for (GS32 cnt = 0; cnt < dataCnt; cnt++)
    {
        do
        {
            cur_byte = buffer[*pos];
            num |= (cur_byte & 0x7f) << shift;
            shift += 7;
            (*pos)++;
        } while (cur_byte & 0x80);


        dataOut[cnt] = zigzag_decode(num);
        num = 0;
        shift = 0;
    }

    return;
}

GS32 GH3X_UprotocolRawdataToBytes(STRawdataFrameInfo *data, GU8 *buffer, GS32 bufferSize)
{
    GS32 pos = 0;

    GH3X_UprotocolWriteBytes(buffer, &pos, (GS32 *)&data->packHeader, sizeof(data->packHeader) / sizeof(STRawdataPackHeader));

    if (data->packHeader.ubGh3xDataEn)
    {
        GH3X_UprotocolWriteBytes(buffer, &pos, &data->unRawdataSize, sizeof(data->unRawdataSize) / sizeof(GS32));
        GH3X_UprotocolWriteBytes(buffer, &pos, data->punRawdata, data->unRawdataSize);
    }

    if (data->packHeader.ubGsDataEn)
    {
        GH3X_UprotocolWriteBytes(buffer, &pos, &data->unGsdataSize, sizeof(data->unGsdataSize) / sizeof(GS32));
        GH3X_UprotocolWriteBytes(buffer, &pos, data->punGsdata, data->unGsdataSize);
    }

    if (data->packHeader.ubFlagInfo)
    {
        GH3X_UprotocolWriteBytes(buffer, &pos, &data->unFlagdataBits, sizeof(data->unFlagdataBits) / sizeof(GS32));
        GH3X_UprotocolWriteBytes(buffer, &pos, data->punFlagdata, data->unFlagdataBits);
    }

    if (data->packHeader.ubAlgoData)
    {
        GH3X_UprotocolWriteBytes(buffer, &pos, &data->unAlgodataBits, sizeof(data->unAlgodataBits) / sizeof(GS32));
        GH3X_UprotocolWriteBytes(buffer, &pos, data->punAlgodata, data->unAlgodataBits);
    }

    if (data->packHeader.ubAgcInfo)
    {
        GH3X_UprotocolWriteBytes(buffer, &pos, &data->unAgcdataSize, sizeof(data->unAgcdataSize) / sizeof(GS32));
        GH3X_UprotocolWriteBytes(buffer, &pos, data->punAgcdata, data->unAgcdataSize);
    }

    if (data->packHeader.ubFrameId)
    {
        GH3X_UprotocolWriteBytes(buffer, &pos, &data->Frame_id, sizeof(data->Frame_id) / sizeof(GS32));
    }

    if (data->packHeader.ubFuncID)
    {
        GH3X_UprotocolWriteBytes(buffer, &pos, &data->function_id, sizeof(data->function_id) / sizeof(GS32));
    }

    if (pos > bufferSize)
    {
        // warning
        return bufferSize;
    }

    return pos;
}

GS32 GH3X_UprotocolBytesToRawdata(STRawdataFrameInfo *data, GU8 *buffer, GS32 bufferSize)
{
    GS32 pos = 0;

    GH3X_UprotocolReadBytes(buffer, &pos, (GS32 *)&data->packHeader, sizeof(data->packHeader) / sizeof(STRawdataPackHeader));

    if (data->packHeader.ubGh3xDataEn)
    {
        GH3X_UprotocolReadBytes(buffer, &pos, &data->unRawdataSize, sizeof(data->unRawdataSize) / sizeof(GS32));
        GH3X_UprotocolReadBytes(buffer, &pos, data->punRawdata, data->unRawdataSize);
    }

    if (data->packHeader.ubGsDataEn)
    {
        GH3X_UprotocolReadBytes(buffer, &pos, &data->unGsdataSize, sizeof(data->unGsdataSize) / sizeof(GS32));
        GH3X_UprotocolReadBytes(buffer, &pos, data->punGsdata, data->unGsdataSize);
    }

    if (data->packHeader.ubFlagInfo)
    {
        GH3X_UprotocolReadBytes(buffer, &pos, &data->unFlagdataBits, sizeof(data->unFlagdataBits) / sizeof(GS32));
        GH3X_UprotocolReadBytes(buffer, &pos, data->punFlagdata, data->unFlagdataBits);
    }

    if (data->packHeader.ubAlgoData)
    {
        GH3X_UprotocolReadBytes(buffer, &pos, &data->unAlgodataBits, sizeof(data->unAlgodataBits) / sizeof(GS32));
        GH3X_UprotocolReadBytes(buffer, &pos, data->punAlgodata, data->unAlgodataBits);
    }

    if (data->packHeader.ubAgcInfo)
    {
        GH3X_UprotocolReadBytes(buffer, &pos, &data->unAgcdataSize, sizeof(data->unAgcdataSize) / sizeof(GS32));
        GH3X_UprotocolReadBytes(buffer, &pos, data->punAgcdata, data->unAgcdataSize);
    }

    if (data->packHeader.ubFrameId)
    {
        GH3X_UprotocolReadBytes(buffer, &pos, &data->Frame_id, sizeof(data->Frame_id) / sizeof(GS32));
    }

    if (data->packHeader.ubFuncID)
    {
        GH3X_UprotocolReadBytes(buffer, &pos, &data->function_id, sizeof(data->function_id) / sizeof(GS32));
    }

    if (pos > bufferSize)
    {
        // warning
        return bufferSize;
    }

    return pos;
}

void GH3X_UprotocolLastDataUpdate(STFrameInfo *pstFrameInfo, GU8 dataCnt, GS32 *lastRawData)
{
    GU8 maxCnt = (dataCnt > GH3X_BT_CHNL_NUM) ? GH3X_BT_CHNL_NUM : dataCnt;
    GU8 i = 0;

    for (i = 0; i < maxCnt; i++)
    {
        lastRawData[i] = (GS32)pstFrameInfo->pstGh3xData->punRawdata[i];
    }

    for (GU8 j = i; j < (GH3X_BT_CHNL_NUM - maxCnt); j++)
    {
        lastRawData[j] = 0;
    }
}

void GH3X_UprotocolRawDataDiff(const STFrameInfo *const pstFrameInfo, GU8 dataCnt, GS32 *diffRawData, GS32 *lastRawData)
{
    GU8 maxCnt = (dataCnt > GH3X_BT_CHNL_NUM) ? GH3X_BT_CHNL_NUM : dataCnt;
    GU8 i = 0;

    for (i = 0; i < maxCnt; i++)
    {
        diffRawData[i] = (GS32)pstFrameInfo->pstGh3xData->punRawdata[i] - lastRawData[i];
        lastRawData[i] = (GS32)pstFrameInfo->pstGh3xData->punRawdata[i];
    }

    for (GU8 j = i; j < (GH3X_BT_CHNL_NUM - maxCnt); j++)
    {
        lastRawData[j] = 0;
    }
}

void GH3X_UprotocolSendFrameDataBytes(const STFrameInfo *const pstFrameInfo)
{
    static GS32 uchLastRawData[64] = {0};
    static GS32 uchDiffRawData[64] = {0};
    static STRawdataFrameBytesInfo stBytesInfo;

    GU8 uchIsEdgeFrame = (pstFrameInfo->pstFlagInfo->punFlag[6] >> 8) & 0xFF;
    if(uchIsEdgeFrame == 1)
    {
        GH3X_Memset(uchLastRawData,0,sizeof(uchLastRawData));
        GH3X_Memset(uchDiffRawData,0,sizeof(uchDiffRawData));
        stBytesInfo.punDiffRawData = uchDiffRawData;;
        stBytesInfo.punLastRawData = uchLastRawData;
        stBytesInfo.uchStartFlag = 1;
        stBytesInfo.uchEndFlag = 0;

        stBytesInfo.puchBytesBuffer = g_uchbytesBuffer;
        stBytesInfo.unBytesBufferLen = sizeof(g_uchbytesBuffer);
        stBytesInfo.unBytesBufferUsed = 0;
    }
    else if(uchIsEdgeFrame == 2)
    {
        stBytesInfo.uchEndFlag = 1;
    }
    STRawdataFrameBytesInfo *pstBytesInfo = &stBytesInfo;
    STRawdataFrameInfo frameInfo = {0};
    GU32 unFlagTemp[GH3X_FRAME_FLAG_NUM + 1] = {0};
    GU8 unFlagTempCnt = 1;
    GU32 unAlgoResultsTemp[20] = {0};
    GU8 unAlgoResultsTempCnt = 1;

    GU8 uchChnlOffset = 0;
    

    uchChnlOffset = pstFrameInfo->pstGh3xData->uchChnlNum;

    // for (GS32 i = 1; i <= pstFrameInfo->pstGh3xData->uchChnlNum; i++)
    // {
    //     if (pstFrameInfo->pstGh3xData->punRawdata[pstFrameInfo->pstGh3xData->uchChnlNum - i] == 0)
    //     {
    //         uchChnlOffset = pstFrameInfo->pstGh3xData->uchChnlNum - i;
    //         break;
    //     }
    // }

    frameInfo.packHeader.ubGh3xDataEn = (pstFrameInfo->pstGh3xData->uchChnlNum != 0) ? 1 : 0;
    // frameInfo.packHeader.ubGsDataEn = (pstFrameInfo->pstGsData->uchEnableFlag != 0) ? 1 : 0;
    frameInfo.packHeader.ubFlagInfo = (pstFrameInfo->pstFlagInfo->uchFlagBits != 0) ? 1 : 0;
    frameInfo.packHeader.ubAlgoData = (pstFrameInfo->pstAlgoData->uchFlag != 0) ? 1 : 0;
    frameInfo.packHeader.ubAgcInfo = (pstFrameInfo->pstGh3xData->punAgcInfo != GH3X_PTR_NULL) ? 1 : 0;
    frameInfo.packHeader.ubFrameId = 1;
    frameInfo.packHeader.ubFuncID = 1;
    memset(frameInfo.packHeader.reserved, 0, sizeof(frameInfo.packHeader.reserved));

    frameInfo.function_id = pstFrameInfo->uchFuncID;
    frameInfo.Frame_id = (GU8)(*pstFrameInfo->punFrameCnt);

    if (frameInfo.packHeader.ubGh3xDataEn)
    {
#if GH_BYTES_STREAM_DIFF_EN
        if (!pstBytesInfo->uchStartFlag)
        {
            GH3X_UprotocolRawDataDiff((STFrameInfo *)pstFrameInfo, uchChnlOffset, pstBytesInfo->punDiffRawData, pstBytesInfo->punLastRawData);
            frameInfo.unRawdataSize = uchChnlOffset;
            frameInfo.punRawdata = pstBytesInfo->punDiffRawData;
        }
        else
        {
            pstBytesInfo->uchStartFlag = 0;
            frameInfo.unRawdataSize = uchChnlOffset;
            frameInfo.punRawdata = (GS32 *)pstFrameInfo->pstGh3xData->punRawdata;
            GH3X_UprotocolLastDataUpdate((STFrameInfo *)pstFrameInfo, uchChnlOffset, pstBytesInfo->punLastRawData);
        }
#else
            frameInfo.unRawdataSize = uchChnlOffset;
            frameInfo.punRawdata = (GS32 *)pstFrameInfo->pstGh3xData->punRawdata;
#endif
    }

    if (frameInfo.packHeader.ubGsDataEn)
    {
        frameInfo.unGsdataSize = GSENSOR_FRAME_MAX_NUM;
        frameInfo.punGsdata = (GS32 *)pstFrameInfo->pstGsData->pusData;
    }

    if (frameInfo.packHeader.ubFlagInfo)
    {
        unFlagTemp[0] = ((GU32)pstFrameInfo->pstFlagInfo->uchFlagBits);
        for (int i = 0; i < GH3X_FRAME_FLAG_NUM; i++)
        {
            if (unFlagTemp[0] & (1 << i))
            {
                unFlagTemp[unFlagTempCnt] = pstFrameInfo->pstFlagInfo->punFlag[i];
                unFlagTempCnt++;
            }
        }

        frameInfo.unFlagdataBits = unFlagTempCnt;
        frameInfo.punFlagdata = (GS32 *)unFlagTemp;
    }

    if (frameInfo.packHeader.ubAlgoData)
    {
        unAlgoResultsTemp[0] = ((GU32)pstFrameInfo->pstAlgoData->unResultBits);
        for (GS32 i = 0; i < 16; i++)
        {
            if (unAlgoResultsTemp[0] & (1 << i))
            {
                unAlgoResultsTemp[unAlgoResultsTempCnt] = pstFrameInfo->pstAlgoData->pnResults[i];
                unAlgoResultsTempCnt++;
            }
        }
        if (unAlgoResultsTempCnt - 1 == pstFrameInfo->pstAlgoData->uchResultNum)
        {
            frameInfo.unAlgodataBits = unAlgoResultsTempCnt;
            frameInfo.punAlgodata = (GS32 *)unAlgoResultsTemp;
        }
    }

    if (frameInfo.packHeader.ubAgcInfo)
    {
        frameInfo.unAgcdataSize = pstFrameInfo->pstGh3xData->uchAgcNum ;
        frameInfo.punAgcdata = (GS32 *)pstFrameInfo->pstGh3xData->punAgcInfo;
    }

    pstBytesInfo->unBytesBufferUsed += GH3X_UprotocolRawdataToBytes(&frameInfo, &pstBytesInfo->puchBytesBuffer[pstBytesInfo->unBytesBufferUsed], pstBytesInfo->unBytesBufferLen);

    // send bytes stream when reach 200bytes or reach the end
    if (pstBytesInfo->unBytesBufferUsed >= 200 || pstBytesInfo->uchEndFlag)
    {
        // send
        RPCPoint rpcpoint = {pstBytesInfo->puchBytesBuffer, pstBytesInfo->unBytesBufferUsed};
        GHRPC_publish("G", "<u8*>", rpcpoint);

        // send finish
        pstBytesInfo->uchStartFlag = 1;
        if (pstBytesInfo->uchEndFlag == 1)
        {
            pstBytesInfo->uchEndFlag = 0;
        }
        pstBytesInfo->unBytesBufferUsed = 0;
        memset(pstBytesInfo->puchBytesBuffer, 0, pstBytesInfo->unBytesBufferLen);
    }
}

void GH3X_UprotocolSendChipEvent(GU32 unEvent)
{
    GU8 uchEvent[4];
    uchEvent[0] = ((unEvent >> 0) & 0xFF);
    uchEvent[1] = ((unEvent >> 8) & 0xFF);
    uchEvent[2] = ((unEvent >> 16) & 0xFF);
    uchEvent[3] = ((unEvent >> 24) & 0xFF);
    RPCPoint rpcpoint = {uchEvent, 4};
    GHRPC_publish("Event", "<u8*>", rpcpoint);
}


extern void GH3X_SerialSendBufWrite(GU8* puchDataBuf, GU16 usBufLen);
extern int GH3X_HalSerialSendData(char* uchTxDataBuf, int usBufLen);
GH_WEAK void GHProtocalLock(void)
{

}
GH_WEAK void GHProtocalUnLock(void)
{

}
static void sendFunction(void* data, int size)
{
#ifdef GOODIX_DEMO_PLATFORM
    GH3X_SerialSendBufWrite((GU8*)data,  size);
#else
    GH3X_HalSerialSendData(data, size);
#endif
}

void protocol_delay(void)
{
    gh3x_bsp_delay_ms(5);
}
void GhCommInit(void)
{
    GhRPCInitialInfo info;
    memset(&info,0,sizeof(GhRPCInitialInfo));
    info.lock = GHProtocalLock;
    info.unlock = GHProtocalUnLock;
    info.delay = protocol_delay;
    info.sendFunction = sendFunction;
    //info.mode = 0;
    GHRPC_init(info);
    //GH3X_SerialSendInit();
}

#endif
 
/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
