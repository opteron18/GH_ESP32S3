/*
* BIALib.c - BIA新算法版本
*/

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "goodix_sys_bia.h"
#include "iot_sys_common_lib.h"
#include "iot_sys_def.h"

/* 配置参数 */
#define BIA_FS    125    //每1s
#define BIA_BUFFER_TIME                     (256)                                                        //ms    
#define BIA_BUFFER_SIZE                     ((BIA_FS * BIA_BUFFER_TIME)/1000)                            // 256ms时间数据长度为16
#define BIA_LASTEST_OUTPUT_TIME             (11.3)                                                        // 最长出值时间15s  （11.3+1）*1.218=14.84
#define BIA_EARLIEST_OUTPUT_TIME            (7680)                                                        // 最短出值时间5.12s
#define BIA_ALL_BUFFER_NUM                  (BIA_EARLIEST_OUTPUT_TIME/BIA_BUFFER_TIME)                    // 最短时间数据长度为20个buffer( 1个buffer元素来自16个数据均值（256ms的一级buffer）)
#define BIA_CALI_R0                         (1000)

#define BIA_CHANNL_REF                     0                                                            //测量ref
#define BIA_CHANNL_RX                      1                                                            //测量Rx
#define BIA_CHANNL_LEAD                    2                                                            //测量LEAD 检测

#define BIA_LEAD_CHECKNUM                  3                                                            //3帧
#define BIA_STABLE_CHECKNUM                3                                                            //3BUF


//拟合参数  4微安驱动电流的拟合参数
//#define BIA_FFM_CONST0                    (-13014    )                                                    //C *10000
//#define BIA_FFM_CONST1                    (3633    )                                                    //Ht2/R *10000
//#define BIA_FFM_CONST2                    (3974    )                                                    //Wt *10000
//#define BIA_FFM_CONST3                    (-783    )                                                    //Xc *10000
//#define BIA_FFM_CONST4                    (53404    )                                                    //Sex *10000

////拟合参数  适配2微安驱动电流的拟合参数
#define BIA_FFM_CONST0                    (55361    )                                                    //C *10000
#define BIA_FFM_CONST1                    (3644    )                                                    //Ht2/R *10000
#define BIA_FFM_CONST2                    (3701    )                                                    //Wt *10000
#define BIA_FFM_CONST3                    (-180    )                                                    //Xc *10000
#define BIA_FFM_CONST4                    (39142)                                                    //Sex *10000

//拟合参数  slot模式8微安驱动电流拟合参数
//#define BIA_FFM_CONST0                    (12781)                                                   //C *10000
//#define BIA_FFM_CONST1                    (3950    )                                                   //Ht2/R *10000
//#define BIA_FFM_CONST2                    (3676    )                                                    //Wt *10000
//#define BIA_FFM_CONST3                    (-574    )                                                    //Xc *10000
//#define BIA_FFM_CONST4                    (42745)                                                    //Sex *10000

#define BIA_TBW_CONST0                    (22174)                                                    //C *10000
#define BIA_TBW_CONST1                    (3029    )                                                    //Ht2/Z *10000
#define BIA_TBW_CONST2                    (2191    )                                                    //Wt *10000
#define BIA_TBW_CONST3                    (1087    )                                                    //age *10000
#define BIA_TBW_CONST4                    (6601    )                                                    //Sex *10000

#define BIA_SMA_CONST0                    (51020)                                                    //C *10000
#define BIA_SMA_CONST1                    (4010)                                                    //Ht2/R *10000
#define BIA_SMA_CONST2                    (38250)                                                    //Sex *10000
#define BIA_SMA_CONST3                    (-710)                                                    //Age *10000

                                         
#define BIA_BMR_CONST0                    (3700000)                                                    //C *10000
#define BIA_BMR_CONST1                    (2160    )                                                    //FFM *100

#define BIA_STD_THR_Cali                   (200)                                                    // STD校准阈值卡控 
#define BIA_STD_THR_Rx                     (200)                                                    //STD测量阈值卡控 

#define BIA_STD_LEAD_THR_Q                 (1000)                                                    //STD脱落阈值卡控 

// 第1次提测算法的4微安电流配置的阈值(常温)
//#define BIA_REF_MAX_THR_DEFAULT            210000                                                        //ref 上限
//#define BIA_REF_MIN_THR_DEFAULT            180000                                                        //ref 下限
//#define BIA_RX_MAX_THR_DEFAULT            190000                                                        //rx 上限
//#define BIA_RX_MIN_THR_DEFAULT            100000                                                        //rx 下限   

//// 第2次提测算法2微安电流配置的阈值 (也适用第一次提测的低温数据采集)
#define BIA_REF_MAX_THR_DEFAULT            (110000)                                                    //ref 上限
#define BIA_REF_MIN_THR_DEFAULT            (90000)                                                     //ref 下限
#define BIA_RX_MAX_THR_DEFAULT             (110000)                                                    //rx  上限
#define BIA_RX_MIN_THR_DEFAULT             (45000)                                                     //rx  下限
#define BIA_LEADDET_MAX_THR_DEFAULT        (1200000)                                                   //rx  上限
#define BIA_LEADDET_MIN_THR_DEFAULT        (90000)                                                     //rx  下限

//  8微安电流配置的阈值  
//#define BIA_REF_MAX_THR_DEFAULT            (420000 )                                                    //ref 上限
//#define BIA_REF_MIN_THR_DEFAULT            (380000 )                                                           //ref 下限
//#define BIA_RX_MAX_THR_DEFAULT            (500000 )                                                        //rx  上限
//#define BIA_RX_MIN_THR_DEFAULT            (160000 )                                                    //rx  下限
//#define BIA_LEADDET_MAX_THR_DEFAULT     (3600000)                                                        //Lead 上限
//#define BIA_LEADDET_MIN_THR_DEFAULT        (360000  )                                                    //Lead 下限

#define BIA_DEBUG_ENABLE                0  
 
GU8  guchDropDateFlagAlg = 0;
GU8  guchDropDateCntAlg = 0;
GU8  guchLeadoffCnt = 0;

#define BIA_AlgResult_Num                14  

typedef struct {
  
    GS32 fs;                                        // 原始采样率 
    GS32 MeasureMode;                      // 工作模式：0-手表demo，1-evk只测Zbody，2-EVK测量接触阻抗
    GS32 total_count;                                // 时间戳
    GS32 channel_count;
    GS32 buffer_num;                                // 周期数
    GS32 second;                                    // 秒数
    GS32 BIA_Cali_end_Time;
    //GS32 sampling_level;                            // 采样率档位
    //GS32 ds_rate;                                    // 降采样倍数
    GS32 BIA_stop;                                    //停止采集flg 默认0                           
    GS32 bia_confg_state;                            //采集校准电阻/待测阻抗标 状态 初始0
    GS32 Last_SlotNum;                 //上一帧rawdata的SlotNum 
    GS32 Last_FrameNum;               //上一帧rawdata的FrameNum 
    GS32 CaliCnt;                     //  EVK跑算法功能校准数据计数
    GS32 CaliIDataSum;                //  EVK跑算法功能校准I数据累加和
    GS32 CaliQDataSum;                //  EVK跑算法功能校准Q数据累加和
    ST_BIA_BUFFER_GS32_T BIA_buf_I_cali;
    ST_BIA_BUFFER_GS32_T BIA_buf_Q_cali;            // 缓存BIA均值的buf 周期
    ST_BIA_BUFFER_GS32_T BIA_buf_I_Rx;
    ST_BIA_BUFFER_GS32_T BIA_buf_Q_Rx;                // 缓存BIA均值的buf 周期
    ST_BIA_BUFFER_GS32_T BIA_all_buf_I_cali;
    ST_BIA_BUFFER_GS32_T BIA_all_buf_Q_cali;            // 缓存BIA均值的buf all
    ST_BIA_BUFFER_GS32_T BIA_all_buf_I_Rx;
    ST_BIA_BUFFER_GS32_T BIA_all_buf_Q_Rx;                // 缓存BIA均值的buf all

    GF32 BIA_Icali;            
    GF32 BIA_Qcali;    // cali
    GF32 BIA_IRx;
    GF32 BIA_QRx;    // Rx
    GF32 BIA_I;
    GF32 BIA_Q;    // after cali
    GF32 BIA_X; //    sqrt(2+Q^2 )  阻抗赋值 (单位欧姆)
    GF32 BIA_theta;  // 阻抗相角   单位°

    //personal information
    GS32 bia_in_height;    //身高，外部输入
    GS32 bia_in_weight;    //体重，外部输入
    GS32 bia_in_sex;    //性别，外部输入
    GS32 bia_in_age;    //年龄，外部输入
    GS32 bia_in_athlete;    //是否运动员，外部输入

    //性能参数
    GS32 BIA_snr;                                            // snr
    GS32 BIA_confid;                                        // confidence
    ST_BIA_BUFFER_GS32_T BIA_lead_buf;                        // lead
    GS32 BIA_lead;                                            // lead
    GS32 BIA_stable;                                        // confidence

    GF32 FFM;                                                //output FFM*100        除脂体重
    GF32 BFR;                                                //output BFR*100
    GF32 TBW;                                                //output TBW*100        体水分

    GF32 TBWR;                                                //output TBWR*100        体水分率
    GF32 BMR;                                                //output BMR*100        基础代谢率
    GF32 BMI;                                                //output BMI*100        身体质量指数
    GF32 SMA;                                                //output SMA*100        骨骼肌质量

    GS32 BIA_CALI_R;                                         // 校准电阻的阻抗幅值，单位：0.001 Ohm
    //阈值
    GS32 BIA_ref_max_thr;                                    // ref max
    GS32 BIA_ref_min_thr;                                    // ref min
    GS32 BIA_Rx_max_thr;                                    // rx max
    GS32 BIA_Rx_min_thr;                                    // rx min       
    GS32 BIA_LeadDet_max_thr;                                // LeadDet max
    GS32 BIA_LeadDet_min_thr;                                // LeadDet min  

    GS32 last_res[BIA_AlgResult_Num];

}ST_BIA_INFO;

// 算法版本
static GU8 g_uchBIAVersion[64] = "BIA_v_1.0.0.2";

ST_BIA_INFO *g_stBIA = NULL;

GU8 *BIAAlgoVersion(void) {

    return &(g_uchBIAVersion[0]);
}

GU32 BIAAlgoInit(GS32 PInfo[], GU32 uFs, GU32 uBiaMeasureMode) {

    GU32 nRes = GX_BIA_ALGO_OK;

    if (NULL != g_stBIA) {
        return GX_BIA_ALGO_NULL_PTR;
    }
    g_stBIA = (ST_BIA_INFO *)IOT_SYS_MALLOC(sizeof(ST_BIA_INFO));
    if (g_stBIA == NULL) {
        return GX_BIA_ALGO_OUT_OF_MEM;
    }
    memset(g_stBIA, 0, sizeof(ST_BIA_INFO));

    // 初始化
    g_stBIA->bia_in_height = PInfo[0];  //身高 99·256cm 
    g_stBIA->bia_in_weight = PInfo[1];  //体重 35`256KG*10 
    g_stBIA->bia_in_sex = PInfo[2];     //性别  0 女  1男
    g_stBIA->bia_in_age = PInfo[3];     //年龄   10·127； 
    g_stBIA->bia_in_athlete = PInfo[4]; //是否为运动员  0否  1是 
    g_stBIA->fs = uFs;
    g_stBIA->MeasureMode = uBiaMeasureMode;
    g_stBIA->CaliCnt = 0; 
    g_stBIA->CaliIDataSum = 0;
    g_stBIA->CaliQDataSum = 0;

    //if (fs % BIA_BASE_SAMPLING_RATE != 0) {
    //    return GX_BIA_ALGO_INVALID_PARAM;
    //}

    nRes = InnerBufferInitF32(&g_stBIA->BIA_buf_I_cali, NULL, BIA_BUFFER_SIZE);
    if (nRes == 1) {
        return GX_BIA_ALGO_OUT_OF_MEM;
    }
    nRes = InnerBufferInitF32(&g_stBIA->BIA_buf_Q_cali, NULL, BIA_BUFFER_SIZE);
    if (nRes == 1) {
        return GX_BIA_ALGO_OUT_OF_MEM;
    }    
    nRes = InnerBufferInitF32(&g_stBIA->BIA_buf_I_Rx, NULL, BIA_BUFFER_SIZE);
    if (nRes == 1) {
        return GX_BIA_ALGO_OUT_OF_MEM;
    }
    nRes = InnerBufferInitF32(&g_stBIA->BIA_buf_Q_Rx, NULL, BIA_BUFFER_SIZE);
    if (nRes == 1) {
        return GX_BIA_ALGO_OUT_OF_MEM;
    }
    nRes = InnerBufferInitF32(&g_stBIA->BIA_all_buf_I_cali, NULL, BIA_STABLE_CHECKNUM);
    if (nRes == 1) {
        return GX_BIA_ALGO_OUT_OF_MEM;
    }
    nRes = InnerBufferInitF32(&g_stBIA->BIA_all_buf_Q_cali, NULL, BIA_STABLE_CHECKNUM);
    if (nRes == 1) {
        return GX_BIA_ALGO_OUT_OF_MEM;
    }
    nRes = InnerBufferInitF32(&g_stBIA->BIA_all_buf_I_Rx, NULL, BIA_ALL_BUFFER_NUM);
    if (nRes == 1) {
        return GX_BIA_ALGO_OUT_OF_MEM;
    }
    nRes = InnerBufferInitF32(&g_stBIA->BIA_all_buf_Q_Rx, NULL, BIA_ALL_BUFFER_NUM);
    if (nRes == 1) {
        return GX_BIA_ALGO_OUT_OF_MEM;
    }
    nRes = InnerBufferInitF32(&g_stBIA->BIA_lead_buf, NULL, BIA_LEAD_CHECKNUM);
    if (nRes == 1) {
        return GX_BIA_ALGO_OUT_OF_MEM;
    }

    g_stBIA->total_count = 0;
    g_stBIA->channel_count = 0;
    g_stBIA->BIA_stable = 0;
    g_stBIA->buffer_num = 0;
    g_stBIA->BIA_stop = 0;
    g_stBIA->Last_SlotNum = 0;
    g_stBIA->Last_FrameNum = 0;

    g_stBIA->bia_confg_state = -1;
    g_stBIA->BIA_Cali_end_Time = 0;
    guchDropDateFlagAlg = 0;
    guchDropDateCntAlg = 0;
    guchLeadoffCnt = 0;
#ifdef  _WIN32
    {
        g_stBIA->BIA_ref_max_thr = BIA_REF_MAX_THR_DEFAULT;
        g_stBIA->BIA_ref_min_thr = BIA_REF_MIN_THR_DEFAULT;
        g_stBIA->BIA_Rx_max_thr = BIA_RX_MAX_THR_DEFAULT;
        g_stBIA->BIA_Rx_min_thr = BIA_RX_MIN_THR_DEFAULT;
        g_stBIA->BIA_LeadDet_max_thr = BIA_LEADDET_MAX_THR_DEFAULT;
        g_stBIA->BIA_LeadDet_min_thr = BIA_LEADDET_MIN_THR_DEFAULT;
    }
#else
    {
        g_stBIA->BIA_CALI_R = PInfo[5];
        g_stBIA->BIA_ref_max_thr = PInfo[6];
        g_stBIA->BIA_ref_min_thr = PInfo[7];
        g_stBIA->BIA_Rx_max_thr = PInfo[8];
        g_stBIA->BIA_Rx_min_thr = PInfo[9];
        g_stBIA->BIA_LeadDet_max_thr = PInfo[10];
        g_stBIA->BIA_LeadDet_min_thr = PInfo[11];
    }
#endif
    return GX_BIA_ALGO_OK;
}

GU32 BIAAlgoDeInit(void) {
    //
  InnerBufferDeinitF32(&g_stBIA->BIA_buf_I_cali);
    InnerBufferDeinitF32(&g_stBIA->BIA_buf_Q_cali);
    InnerBufferDeinitF32(&g_stBIA->BIA_buf_I_Rx);
    InnerBufferDeinitF32(&g_stBIA->BIA_buf_Q_Rx);
    InnerBufferDeinitF32(&g_stBIA->BIA_all_buf_I_cali);
    InnerBufferDeinitF32(&g_stBIA->BIA_all_buf_Q_cali);
    InnerBufferDeinitF32(&g_stBIA->BIA_all_buf_I_Rx);
    InnerBufferDeinitF32(&g_stBIA->BIA_all_buf_Q_Rx);

    InnerBufferDeinitF32(&g_stBIA->BIA_lead_buf);
    if (NULL == g_stBIA) {
        return GX_BIA_ALGO_DEINIT_ABORT;
    }
    IOT_SYS_FREE(g_stBIA);
    g_stBIA = NULL;

    return GX_BIA_ALGO_OK;
}


void Goodix_LeadDetection(ST_BIA_INPUT_INFO* rawdata, ST_BIA_INFO* bia_info)
{
    
    GF32 raw_X = BIA_SQRTF((GF32)rawdata->bia_raw_I * (GF32)rawdata->bia_raw_I + (GF32)rawdata->bia_raw_Q * (GF32)rawdata->bia_raw_Q);
    int32_t max_thr, min_thr;
    if (g_stBIA->bia_confg_state == BIA_CHANNL_REF)
    {
        max_thr = bia_info->BIA_ref_max_thr;
        min_thr = bia_info->BIA_ref_min_thr;
    }
    else if (g_stBIA->bia_confg_state == BIA_CHANNL_RX)
    {
        max_thr = bia_info->BIA_Rx_max_thr;
        min_thr = bia_info->BIA_Rx_min_thr;
    }
    else
    {
        max_thr = bia_info->BIA_LeadDet_max_thr;
        min_thr = bia_info->BIA_LeadDet_min_thr;
    }

    // 通过阻抗的大小来判断是否佩戴；可以考虑再单独判断一下I和Q的大小卡控佩戴
    if (raw_X >= min_thr && raw_X <= max_thr)
    {
        InnerBufferAddF32(&bia_info->BIA_lead_buf, BIA_LEAD_ON);
    }
    else
    {
        InnerBufferAddF32(&bia_info->BIA_lead_buf, BIA_LEAD_OFF);
    }    
}

void Goodix_CondidenceCalculate(ST_BIA_INFO* bia_info)
{
    GF32 std_I, std_Q, std_all;

    InnerBufferGetStdF32(&bia_info->BIA_all_buf_I_Rx, &std_I);
    InnerBufferGetStdF32(&bia_info->BIA_all_buf_Q_Rx, &std_Q);
    std_all = BIA_SQRTF(std_I * std_I + std_Q * std_Q);
    if (std_all < 100) {
        bia_info->BIA_confid = 90;
    }
    else if (std_all >= 100 && std_all < 200) {

        bia_info->BIA_confid = 80;    //为提高出值率测试前期置信度大于等于90出值，后期80即可出值          
    }
    else if (std_all >= 200 && std_all < 300) {

        bia_info->BIA_confid = 70;
    }
    else if (std_all >= 300 && std_all < 400) {

        bia_info->BIA_confid = 60;
    }
    else if (std_all >= 400 && std_all < 500) {

        bia_info->BIA_confid = 50;
    }
    else {
        bia_info->BIA_confid = 10;
    }

}

GU32 BIACalculateAlgo(ST_BIA_INPUT_INFO *rawdata, GS32 resultArr[]) {

    g_stBIA->total_count++; // counter
    g_stBIA->second = g_stBIA->total_count / g_stBIA->fs;

    //通道转换
    if (g_stBIA->bia_confg_state - rawdata->bia_confg_flg != 0) //模式转换
    {
        g_stBIA->channel_count = 0; // counter
        g_stBIA->BIA_stable = 0;
        g_stBIA->buffer_num = 0;
        g_stBIA->BIA_stop = 0;
        //g_stBIA->second = 0;    //second
        g_stBIA->bia_confg_state = rawdata->bia_confg_flg; //// bia_confg_flg的值与对应的状态  0:采集校准电阻;1:测量人体阻抗 2：电极脱落检测
    }
    
    // 校准过程： 计算每一帧数据对应的阻抗赋值和相角  (使用参考电阻的阻值校准当前测量值)
    if(g_stBIA->bia_confg_state > BIA_CHANNL_REF)  ////校准结束之后
    {
        GF32 fcal, fBIA_I, fBIA_Q;
        fcal = g_stBIA->BIA_Icali * g_stBIA->BIA_Icali + g_stBIA->BIA_Qcali * g_stBIA->BIA_Qcali;
        if(fcal)
        {
           fBIA_I = g_stBIA->BIA_CALI_R* (g_stBIA->BIA_Icali * rawdata->bia_raw_I + g_stBIA->BIA_Qcali * rawdata->bia_raw_Q) / fcal;
           fBIA_Q = g_stBIA->BIA_CALI_R* (g_stBIA->BIA_Icali * rawdata->bia_raw_Q - g_stBIA->BIA_Qcali * rawdata->bia_raw_I) / fcal;
           g_stBIA->BIA_X = BIA_SQRTF(fBIA_I * fBIA_I + fBIA_Q * fBIA_Q);
           g_stBIA->BIA_theta = BIA_ATAN(fBIA_Q / fBIA_I) * 180 / 3.1415926 * 1000;
           resultArr[BIA_CALI_I_OFFSET] = (GS32)(fBIA_I);
           resultArr[BIA_CALI_Q_OFFSET] = (GS32)(fBIA_Q);
        }
        resultArr[BIA_MAG_OFFSET] = (GS32)(g_stBIA->BIA_X);
        resultArr[BIA_PHASE_OFFSET] = (GS32)(g_stBIA->BIA_theta);
    }

    //是否结束采集
    if (g_stBIA->total_count >= (BIA_LASTEST_OUTPUT_TIME*BIA_FS +1))
    {
        g_stBIA->BIA_stop = 1;
        g_stBIA->channel_count++;
        resultArr[BIA_STOP_OFFSET] = g_stBIA->BIA_stop;
        resultArr[BIA_LEAD_OFFSET] = g_stBIA->BIA_lead;
        memcpy((resultArr + 2), (g_stBIA->last_res + 2), 10 * sizeof(GS32)); //为了结束测试stop和lead标志不能使用上次的
        return (GX_BIA_ALGO_OK);
    }
    
  if(g_stBIA->MeasureMode==1 || g_stBIA->MeasureMode==2) ////EVK阻抗计算功能：1：只测Zbody  2:测量Zbody和接触阻抗    0表示手表demo
    {
     //计算校准Rawdata均值
       if (rawdata->bia_confg_flg == 0)  // 校准
       {
             
       g_stBIA->CaliCnt++;
       g_stBIA->CaliIDataSum += rawdata->bia_raw_I;
       g_stBIA->CaliQDataSum += rawdata->bia_raw_Q;
             //// 7个以上的数据进行校准数据均指计算防止 校准数据太少没法完成校准均值计算
       if(g_stBIA->CaliCnt >= 7)  //// 大于22个数据开始计算均值(配置决定计算均值的数据个数，125Hz时slot运行时间256ms,丢掉6个数据只有24个数据用于计算均值)
       {
          g_stBIA->BIA_Icali = g_stBIA->CaliIDataSum/g_stBIA->CaliCnt;
          g_stBIA->BIA_Qcali = g_stBIA->CaliQDataSum/g_stBIA->CaliCnt;
        }
        if(g_stBIA->CaliCnt >= 200)   //// 防止数据累加溢出
        {
          g_stBIA->CaliCnt = 0;
          g_stBIA->CaliIDataSum = 0;
          g_stBIA->CaliQDataSum = 0;
        }

       }
     return (GX_BIA_ALGO_OK);         
    }

    //slot或者frame切换后第一帧采样数据掉0，需要丢掉前几帧异常数据   
    if ((g_stBIA->Last_SlotNum != rawdata->idx_slot) | (g_stBIA->Last_FrameNum != rawdata->idx_frame)) // slot或者frame切换
    {
        guchDropDateFlagAlg = 1; // counter
    }
    g_stBIA->Last_SlotNum = rawdata->idx_slot;
    g_stBIA->Last_FrameNum = rawdata->idx_frame;
    if (guchDropDateFlagAlg)
    {
        guchDropDateCntAlg++;
        ////LeadDet的时候需要在丢点之后继续调用2次lead检测函数，防止上一个frame的数据影响导致下一个frame的lead错报：因为leadon需要连续3个点的结果满足要求才是leadon,只要有一个点不满足就是leadoff
        if ( (guchDropDateCntAlg <= 2) && BIA_CHANNL_LEAD == rawdata->bia_confg_flg)
        {
            resultArr[BIA_STOP_OFFSET] = g_stBIA->BIA_stop;
            resultArr[BIA_LEAD_OFFSET] = BIA_LEAD_ON;
            memcpy((resultArr + 2), (g_stBIA->last_res + 2), 10 * sizeof(GS32));
            Goodix_LeadDetection(rawdata, g_stBIA); //// 下位机一旦接收到leadoff就认为当前Frame的电极接触有问题，所以先调用3次该函数确保切换frame后的第9帧数据lead状态能准确
            return (GX_BIA_ALGO_OK);
        }
        else
        {
          guchDropDateFlagAlg = 0;
            guchDropDateCntAlg = 0;
        }
            
    }

    //lead检测
    Goodix_LeadDetection(rawdata, g_stBIA);
    GS32 cnt_lead = 0;
    InnerBufferGetSumF32(&g_stBIA->BIA_lead_buf, &cnt_lead);  
    if (cnt_lead >= BIA_LEAD_CHECKNUM)  // lead_buf 长度为3 连续3帧数据在上下阈值之间则leadon, 否则leadoff
    {
        g_stBIA->BIA_lead = 1;
    }
    else
    {
        g_stBIA->BIA_lead = 0;
        g_stBIA->channel_count = 0; // counter
        resultArr[BIA_STOP_OFFSET] = g_stBIA->BIA_stop;
        resultArr[BIA_LEAD_OFFSET] = g_stBIA->BIA_lead;
        memcpy(g_stBIA->last_res, resultArr, 2 * sizeof(GS32)); // lead off之后，上一次的结果更新停止和lead状态
        return (GX_BIA_ALGO_OK);
    }

    //采集数据
    if (rawdata->bia_confg_flg == 0)  // 校准
    {
        InnerBufferAddF32(&g_stBIA->BIA_buf_I_cali, rawdata->bia_raw_I);  // g_stBIA->BIA_buf_I_cali  length = 16
        InnerBufferAddF32(&g_stBIA->BIA_buf_Q_cali, rawdata->bia_raw_Q);
    }
    else if (rawdata->bia_confg_flg == 1)  // 测量人体 
    {
        InnerBufferAddF32(&g_stBIA->BIA_buf_I_Rx, rawdata->bia_raw_I);
        InnerBufferAddF32(&g_stBIA->BIA_buf_Q_Rx, rawdata->bia_raw_Q);
    }
    else
    {
        resultArr[BIA_STOP_OFFSET] = g_stBIA->BIA_stop;
        resultArr[BIA_LEAD_OFFSET] = g_stBIA->BIA_lead;
        return (GX_BIA_ALGO_OK);   ////  脱落检测数据只需要送LeadDetection即可
    }

#if BIA_DEBUG_ENABLE
    IOT_SYS_PRINT("[Input][%s][%d]:rawdataI:%f, rawdataQ:%f.\n", __FUNCTION__, __LINE__, fBIADataI, fBIADataQ);
#endif

    if ((g_stBIA->channel_count % (BIA_BUFFER_SIZE)) == (BIA_BUFFER_SIZE - 1))  // BIA_BUFFER_SIZE = 16    每256毫秒计算一次
    {
         g_stBIA->buffer_num++;
        GF32 fImean, fQmean, fIstd, fQstd;        
        if (g_stBIA->bia_confg_state == BIA_CHANNL_REF)
        {
            InnerBufferGetMeanF32(&g_stBIA->BIA_buf_I_cali, &fImean);  //g_stBIA->BIA_buf_I_cali  length = 16
            InnerBufferGetMeanF32(&g_stBIA->BIA_buf_Q_cali, &fQmean);
            InnerBufferGetStdF32(&g_stBIA->BIA_buf_I_cali, &fIstd);
            InnerBufferGetStdF32(&g_stBIA->BIA_buf_Q_cali, &fQstd);

            if (fIstd <= BIA_STD_THR_Cali && fQstd <= BIA_STD_THR_Cali) // 每16帧数据计算一次(256ms)
            {
                g_stBIA->BIA_stable++ ;
                InnerBufferAddF32(&g_stBIA->BIA_all_buf_I_cali, (GS32)fImean);
                InnerBufferAddF32(&g_stBIA->BIA_all_buf_Q_cali, (GS32)fQmean);    
            }
            else {
                g_stBIA->BIA_stable = 0;
                g_stBIA->channel_count = 0; // counter
                resultArr[BIA_STOP_OFFSET] = g_stBIA->BIA_stop;
                resultArr[BIA_LEAD_OFFSET] = g_stBIA->BIA_lead;
                memcpy(g_stBIA->last_res, resultArr, 12 * sizeof(GS32));
                return (GX_BIA_ALGO_OK);

            }
            if (g_stBIA->BIA_stable >= BIA_STABLE_CHECKNUM) //3次及以上计算周期（256ms）的 I,Q数据std < BIA_STD_THR_Cail，测量稳定结束校准
            {
                g_stBIA->BIA_Cali_end_Time = g_stBIA->second;
                g_stBIA->BIA_stop = 1; //cali stop
                g_stBIA->channel_count++;
                resultArr[BIA_STOP_OFFSET] = g_stBIA->BIA_stop;
                resultArr[BIA_LEAD_OFFSET] = g_stBIA->BIA_lead;
                memcpy(g_stBIA->last_res, resultArr, 12 * sizeof(GS32));
                /////保存校准时的I, Q数据
                InnerBufferGetMeanF32(&g_stBIA->BIA_all_buf_I_cali, &g_stBIA->BIA_Icali);
                InnerBufferGetMeanF32(&g_stBIA->BIA_all_buf_Q_cali, &g_stBIA->BIA_Qcali);
                resultArr[BIA_I_DATA_OFFSET] = (GS32) g_stBIA->BIA_Icali;
                resultArr[BIA_Q_DATA_OFFSET] = (GS32) g_stBIA->BIA_Qcali;
                return (GX_BIA_ALGO_OK);
            }
        }
        if (g_stBIA->bia_confg_state == BIA_CHANNL_RX)
        {
            InnerBufferGetMeanF32(&g_stBIA->BIA_buf_I_Rx, &fImean);
            InnerBufferGetMeanF32(&g_stBIA->BIA_buf_Q_Rx, &fQmean);
            InnerBufferGetStdF32(&g_stBIA->BIA_buf_I_Rx, &fIstd);
            InnerBufferGetStdF32(&g_stBIA->BIA_buf_Q_Rx, &fQstd);

            if (BIA_STD_LEAD_THR_Q < fQstd)
            {
                guchLeadoffCnt++;
            }
            else
            {
                guchLeadoffCnt = 0;
            }
            ///// 人体应该为容性的 所以I和Q符号相反  
            if ((2 < guchLeadoffCnt) || (fQmean / (fImean + 1) > 0))
            {
                g_stBIA->BIA_lead = 0;
                g_stBIA->channel_count++;
                resultArr[BIA_STOP_OFFSET] = g_stBIA->BIA_stop;
                resultArr[BIA_LEAD_OFFSET] = g_stBIA->BIA_lead;
                memcpy(g_stBIA->last_res, resultArr, 2 * sizeof(GS32)); // lead off之后，上一次的结果更新停止和lead状态
                return (GX_BIA_ALGO_OK);
            }

            if (fIstd <= BIA_STD_THR_Rx && fQstd <= BIA_STD_THR_Rx)
            {
                g_stBIA->BIA_stable++;
                InnerBufferAddF32(&g_stBIA->BIA_all_buf_I_Rx, (GS32)fImean);  // BIA_all_buf_I_Rx  length = 20
                InnerBufferAddF32(&g_stBIA->BIA_all_buf_Q_Rx, (GS32)fQmean);
            }
            else {
                //g_stBIA->channel_count = 0; // counter  为了提高出值率，测量期间不再要求连续的STD小于阈值
                g_stBIA->BIA_stable = 0;  // 连续一段时间的数据稳定
                g_stBIA->channel_count++;
                resultArr[BIA_STOP_OFFSET] = g_stBIA->BIA_stop;
                resultArr[BIA_LEAD_OFFSET] = g_stBIA->BIA_lead;
                memcpy((resultArr + 2), (g_stBIA->last_res + 2), 10 * sizeof(GS32));// 使用上一次的出值结果
                memcpy(g_stBIA->last_res, resultArr, 2 * sizeof(GS32)); ////上一次的结果更新停止和lead标志
                return (GX_BIA_ALGO_OK);
            }
        }
        
    }

#if BIA_DEBUG_ENABLE
        IOT_SYS_PRINT("[BIA][%s][%d]:get flag\n", __FUNCTION__, __LINE__);
#endif 
    
        //没有达到最短出值时间                           // 16帧的一级buffer没有处理的话 直接函数返回
    if ((g_stBIA->channel_count < BIA_BUFFER_SIZE * BIA_EARLIEST_OUTPUT_TIME / BIA_BUFFER_TIME -1) || (g_stBIA->channel_count % (BIA_BUFFER_SIZE) != (BIA_BUFFER_SIZE-1)) )
    {
        // 不处理
        memcpy(resultArr, g_stBIA->last_res, 12 * sizeof(GS32));
        g_stBIA->channel_count++; // counter
        resultArr[BIA_STOP_OFFSET] = g_stBIA->BIA_stop;
        resultArr[BIA_LEAD_OFFSET] = g_stBIA->BIA_lead;
        ////实时更新停止和lead标志(不能使用上次的结果)
        return GX_BIA_ALGO_OK;
    }
    //达到最短出值时间                                                          //30个buffer(256ms级)的数据稳定(数据不定时抖动大，为了提高出值率不再要求连续稳定)   // 稳定的I Q数据个数（256ms级）大于30个
    if ((g_stBIA->channel_count >= BIA_BUFFER_SIZE * BIA_EARLIEST_OUTPUT_TIME / BIA_BUFFER_TIME -1) && (g_stBIA->BIA_stable >= BIA_ALL_BUFFER_NUM) && (g_stBIA->BIA_all_buf_I_Rx.num >= BIA_ALL_BUFFER_NUM) && (g_stBIA->bia_confg_state == BIA_CHANNL_RX))
    {
        InnerBufferGetMeanF32(&g_stBIA->BIA_all_buf_I_cali, &g_stBIA->BIA_Icali);
        InnerBufferGetMeanF32(&g_stBIA->BIA_all_buf_Q_cali, &g_stBIA->BIA_Qcali);

        InnerBufferGetMeanF32(&g_stBIA->BIA_all_buf_I_Rx, &g_stBIA->BIA_IRx);
        InnerBufferGetMeanF32(&g_stBIA->BIA_all_buf_Q_Rx, &g_stBIA->BIA_QRx);

        // 校准过程
        GF32 fcal= g_stBIA->BIA_Icali * g_stBIA->BIA_Icali + g_stBIA->BIA_Qcali * g_stBIA->BIA_Qcali;
        #ifdef  _WIN32
        g_stBIA->BIA_I = BIA_CALI_R0 * (g_stBIA->BIA_Icali * g_stBIA->BIA_IRx + g_stBIA->BIA_Qcali * g_stBIA->BIA_QRx) / fcal;
        g_stBIA->BIA_Q = BIA_CALI_R0 * (g_stBIA->BIA_Icali * g_stBIA->BIA_QRx - g_stBIA->BIA_Qcali * g_stBIA->BIA_IRx) / fcal;
        #else
        g_stBIA->BIA_I = (g_stBIA->BIA_CALI_R/1000) * (g_stBIA->BIA_Icali * g_stBIA->BIA_IRx + g_stBIA->BIA_Qcali * g_stBIA->BIA_QRx) / fcal;
        g_stBIA->BIA_Q = (g_stBIA->BIA_CALI_R/1000) * (g_stBIA->BIA_Icali * g_stBIA->BIA_QRx - g_stBIA->BIA_Qcali * g_stBIA->BIA_IRx) / fcal;
        #endif
        g_stBIA->BIA_X = BIA_SQRTF(g_stBIA->BIA_I * g_stBIA->BIA_I + g_stBIA->BIA_Q  * g_stBIA->BIA_Q);

        //拟合过程
        g_stBIA->FFM = (BIA_FFM_CONST0 + BIA_FFM_CONST1 * (GF32)g_stBIA->bia_in_height * (GF32)g_stBIA->bia_in_height / g_stBIA->BIA_I + BIA_FFM_CONST2 * (GF32)g_stBIA->bia_in_weight/10 + BIA_FFM_CONST3 * g_stBIA->BIA_Q + BIA_FFM_CONST4 * (GF32)g_stBIA->bia_in_sex) / 100; //保留两位小数 FFM*100
        g_stBIA->BFR = 10000 - g_stBIA->FFM*100 / ((GF32)g_stBIA->bia_in_weight/10);
        
        g_stBIA->TBW = (BIA_TBW_CONST0 + BIA_TBW_CONST1 * (GF32)g_stBIA->bia_in_height * (GF32)g_stBIA->bia_in_height / g_stBIA->BIA_X + BIA_TBW_CONST2 * (GF32)g_stBIA->bia_in_weight/10 + BIA_TBW_CONST3 * (GF32)g_stBIA->bia_in_age + BIA_TBW_CONST4 * (GF32)g_stBIA->bia_in_sex) / 100; //保留两位小数 FFM*100
        g_stBIA->TBWR = g_stBIA->TBW * 100 / ((GF32)g_stBIA->bia_in_weight / 10);
        g_stBIA->BMR = (BIA_BMR_CONST0 + BIA_BMR_CONST1 * g_stBIA->FFM)/100 ;
        g_stBIA->BMI = (GF32)g_stBIA->bia_in_weight/10 * 10000 / ((GF32)g_stBIA->bia_in_height * (GF32)g_stBIA->bia_in_height /100);
        g_stBIA->SMA = (BIA_SMA_CONST0 + BIA_SMA_CONST1 * (GF32)g_stBIA->bia_in_height * (GF32)g_stBIA->bia_in_height / g_stBIA->BIA_I + BIA_SMA_CONST2 * (GF32)g_stBIA->bia_in_sex + BIA_SMA_CONST3 * (GF32)g_stBIA->bia_in_age) / 100; // 保留两位小数 SMA*100
        
        Goodix_CondidenceCalculate(g_stBIA);
        //result    
    }

    g_stBIA->channel_count++; // counter
    resultArr[BIA_STOP_OFFSET] = g_stBIA->BIA_stop;
    resultArr[BIA_LEAD_OFFSET] = g_stBIA->BIA_lead;

    if ((g_stBIA->BIA_stop == 1) && (g_stBIA->bia_confg_state == BIA_CHANNL_RX)) // 结束测试不再刷新出值结果;防止数据抖动引起置信度变化 App无法出值
    {
        memcpy((resultArr + 2), (g_stBIA->last_res + 2), 10 * sizeof(GS32));// 使用上一次的出值结果  
        return (GX_BIA_ALGO_OK);
    }

    ////为了提高出值率：随着测试时间的进行，结束测试对置信度的要求越来越低
    //if ((g_stBIA->second - g_stBIA->BIA_Cali_end_Time) <= 10)
    //{
    //    if (g_stBIA->BIA_confid >= 90)
    //    {
    //        g_stBIA->BIA_stop = 1;
    //    }
    //}
    //else if ((g_stBIA->second - g_stBIA->BIA_Cali_end_Time) <= 12) //  置信度大于80结束测试
    //{
    //    if (g_stBIA->BIA_confid >= 80)
    //    {
    //        g_stBIA->BIA_stop = 1;
    //    }
    //}
    

    //异常处理: 排除-除脂体重大于体重 （校准或测量异常可能出现）
    if ( (g_stBIA->FFM != 0) && ( (GS32)(g_stBIA->FFM / 10) < g_stBIA->bia_in_weight ) )
    {   
        resultArr[BIA_STOP_OFFSET] = g_stBIA->BIA_stop;
        resultArr[BIA_LEAD_OFFSET] = g_stBIA->BIA_lead;
        resultArr[BIA_CONFID_OFFSET] = g_stBIA->BIA_confid;
        resultArr[BIA_I_DATA_OFFSET] = (GS32)g_stBIA->BIA_I;
        resultArr[BIA_Q_DATA_OFFSET] = (GS32)g_stBIA->BIA_Q;
        resultArr[BIA_FFM_OFFSET] = (GS32)g_stBIA->FFM;
        resultArr[BIA_BFR_OFFSET] = (GS32)g_stBIA->BFR;
        resultArr[BIA_TBW_OFFSET] = (GS32)g_stBIA->TBW;
        resultArr[BIA_TBWR_OFFSET] = (GS32)g_stBIA->TBWR;
        resultArr[BIA_BMR_OFFSET] = (GS32)g_stBIA->BMR;
        resultArr[BIA_BMI_OFFSET] = (GS32)g_stBIA->BMI;
        resultArr[BIA_SMA_OFFSET] = (GS32)g_stBIA->SMA;
        memcpy(g_stBIA->last_res, resultArr, 12 * sizeof(GS32));    
    }


    /*memcpy(resultArr, g_stBIA->last_res, 12 * sizeof(GS32));*/
#if BIA_DEBUG_ENABLE
    IOT_SYS_PRINT("[Output][%s][%d]:%d, %d, %d, %d, %d, %d.\n", __FUNCTION__, __LINE__, nBIAArr[0], nBIAArr[1], nBIAArr[2], nBIAArr[3], nBIAArr[4], nBIAArr[5]);
#endif

    return (GX_BIA_ALGO_OK);
}
