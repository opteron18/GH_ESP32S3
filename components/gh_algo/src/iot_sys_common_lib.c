/*
* RomaIICommon.c
*/

#include <stddef.h>

#include "iot_sys_common_lib.h"

float InnerRound(float x) {
    return (x > 0.0f) ? floor(x + 0.5f) : ceil(x - 0.5f);
}

///* Basic Functions */
GS32 IotGetSumF32(GS32 *pfData, GU32 nLen) {

    GS32 fSum = 0;
    GU32 ii = 0;
    for (ii = 0; ii < nLen; ii++) {
        fSum += pfData[ii];
    }
    return fSum;
}

GF32 IotGetMeanF32(GS32 *pfData, GU32 nLen) {

    GF32 fMean = 0;
    fMean = IotGetSumF32(pfData, nLen);
    fMean = fMean / nLen;
    return fMean;
}

GF32 IotGetVarianceF32(GS32 *pfData, GU32 nLen) {
    GF32 fMean = IotGetMeanF32(pfData, nLen);
    GF32 fSum = 0;
    GU32 ii = 0;
    for (ii = 0; ii < nLen; ii++) {
        fSum += ((GF32)pfData[ii] - fMean) * ((GF32)pfData[ii] - fMean);
    }
    return fSum / (nLen - 1);
}

GF32 IotGetStdF32(GS32 *pfData, GU32 nLen) {
    GF32 fVari = IotGetVarianceF32(pfData, nLen);
    return BIA_SQRTF(fVari);
}

/* Buffer Functions */
GS32 InnerBufferInitF32(ST_BIA_BUFFER_GS32_T *pstBuf, GS32*pfBuf, GS32 nSize) {

    if (NULL == pfBuf) {
        pfBuf = (GS32*)IOT_SYS_MALLOC(sizeof(GS32)*(nSize));
        if (pfBuf == NULL) {
            return 1;
        }
        memset(pfBuf, 0, sizeof(GS32)*(nSize));
        pstBuf->num = 0;
        pstBuf->addCount = 0;
    }
    else {
        pstBuf->num = nSize;
        pstBuf->addCount = nSize;
    }

    pstBuf->buf = pfBuf;
    pstBuf->size = nSize;

    return 0;
}

void InnerBufferAddF32(ST_BIA_BUFFER_GS32_T *pstBuf, GS32 fData) {

    if (pstBuf->num < pstBuf->size) {
        pstBuf->buf[pstBuf->num] = fData;
        pstBuf->num++;
    }
    else {
        memcpy(pstBuf->buf, &(pstBuf->buf[1]), sizeof(GS32)*(pstBuf->size - 1));
        pstBuf->buf[pstBuf->size - 1] = fData;
    }
    pstBuf->addCount++;
}

void InnerBufferDeinitF32(ST_BIA_BUFFER_GS32_T *stBuf) {

    if (stBuf->buf != NULL) {
        IOT_SYS_FREE(stBuf->buf);
        stBuf->buf = NULL;
    }
}

void InnerBufferGetMeanF32(ST_BIA_BUFFER_GS32_T *stBuf, GF32 *pRes) {

    if (stBuf->num == 0) {
        return;
    }
    *pRes = IotGetMeanF32(stBuf->buf, stBuf->num);
}
void InnerBufferGetSumF32(ST_BIA_BUFFER_GS32_T* stBuf, GS32* pRes) {

    if (stBuf->num == 0) {
        return;
    }
    *pRes = IotGetSumF32(stBuf->buf, stBuf->num);
}

void InnerBufferGetStdF32(ST_BIA_BUFFER_GS32_T *stBuf, GF32 *pRes) {
    if (stBuf->num == 0) {
        return;
    }
    *pRes = IotGetStdF32(stBuf->buf, stBuf->num);
}

GU8 InnerBufferIsFullF32(ST_BIA_BUFFER_GS32_T *stBuf) {

    return (stBuf->num == stBuf->size);
}

GS32 InnerBufferIsRoundF32(ST_BIA_BUFFER_GS32_T *stBuf) {
    GS32 nRes = 0;
    nRes = ((stBuf->addCount % stBuf->size) == 0);
    return nRes;
}

GF32 InnerBufferGetValueAtIdxF32(ST_BIA_BUFFER_GS32_T *stBuf, GU32 nIdx) {

    if (nIdx >= stBuf->size) {
        return 0.0f;
    }
    return stBuf->buf[nIdx];
}

GU32 InnerBufferGetNumF32(ST_BIA_BUFFER_GS32_T *stBuf) {

    return stBuf->num;
}

void IotBubbleSortF32(GF32 fDataArr[], GS32 nArraySize) {

    GS32 ii = 0;
    GS32 jj = 0;
    GF32 fSwap;

    for (ii = (nArraySize - 1); ii > 0; ii--) {
        for (jj = 1; jj <= ii; jj++) {
            if (fDataArr[jj - 1] > fDataArr[jj]) {
                fSwap = fDataArr[jj - 1];
                fDataArr[jj - 1] = fDataArr[jj];
                fDataArr[jj] = fSwap;
            }
        }
    }
}
