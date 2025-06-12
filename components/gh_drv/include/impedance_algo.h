/**
 * @copyright (c) 2003 - 2024, Goodix Co., Ltd. All rights reserved.
 *
 * @file    impedance_algo.h
 *
 * @brief   header file of gh3x bia offset process
 *
 * @version GH(M)3X2X_DRV_LIB_DEMO_1.0
 *
 * @author  Gooidx Iot Team
 *
 */
#ifndef __IMPEDANCE_ALGO_H__
#define __IMPEDANCE_ALGO_H__

#include "gh3x_bia_common.h"
#include "gh3x_bia_invalid_data_filter.h"
#include "gh3x_bia_offset_processor.h"
#include "gh3x_bia_cali_processor.h"
#include "gh3x_bia_impedance_fixer.h"

typedef struct {
    complex_S32 stIQSumInOneFrame;
    GU8 uchSampleCntInOneFrame;
} STIqAvaCalculator;


typedef struct {
	GU8 uchFrqPointNum;
    GU16 usFrqValue[BIA_FRQ_NUM_LIMIT];
} STFrqInfo;

typedef struct {
	GU8 uchFrqIndex;
    GU8 uchNetIndex;
} STImpedanceChnlMap;

typedef struct {
    STIqAvaCalculator stIqAvaCalculator;
    BIA_DATA_FILTER_T stInvalidDataFilter;
    BIA_OFFSET_PRO_T stBiaOffsetPro;
    BIA_CALI_PRO_T stBiaCaliPro;
    STFrqInfo stFrqInfo;
    GU8 uchWaitingLastFrameInFifoStream;
    GU8 uchSuccessedFlag;
	STImpedanceChnlMap pstImpedanceNetChnlMap[BIA_FRQ_NUM_LIMIT][BIA_FRAME_NUM_LIMIT];
    complex_double pstImpedanceOpenParam[BIA_IMPEDANCE_NET_NUM_LIMIT-1];
    complex_double pstImpedanceNetValue[BIA_FRQ_NUM_LIMIT][BIA_IMPEDANCE_NET_NUM_LIMIT];
    complex_double stImpedanceValueRaw;
    complex_double pstImpedanceZbodyFixed[BIA_FRQ_NUM_LIMIT];
    GU8 puchImpedanceNetValueValidFlag[BIA_FRQ_NUM_LIMIT];  //bit0: ZE1 bit1:ZE2 bit2: ZE3  bit3:ZE4  bit4:Z_body
    EM_BIA_SAMPLE_STATE emState;
    GU8 uchLastFrqIndex;
    GU8 uchLastNetIndex;
    GU8 uchUpFrqIndex;
    GU8 uchUpNetIndex;
} STImpedanceAglo;

typedef struct {
    GU8 uchGlobalFlag;
    GU8 uchFrqIndex;
    GU8 puchNetIndex;
    GU8 uchZbodyFlag;
} STUpdataFlag;


typedef struct {
    GU8 FrameId;
    GU8 SlotId;
    complex_S32 stIqRawdata;
} STImdanceAlgoSelfTestRawdata;

extern void BiaImpedanceAlgoInit(STImpedanceAglo *pstImpedanceAglo);
extern void BiaImpedanceAlgoNetChnlMapCfg(STImpedanceAglo *pstImpedanceAglo,STFrqInfo* pstFrqInfo, STImpedanceChnlMap* pstImpedanceNetChnlMap);
EM_BIA_SAMPLE_STATE BiaImpedanceAlgoExe(STImpedanceAglo *pstImpedanceAglo, complex_S32* pstIq, GU8 uchFrameId, GU8 uchSlotId, const STFrameInfo * const pstFrameInfo, STUpdataFlag* pstUpdataFlag);
extern void BiaImpedanceAlgoResult2Protocol(STUpdataFlag *pstUpdataFlag,STImpedanceAglo *pstImpedanceAglo, const STFrameInfo * const pstFrameInfo);
extern void GH3X_InitFunctionFrameInfo(const STFrameInfo * const pstFrameInfo);
#endif


