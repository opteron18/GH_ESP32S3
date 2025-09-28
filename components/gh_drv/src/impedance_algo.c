#include "impedance_algo.h"
#include "gh3x_demo_config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

#if __FUNC_TYPE_BIA_ENABLE__

#define BIA_INVALID_IMP_CHNL_MAP_VALUE  0xFF
#define BIA_INVALID_IMP_FRQ_INDEX  0xFF
#define BIA_INVALID_IMP_NET_INDEX  0xFF
#define BIA_IMP_Z_BODY_NET_INDEX  4

#define SELF_TEST  0



static void IqAvaCalculatorInit(STIqAvaCalculator *pstIqAvaCalculator)
{
    GH3X_Memset((void*)(pstIqAvaCalculator), 0, sizeof(STIqAvaCalculator));
}

static void IqAvaCalculatorAccumulate(STIqAvaCalculator *pstIqAvaCalculator, complex_S32* pstInputIq)
{
    pstIqAvaCalculator->stIQSumInOneFrame.real += pstInputIq->real;
    pstIqAvaCalculator->stIQSumInOneFrame.imag += pstInputIq->imag;
    pstIqAvaCalculator->uchSampleCntInOneFrame ++;
}

static GU8 IqAvaCalculatorAverage(STIqAvaCalculator *pstIqAvaCalculator, complex_S32* pstOutputIq)
{
    if(pstIqAvaCalculator->uchSampleCntInOneFrame)
    {
        pstOutputIq->real = pstIqAvaCalculator->stIQSumInOneFrame.real/pstIqAvaCalculator->uchSampleCntInOneFrame;
        pstOutputIq->imag = pstIqAvaCalculator->stIQSumInOneFrame.imag/pstIqAvaCalculator->uchSampleCntInOneFrame;
        return 1;
    }
    else
    {
        return 0;
    }
}

#if SELF_TEST
const STImdanceAlgoSelfTestRawdata g_SelfTestRawdataOffset[] = 
{
    /*frame id     slot id    I     Q*/
    //offset
    {0,             0,       {-10,  10}},
    {0,             1,       {-10,  10}},
    {0,             2,       {-10,  10}},
};

const STImdanceAlgoSelfTestRawdata g_SelfTestRawdataCali[] = 
{
    //cali
    {0,             0,       {2000-10,  10}},
    {0,             1,       {2000-10,  10}},
    {0,             2,       {2000-10,  10}},
};

const STImdanceAlgoSelfTestRawdata g_SelfTestRawdataNet[] = 
{
    //net impedance
    {1,             0,       {-30,  31}},
    {1,             1,       {-30,  32}},
    {1,             2,       {-30,  33}},
    {2,             0,       {-40,  41}},
    {2,             1,       {-40,  42}},
    {2,             2,       {-40,  43}},
    {3,             0,       {-50,  51}},
    {3,             1,       {-50,  52}},
    {3,             2,       {-50,  53}},
    {4,             0,       {-60,  61}},
    {4,             1,       {-60,  62}},
    {4,             2,       {-60,  63}},
    {5,             0,       {-70,  71}},
    {5,             1,       {-70,  72}},
    {5,             2,       {-70,  73}},

};


STImpedanceAglo g_stSelfTestAlgo;
#define SELF_TEST_FRAM_INVALI_POINT  3
#define SELF_TEST_FRAM_ALL_POINT  6


void BiaSelfDataGen(EM_BIA_SAMPLE_STATE emState, STImdanceAlgoSelfTestRawdata *pstRawdata)
{
    static GU16 usDataSwtichCnt = 0;
    static GU16 usSampleCnt = 0;
    static EM_BIA_SAMPLE_STATE emLastState = BIA_SAMPLE_STATE_OFFSET;

    if(emLastState != emState)
    {
        usDataSwtichCnt = 0;
        usSampleCnt = 0;
    }


    GU16 usSwtichLimit = sizeof(g_SelfTestRawdataOffset)/sizeof(STImdanceAlgoSelfTestRawdata);

    const STImdanceAlgoSelfTestRawdata *pstRawdataUsed = g_SelfTestRawdataOffset;

    if(BIA_SAMPLE_STATE_OFFSET == emState)
    {
        pstRawdataUsed = g_SelfTestRawdataOffset;
        usSwtichLimit = sizeof(g_SelfTestRawdataOffset)/sizeof(STImdanceAlgoSelfTestRawdata);
    }
    if(BIA_SAMPLE_STATE_CALI == emState)
    {
        pstRawdataUsed = g_SelfTestRawdataCali;
        usSwtichLimit = sizeof(g_SelfTestRawdataCali)/sizeof(STImdanceAlgoSelfTestRawdata);
    }
    if(BIA_SAMPLE_STATE_NET_IMEDANCE == emState)
    {
        pstRawdataUsed = g_SelfTestRawdataNet;
         usSwtichLimit = sizeof(g_SelfTestRawdataNet)/sizeof(STImdanceAlgoSelfTestRawdata);
    }
    pstRawdata->FrameId = pstRawdataUsed[usDataSwtichCnt].FrameId;
    pstRawdata->SlotId = pstRawdataUsed[usDataSwtichCnt].SlotId;

    if(usSampleCnt < SELF_TEST_FRAM_INVALI_POINT)
    {
        pstRawdata->stIqRawdata.real = 0;
        pstRawdata->stIqRawdata.imag = 0;
    }
    else
    {
        pstRawdata->stIqRawdata.real = pstRawdataUsed[usDataSwtichCnt].stIqRawdata.real;
        pstRawdata->stIqRawdata.imag = pstRawdataUsed[usDataSwtichCnt].stIqRawdata.imag;
    }

    usSampleCnt ++;
    if(usSampleCnt >= SELF_TEST_FRAM_ALL_POINT)
    {
        usSampleCnt = 0;
        usDataSwtichCnt ++;
    }
    if(usDataSwtichCnt >= usSwtichLimit)
    {
        usDataSwtichCnt = 0;
    }
    emLastState = emState;
}

void BiaImpedanceAlgoSelfTest(void)
{
    EM_BIA_SAMPLE_STATE emState;
    EM_BIA_SAMPLE_STATE emStateNext;
    emState = BIA_SAMPLE_STATE_OFFSET;
    BiaImpedanceAlgoInit(&g_stSelfTestAlgo);
    GU16 usFrameCnt = 0;

    while(1)
    {
        STImdanceAlgoSelfTestRawdata stRawdata;
        STUpdataFlag stUpdataFlag;
        BiaSelfDataGen(emState,&stRawdata);
        #if 1
        if(BIA_SAMPLE_STATE_NET_IMEDANCE == emState)
        {
            EXAMPLE_LOG("[SelfTest] usFrameCnt = %d, frame id = %d, slot id = %d, I = %.3f, Q = %.3f ", 
            (int)usFrameCnt, 
            (int)stRawdata.FrameId,
            (int)stRawdata.SlotId,
            (float)stRawdata.stIqRawdata.real,
            (float)stRawdata.stIqRawdata.imag
            );
            vTaskDelay(400);
        }
        #endif

        emStateNext = BiaImpedanceAlgoExe(&g_stSelfTestAlgo, &stRawdata.stIqRawdata, stRawdata.FrameId, stRawdata.SlotId,0, &stUpdataFlag);
        
        if(emStateNext != emState)
        {
            BiaImpedanceAlgoExe(&g_stSelfTestAlgo, &stRawdata.stIqRawdata, stRawdata.FrameId, stRawdata.SlotId,1, &stUpdataFlag);
            emState = emStateNext;
        }
        usFrameCnt ++;
        
    }
}

#endif


extern complex_double gstZopen[];

/// @brief BiaImpedanceAlgoInit
/// @param pstImpedanceAglo point of instance of STImpedanceAglo
void BiaImpedanceAlgoInit(STImpedanceAglo *pstImpedanceAglo)
{
    //private member clear to zero
    GH3X_Memset((void*)(pstImpedanceAglo), 0, sizeof(STImpedanceAglo));

    //sub-class init
    BiaInvalidDataFilterInit(&pstImpedanceAglo->stInvalidDataFilter);
    BiaOffsetProcessorInit(&pstImpedanceAglo->stBiaOffsetPro);
    BiaCaliProcessorInit(&pstImpedanceAglo->stBiaCaliPro);
    IqAvaCalculatorInit(&pstImpedanceAglo->stIqAvaCalculator);
    
    //this class init
    GH3X_Memset((void*)(pstImpedanceAglo->pstImpedanceNetChnlMap), BIA_INVALID_IMP_CHNL_MAP_VALUE, sizeof(pstImpedanceAglo->pstImpedanceNetChnlMap));
    pstImpedanceAglo->emState = BIA_SAMPLE_STATE_OFFSET;
    pstImpedanceAglo->uchLastFrqIndex = BIA_INVALID_IMP_FRQ_INDEX;
    pstImpedanceAglo->uchLastNetIndex = BIA_INVALID_IMP_NET_INDEX;
    pstImpedanceAglo->uchUpFrqIndex = BIA_INVALID_IMP_FRQ_INDEX;
#if SELF_TEST
    gstZopen[0].real = -1;
    gstZopen[0].imag = 1;
    gstZopen[1].real = -2;
    gstZopen[1].imag = 2;
    gstZopen[2].real = -3;
    gstZopen[2].imag = 3;
    gstZopen[3].real = -4;
    gstZopen[3].imag = 4;
    gstZopen[4].real = -5;
    gstZopen[4].imag = 5;
#endif

#if __BIA_IMPEDANCE_FIXER__  
    GH3X_Memcpy((void*)(pstImpedanceAglo->pstImpedanceOpenParam),gstZopen,sizeof(pstImpedanceAglo->pstImpedanceOpenParam));
#endif

#if SELF_TEST
    //init for self-test
    pstImpedanceAglo->stFrqInfo.uchFrqPointNum = 3;
    pstImpedanceAglo->stFrqInfo.usFrqValue[0] = 5;
    pstImpedanceAglo->stFrqInfo.usFrqValue[1] = 50;
    pstImpedanceAglo->stFrqInfo.usFrqValue[2] = 250;

    pstImpedanceAglo->pstImpedanceNetChnlMap[0][0].uchFrqIndex = 0;
    pstImpedanceAglo->pstImpedanceNetChnlMap[0][0].uchNetIndex = 4;
    pstImpedanceAglo->pstImpedanceNetChnlMap[1][0].uchFrqIndex = 1;
    pstImpedanceAglo->pstImpedanceNetChnlMap[1][0].uchNetIndex = 4;
    pstImpedanceAglo->pstImpedanceNetChnlMap[2][0].uchFrqIndex = 2;
    pstImpedanceAglo->pstImpedanceNetChnlMap[2][0].uchNetIndex = 4;

    pstImpedanceAglo->pstImpedanceNetChnlMap[0][1].uchFrqIndex = 0;
    pstImpedanceAglo->pstImpedanceNetChnlMap[0][1].uchNetIndex = 0;
    pstImpedanceAglo->pstImpedanceNetChnlMap[1][1].uchFrqIndex = 1;
    pstImpedanceAglo->pstImpedanceNetChnlMap[1][1].uchNetIndex = 0;
    pstImpedanceAglo->pstImpedanceNetChnlMap[2][1].uchFrqIndex = 2;
    pstImpedanceAglo->pstImpedanceNetChnlMap[2][1].uchNetIndex = 0;

    pstImpedanceAglo->pstImpedanceNetChnlMap[0][2].uchFrqIndex = 0;
    pstImpedanceAglo->pstImpedanceNetChnlMap[0][2].uchNetIndex = 1;
    pstImpedanceAglo->pstImpedanceNetChnlMap[1][2].uchFrqIndex = 1;
    pstImpedanceAglo->pstImpedanceNetChnlMap[1][2].uchNetIndex = 1;
    pstImpedanceAglo->pstImpedanceNetChnlMap[2][2].uchFrqIndex = 2;
    pstImpedanceAglo->pstImpedanceNetChnlMap[2][2].uchNetIndex = 1;

    pstImpedanceAglo->pstImpedanceNetChnlMap[0][3].uchFrqIndex = 0;
    pstImpedanceAglo->pstImpedanceNetChnlMap[0][3].uchNetIndex = 2;
    pstImpedanceAglo->pstImpedanceNetChnlMap[1][3].uchFrqIndex = 1;
    pstImpedanceAglo->pstImpedanceNetChnlMap[1][3].uchNetIndex = 2;
    pstImpedanceAglo->pstImpedanceNetChnlMap[2][3].uchFrqIndex = 2;
    pstImpedanceAglo->pstImpedanceNetChnlMap[2][3].uchNetIndex = 2;

    pstImpedanceAglo->pstImpedanceNetChnlMap[0][4].uchFrqIndex = 0;
    pstImpedanceAglo->pstImpedanceNetChnlMap[0][4].uchNetIndex = 3;
    pstImpedanceAglo->pstImpedanceNetChnlMap[1][4].uchFrqIndex = 1;
    pstImpedanceAglo->pstImpedanceNetChnlMap[1][4].uchNetIndex = 3;
    pstImpedanceAglo->pstImpedanceNetChnlMap[2][4].uchFrqIndex = 2;
    pstImpedanceAglo->pstImpedanceNetChnlMap[2][4].uchNetIndex = 3;

    pstImpedanceAglo->pstImpedanceNetChnlMap[0][5].uchFrqIndex = 0;
    pstImpedanceAglo->pstImpedanceNetChnlMap[0][5].uchNetIndex = 4;
    pstImpedanceAglo->pstImpedanceNetChnlMap[1][5].uchFrqIndex = 1;
    pstImpedanceAglo->pstImpedanceNetChnlMap[1][5].uchNetIndex = 4;
    pstImpedanceAglo->pstImpedanceNetChnlMap[2][5].uchFrqIndex = 2;
    pstImpedanceAglo->pstImpedanceNetChnlMap[2][5].uchNetIndex = 4;
#endif
}


/// @brief bia impedance algo excute function
/// @param pstImpedanceAglo point of instance of STImpedanceAglo
/// @param pstIq this frame IQ
/// @param uchFrameId frame id of this frame IQ
/// @param uchSlotId slot id of this frame IQ
/// @param uchLastFrameFlagInFifoStream  1: this IQ frame is last frame in fifo stream  0: this IQ frame is NOT last frame in fifo stream
/// @param pstUpdataFlag update flag
/// @return request of next state
EM_BIA_SAMPLE_STATE BiaImpedanceAlgoExe(STImpedanceAglo *pstImpedanceAglo, complex_S32* pstIq, GU8 uchFrameId, GU8 uchSlotId, const STFrameInfo * const pstFrameInfo, STUpdataFlag* pstUpdataFlag)
{
    GU8 uchFrqIndex;
    GU8 uchNetIndex;

    EM_BIA_SAMPLE_STATE emStateNext = pstImpedanceAglo->emState;
//    SlotId=,I = %d,Q = %d\r\n", pstFrameInfo->pstFlagInfo->punFlag[0],*(pstFrameInfo->punFrameCnt),pstImpedanceAglo->uchWaitingLastFrameInFifoStream,uchSlotId,pstIq->real,pstIq->imag);

    GH3X_Memset((void*)pstUpdataFlag, 0, sizeof(STUpdataFlag));

    if(pstImpedanceAglo->uchWaitingLastFrameInFifoStream)
    {
        if(pstFrameInfo->pstFlagInfo->punFlag[0])
        {
            pstImpedanceAglo->uchWaitingLastFrameInFifoStream = 0;
//            GH3X_InitFunctionFrameInfo(pstFrameInfo);  
        }
        goto DATA_SKIP;
    }
    
    if(BIA_DATA_INVALID == BiaInvalidDataFilterExe(&pstImpedanceAglo->stInvalidDataFilter, uchFrameId, uchSlotId))
    {
        goto DATA_SKIP;
    }
    
    uchFrqIndex = pstImpedanceAglo->pstImpedanceNetChnlMap[uchSlotId][uchFrameId].uchFrqIndex;
    uchNetIndex = pstImpedanceAglo->pstImpedanceNetChnlMap[uchSlotId][uchFrameId].uchNetIndex;
    
    if((uchFrqIndex >= BIA_FRQ_NUM_LIMIT)||(uchNetIndex >= BIA_IMPEDANCE_NET_NUM_LIMIT))  //index is out of range
    {
        goto DATA_SKIP;
    }

    if(BIA_SAMPLE_STATE_OFFSET == pstImpedanceAglo->emState)
    {
        if(BIA_OFFSET_STATE_SUCCESSED ==  BiaOffsetProcessorOffsetRecord(&pstImpedanceAglo->stBiaOffsetPro, pstIq, uchFrqIndex))
        {
            pstImpedanceAglo->uchSuccessedFlag |= (1 << (uchFrqIndex));
            if((pstImpedanceAglo->uchSuccessedFlag >= ((1 << pstImpedanceAglo->stFrqInfo.uchFrqPointNum) - 1)))
            {
                emStateNext = BIA_SAMPLE_STATE_CALI;
                
            }
        }
        goto STATE_SWITCH_POST_PRO;
    }
    else if(BIA_SAMPLE_STATE_CALI == pstImpedanceAglo->emState)
    {
        complex_S32 stDeoffsetIq;
        EXAMPLE_LOG("[StandardResisIqRecord No offset] FreqIndex=%d,I = %d,Q = %d\r\n", uchFrqIndex, pstIq->real,pstIq->imag);
        BiaOffsetProcessorDeoffset(&pstImpedanceAglo->stBiaOffsetPro, pstIq, uchFrqIndex, &stDeoffsetIq);
        if(BIA_CALI_STATE_SUCCESSED == BiaCaliProcessorStandardResisIqRecord(&pstImpedanceAglo->stBiaCaliPro, &stDeoffsetIq, uchFrqIndex))
        {
            pstImpedanceAglo->uchSuccessedFlag |= (1 << (uchFrqIndex));
            if((pstImpedanceAglo->uchSuccessedFlag >= ((1 << pstImpedanceAglo->stFrqInfo.uchFrqPointNum) - 1)))
            {
                emStateNext = BIA_SAMPLE_STATE_NET_IMEDANCE;
            }
        }
        goto STATE_SWITCH_POST_PRO;
    }
    else if(BIA_SAMPLE_STATE_NET_IMEDANCE == pstImpedanceAglo->emState)
    {
        if((pstImpedanceAglo->uchLastFrqIndex != uchFrqIndex)||(pstImpedanceAglo->uchLastNetIndex != uchNetIndex))  //frq index or net index has changed, we need ouput average of IQ, and use this IQ to calculate impedance
        {
            complex_S32 stAvgIq;
            if(IqAvaCalculatorAverage(&pstImpedanceAglo->stIqAvaCalculator,&stAvgIq))
            {
                BiaOffsetProcessorDeoffset(&pstImpedanceAglo->stBiaOffsetPro, &stAvgIq, pstImpedanceAglo->uchLastFrqIndex, &stAvgIq);
                BiaCaliProcessorCalibrate(&pstImpedanceAglo->stBiaCaliPro, &stAvgIq, pstImpedanceAglo->uchLastFrqIndex,\
                   &(pstImpedanceAglo->pstImpedanceNetValue[pstImpedanceAglo->uchLastFrqIndex][pstImpedanceAglo->uchLastNetIndex]));
                
                pstImpedanceAglo->stImpedanceValueRaw = pstImpedanceAglo->pstImpedanceNetValue[pstImpedanceAglo->uchLastFrqIndex][pstImpedanceAglo->uchLastNetIndex];
                
                pstImpedanceAglo->puchImpedanceNetValueValidFlag[pstImpedanceAglo->uchLastFrqIndex] |= (1 << (pstImpedanceAglo->uchLastNetIndex));
            
                if(pstImpedanceAglo->uchLastNetIndex < BIA_IMP_Z_BODY_NET_INDEX)  //this iq is from contact net
                {
                    //call contact fix algo
#if __BIA_IMPEDANCE_FIXER__                    
                    BiaImpedanceFixerContactFixViaOpenParam( 
                        &(pstImpedanceAglo->pstImpedanceNetValue[pstImpedanceAglo->uchLastFrqIndex][pstImpedanceAglo->uchLastNetIndex]),
                        pstImpedanceAglo->pstImpedanceOpenParam[pstImpedanceAglo->uchLastNetIndex],
                        pstImpedanceAglo->stFrqInfo.usFrqValue[pstImpedanceAglo->uchLastFrqIndex], 
                        &(pstImpedanceAglo->pstImpedanceNetValue[pstImpedanceAglo->uchLastFrqIndex][pstImpedanceAglo->uchLastNetIndex])
                        );
#endif                    
                    pstUpdataFlag->uchGlobalFlag = 1;
                    
                }

                if(pstImpedanceAglo->puchImpedanceNetValueValidFlag[pstImpedanceAglo->uchLastFrqIndex] >= ((1 << (BIA_IMP_Z_BODY_NET_INDEX + 1)) - 1))  //four contact and zbody values are all avalible
                {
#if __BIA_IMPEDANCE_FIXER__
                    BiaImpedanceFixerZbodyFix(
                        pstImpedanceAglo->pstImpedanceNetValue[pstImpedanceAglo->uchLastFrqIndex],
                        pstImpedanceAglo->stFrqInfo.usFrqValue[pstImpedanceAglo->uchLastFrqIndex],
                        &(pstImpedanceAglo->pstImpedanceZbodyFixed[pstImpedanceAglo->uchLastFrqIndex])
                        );
#endif                    
                    pstImpedanceAglo->puchImpedanceNetValueValidFlag[pstImpedanceAglo->uchLastFrqIndex] |= (1 << BIA_IMP_Z_BODY_NET_INDEX);
                    pstUpdataFlag->uchGlobalFlag = 1;
                    pstUpdataFlag->uchZbodyFlag = 1;
 
                }
                pstUpdataFlag->puchNetIndex = pstImpedanceAglo->uchLastNetIndex;
                pstUpdataFlag->uchFrqIndex = pstImpedanceAglo->uchLastFrqIndex;

                EXAMPLE_LOG("[BiaImpedanceAlgoExe]gf=%d, zf=%d, puchNetIndex= %d, uchFrqIndex =%d, I = %.3f, Q = %.3f\r\n", 
                (int)pstUpdataFlag->uchGlobalFlag,
                (int)pstUpdataFlag->uchZbodyFlag,
                (int)pstUpdataFlag->puchNetIndex,
                (int)pstUpdataFlag->uchFrqIndex,
                (double)pstImpedanceAglo->pstImpedanceNetValue[pstUpdataFlag->uchFrqIndex][pstUpdataFlag->puchNetIndex].real,
                (double)pstImpedanceAglo->pstImpedanceNetValue[pstUpdataFlag->uchFrqIndex][pstUpdataFlag->puchNetIndex].imag
                );
            }
            IqAvaCalculatorInit(&pstImpedanceAglo->stIqAvaCalculator);
        }
        IqAvaCalculatorAccumulate(&pstImpedanceAglo->stIqAvaCalculator,pstIq);
        goto STATE_SWITCH_POST_PRO;
    }

STATE_SWITCH_POST_PRO:
    if(pstImpedanceAglo->emState != emStateNext)
    {
        EXAMPLE_LOG("[BiaImpedanceAlgoExe] new state = %d\r\n", (int)emStateNext);
        IqAvaCalculatorInit(&pstImpedanceAglo->stIqAvaCalculator);
        pstImpedanceAglo->uchSuccessedFlag = 0;
        pstImpedanceAglo->uchLastFrqIndex = BIA_INVALID_IMP_FRQ_INDEX;
        pstImpedanceAglo->uchLastNetIndex = BIA_INVALID_IMP_NET_INDEX;
        pstImpedanceAglo->emState = emStateNext;
        if((0 == pstFrameInfo->pstFlagInfo->punFlag[0])&&(BIA_SAMPLE_STATE_CALI== emStateNext))
        {
            pstImpedanceAglo->uchWaitingLastFrameInFifoStream = 1;
        }
        
    }
    else
    {
        pstImpedanceAglo->uchLastFrqIndex = uchFrqIndex;
        pstImpedanceAglo->uchLastNetIndex = uchNetIndex;
    }

DATA_SKIP:
    return emStateNext;
}


void BiaImpedanceAlgoNetChnlMapCfg(STImpedanceAglo *pstImpedanceAglo,STFrqInfo* pstFrqInfo, STImpedanceChnlMap* pstImpedanceNetChnlMap)
{
    GH3X_Memcpy((void *) (&(pstImpedanceAglo->stFrqInfo)), (void *)pstFrqInfo, sizeof(STFrqInfo));
    GH3X_Memcpy((void *) (pstImpedanceAglo->pstImpedanceNetChnlMap), (void *)pstImpedanceNetChnlMap, sizeof(STImpedanceChnlMap)*BIA_FRQ_NUM_LIMIT*BIA_FRAME_NUM_LIMIT);
}

#define pi (3.1415926)
#define F_pF (1000000000000)
#define BIA_FS  (250000)

uint16_t BiaImpedanceAlgoCapCalculate(float imag)
{
    uint16_t cap_value = -10*F_pF/(2*pi*BIA_FS*imag); // parasitic capacitance, unit(0.1pF)
    return cap_value;
}

extern complex_double gstZopenOff[];
extern complex_double gstZopenOn[];

void BiaImpedanceAlgoResult2Protocol(STUpdataFlag *pstUpdataFlag, STImpedanceAglo *pstImpedanceAglo, const STFrameInfo * const pstFrameInfo)
{
    
    pstFrameInfo->pstGh3xData->uchAgcNum = 0;
    pstFrameInfo->pstAlgoData->uchResultNum =  0;       
    if(pstUpdataFlag->uchGlobalFlag)
    {
      pstImpedanceAglo->uchUpNetIndex =  pstUpdataFlag->puchNetIndex;
      pstImpedanceAglo->uchUpFrqIndex = pstUpdataFlag->uchFrqIndex;
      ///// Algoresult                           
      pstFrameInfo->pstAlgoData->pnResults[3] =  (GS32)(1000*pstImpedanceAglo->stImpedanceValueRaw.real); //// I after cali
      pstFrameInfo->pstAlgoData->pnResults[4] =  (GS32)(1000*pstImpedanceAglo->stImpedanceValueRaw.imag); //// Q after cali
      pstFrameInfo->pstAlgoData->unResultBits |=  0x0018;
      pstFrameInfo->pstAlgoData->uchResultNum =  2;   
      pstFrameInfo->pstAlgoData->uchFlag =  1;          
    }
    
    if(pstUpdataFlag->uchZbodyFlag)
    {
       /////  AgcInfo save fixer I\Q
       pstImpedanceAglo->uchUpFrqIndex = pstUpdataFlag->uchFrqIndex; 
       pstFrameInfo->pstGh3xData->uchAgcNum = 14;
  
       pstFrameInfo->pstGh3xData->punAgcInfo[0] =  (GS32)(1000*pstImpedanceAglo->pstImpedanceNetValue[pstImpedanceAglo->uchUpFrqIndex][0].real);
       pstFrameInfo->pstGh3xData->punAgcInfo[1] =  (GS32)(1000*pstImpedanceAglo->pstImpedanceNetValue[pstImpedanceAglo->uchUpFrqIndex][0].imag);
       pstFrameInfo->pstGh3xData->punAgcInfo[4] =  (GS32)(1000*pstImpedanceAglo->pstImpedanceNetValue[pstImpedanceAglo->uchUpFrqIndex][1].real);
       pstFrameInfo->pstGh3xData->punAgcInfo[5] =  (GS32)(1000*pstImpedanceAglo->pstImpedanceNetValue[pstImpedanceAglo->uchUpFrqIndex][1].imag);
       pstFrameInfo->pstGh3xData->punAgcInfo[2] =  (GS32)(1000*pstImpedanceAglo->pstImpedanceNetValue[pstImpedanceAglo->uchUpFrqIndex][2].real);
       pstFrameInfo->pstGh3xData->punAgcInfo[3] =  (GS32)(1000*pstImpedanceAglo->pstImpedanceNetValue[pstImpedanceAglo->uchUpFrqIndex][2].imag);
       pstFrameInfo->pstGh3xData->punAgcInfo[6] =  (GS32)(1000*pstImpedanceAglo->pstImpedanceNetValue[pstImpedanceAglo->uchUpFrqIndex][3].real);
       pstFrameInfo->pstGh3xData->punAgcInfo[7] =  (GS32)(1000*pstImpedanceAglo->pstImpedanceNetValue[pstImpedanceAglo->uchUpFrqIndex][3].imag);
#if __BIA_IMPEDANCE_FIXER__       
       pstFrameInfo->pstGh3xData->punAgcInfo[8] =  (GS32)(1000*pstImpedanceAglo->pstImpedanceZbodyFixed[pstImpedanceAglo->uchUpFrqIndex].real);
       pstFrameInfo->pstGh3xData->punAgcInfo[9] =  (GS32)(1000*pstImpedanceAglo->pstImpedanceZbodyFixed[pstImpedanceAglo->uchUpFrqIndex].imag); 
#else
       pstFrameInfo->pstGh3xData->punAgcInfo[8] =  (GS32)(1000*pstImpedanceAglo->pstImpedanceNetValue[pstImpedanceAglo->uchUpFrqIndex][4].real);
       pstFrameInfo->pstGh3xData->punAgcInfo[9] =  (GS32)(1000*pstImpedanceAglo->pstImpedanceNetValue[pstImpedanceAglo->uchUpFrqIndex][4].imag);
#endif 

       //// parasitic capacitance in sheid on/off
       uint16_t cap_value,cap_value_off;
       cap_value = BiaImpedanceAlgoCapCalculate( gstZopenOn[0].imag);
       cap_value_off = BiaImpedanceAlgoCapCalculate(gstZopenOff[0].imag);
       pstFrameInfo->pstGh3xData->punAgcInfo[10] = ((cap_value << 16) + cap_value_off);

       EXAMPLE_LOG("[BiaUploadRaw] cap_off=%d, cap_on=%d\r\n",cap_value_off,cap_value); 
       
       cap_value = BiaImpedanceAlgoCapCalculate( gstZopenOn[1].imag);
       cap_value_off = BiaImpedanceAlgoCapCalculate(gstZopenOff[1].imag);
       pstFrameInfo->pstGh3xData->punAgcInfo[12] = ((cap_value << 16) + cap_value_off);

       cap_value = BiaImpedanceAlgoCapCalculate( gstZopenOn[2].imag);
       cap_value_off = BiaImpedanceAlgoCapCalculate(gstZopenOff[2].imag);
       pstFrameInfo->pstGh3xData->punAgcInfo[11] = ((cap_value << 16) + cap_value_off);

       cap_value = BiaImpedanceAlgoCapCalculate( gstZopenOn[3].imag);
       cap_value_off = BiaImpedanceAlgoCapCalculate(gstZopenOff[3].imag);
       pstFrameInfo->pstGh3xData->punAgcInfo[13] = ((cap_value << 16) + cap_value_off);

//       EXAMPLE_LOG("[BiaUploadRaw] Updata=%d,FrqIndex=%d,F3=%d,I5=%.3f,Q5=%.3f\r\n",
//                    pstUpdataFlag->uchZbodyFlag,pstImpedanceAglo->uchUpFrqIndex, pstFrameInfo->pstFlagInfo->punFlag[3],
//              (double)pstImpedanceAglo->pstImpedanceZbodyFixed[0].real,(double)pstImpedanceAglo->pstImpedanceZbodyFixed[0].imag); 
        
    }
    
    ///// Flag 
    pstFrameInfo->pstFlagInfo->punFlag[1] =  pstImpedanceAglo->emState;  // Flag1:    BIA STATE 
    pstFrameInfo->pstFlagInfo->uchFlagBits |= (1<<1);
    // Flag3:   bit0-3:  BIA_Slot_num : 0-7;   bit8-23: Frequecy(kHz)
    GU8  slotIndex = pstFrameInfo->pstFlagInfo->punFlag[3]&0X0F;
    GU32 unTxFs = pstImpedanceAglo->stFrqInfo.usFrqValue[slotIndex];
    if( pstImpedanceAglo->uchUpFrqIndex != (BIA_INVALID_IMP_FRQ_INDEX) ){
       unTxFs = pstImpedanceAglo->stFrqInfo.usFrqValue[pstImpedanceAglo->uchUpFrqIndex];  
    }
    pstFrameInfo->pstFlagInfo->punFlag[3] = ( ((pstFrameInfo->pstFlagInfo->punFlag[3])&(~(0xFFFF<<8)))|(unTxFs<<8) ); 
    pstFrameInfo->pstFlagInfo->uchFlagBits |= (1<<3); 
    
    // Flag4:   bit0-3:  BIA_Frame_num : 0-7;   bit8-23: NetIndex
    pstFrameInfo->pstFlagInfo->punFlag[4] = ( ((pstFrameInfo->pstFlagInfo->punFlag[4])&(~(0xFFFF<<8)))|((pstImpedanceAglo->uchUpNetIndex)<<8) ); 
    pstFrameInfo->pstFlagInfo->uchFlagBits |= (1<<4); 
    
//    EXAMPLE_LOG("[FreIndexCompare] guchFreIndex=%d,upFreIndex=%d\r\n", pstImpedanceAglo->uchUpFrqIndex, pstUpdataFlag->uchFrqIndex);
//    EXAMPLE_LOG("[NetIndexCompare] guchNetIndex=%d,upNetIndex=%d\r\n", guchNetIndex, pstUpdataFlag->puchNetIndex);
   
    EXAMPLE_LOG("[BiaUpload] Updata=%d,FrqIndex=%d,F3=%d,I1=%d,Q1=%d,I2=%d,Q2=%d,I3=%d,Q3=%d,I4=%d,Q4=%d,I5=%d,Q5=%d\r\n", 
    (GS32)pstUpdataFlag->uchZbodyFlag,pstImpedanceAglo->uchUpFrqIndex,(GS32)pstFrameInfo->pstFlagInfo->punFlag[3],\
    (GS32)pstFrameInfo->pstGh3xData->punAgcInfo[0],(GS32)pstFrameInfo->pstGh3xData->punAgcInfo[1],\
    (GS32)pstFrameInfo->pstGh3xData->punAgcInfo[2],(GS32)pstFrameInfo->pstGh3xData->punAgcInfo[3],\
    (GS32)pstFrameInfo->pstGh3xData->punAgcInfo[4],(GS32)pstFrameInfo->pstGh3xData->punAgcInfo[5],\
    (GS32)pstFrameInfo->pstGh3xData->punAgcInfo[6],(GS32)pstFrameInfo->pstGh3xData->punAgcInfo[7],\
    (GS32)pstFrameInfo->pstGh3xData->punAgcInfo[8],(GS32)pstFrameInfo->pstGh3xData->punAgcInfo[9]);  

}
#endif
