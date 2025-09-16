/**
  ****************************************************************************************
  * @file    gh3x_demo_struct.c
  * @author  GHealth Driver Team
  * @brief   
  ****************************************************************************************
  * @attention  
  #####Copyright (c) 2022 GOODIX
   All rights reserved.
  
  ****************************************************************************************
  */

#include "gh3x_drv.h"
#include "gh3x_demo_config.h"

GU32 gpunFrameRawdata[GH3X_CHANNEL_MAX_NUM] = {0};
GU32 gpunFrameAgcInfo[GH3X_CHANNEL_MAX_NUM] = {0};
#if (__GSENSOR_ENABLE__)
GS32 gusGsenorFrameData[GSENSOR_FRAME_MAX_NUM] = {0};
#endif

#if __GSENSOR_ENABLE__
#if __FUNC_TYPE_ADT_ENABLE__
STGsensorData gstGsData_ADT =
{
    .uchEnableFlag = 0,
    .pusData = gusGsenorFrameData
};
#endif
#if __FUNC_TYPE_HR_ENABLE__
STGsensorData gstGsData_HR =
{
    .uchEnableFlag = 0,
    .pusData = gusGsenorFrameData
};
#endif
#if __FUNC_TYPE_HRV_ENABLE__
STGsensorData gstGsData_HRV =
{
    .uchEnableFlag = 0,
    .pusData = gusGsenorFrameData
};
#endif
#if __FUNC_TYPE_HSM_ENABLE__
STGsensorData gstGsData_HSM =
{
    .uchEnableFlag = 0,
    .pusData = gusGsenorFrameData
};
#endif
#if __FUNC_TYPE_FPBP_ENABLE__
STGsensorData gstGsData_FPBP =
{
    .uchEnableFlag = 0,
    .pusData = gusGsenorFrameData
};
#endif
#if __FUNC_TYPE_PWA_ENABLE__
STGsensorData gstGsData_PWA =
{
    .uchEnableFlag = 0,
    .pusData = gusGsenorFrameData
};
#endif
#if __FUNC_TYPE_SPO2_ENABLE__
STGsensorData gstGsData_SPO2 =
{
    .uchEnableFlag = 0,
    .pusData = gusGsenorFrameData
};
#endif
#if __FUNC_TYPE_ECG_ENABLE__
STGsensorData gstGsData_ECG =
{
    .uchEnableFlag = 0,
    .pusData = gusGsenorFrameData
};
#endif
#if __FUNC_TYPE_PWTT_ENABLE__
STGsensorData gstGsData_PWTT =
{
    .uchEnableFlag = 0,
    .pusData = gusGsenorFrameData
};
#endif
#if __FUNC_TYPE_SOFT_ADT_GREEN_ENABLE__
STGsensorData gstGsData_SOFT_ADT_GREEN =
{
    .uchEnableFlag = 0,
    .pusData = gusGsenorFrameData
};
#endif
#if __FUNC_TYPE_BT_ENABLE__
STGsensorData gstGsData_BT =
{
    .uchEnableFlag = 0,
    .pusData = gusGsenorFrameData
};
#endif
#if __FUNC_TYPE_RESP_ENABLE__
STGsensorData gstGsData_RESP =
{
    .uchEnableFlag = 0,
    .pusData = gusGsenorFrameData
};
#endif
#if __FUNC_TYPE_AF_ENABLE__
STGsensorData gstGsData_AF =
{
    .uchEnableFlag = 0,
    .pusData = gusGsenorFrameData
};
#endif
#if __FUNC_TYPE_TEST1_ENABLE__
STGsensorData gstGsData_TEST1 =
{
    .uchEnableFlag = 0,
    .pusData = gusGsenorFrameData
};
#endif
#if __FUNC_TYPE_TEST2_ENABLE__
STGsensorData gstGsData_TEST2 =
{
    .uchEnableFlag = 0,
    .pusData = gusGsenorFrameData
};
#endif
#if __FUNC_TYPE_SOFT_ADT_IR_ENABLE__
STGsensorData gstGsData_SOFT_ADT_IR =
{
    .uchEnableFlag = 0,
    .pusData = gusGsenorFrameData
};
#endif
#if __FUNC_TYPE_BIA_ENABLE__
STGsensorData gstGsData_BIA =
{
    .uchEnableFlag = 0,
    .pusData = gusGsenorFrameData
};
#endif
#if __FUNC_TYPE_GSR_ENABLE__
STGsensorData gstGsData_GSR =
{
    .uchEnableFlag = 0,
    .pusData = gusGsenorFrameData
};
#endif
#endif
 

#if (__DRIVER_LIB_MODE__ == __DRV_LIB_WITH_ALGO__)
#if __FUNC_TYPE_ADT_ENABLE__
GS32 gpnAlgoResults_ADT[GH3X_ADT_CHNL_NUM] = {0};
STAlgoData gstAlgoData_ADT =
{
    .uchEnableFlag = 0,
    .uchFlag = 0,
    .uchResultNum = 0,
    .unResultBits = 0,
    .pnResults = gpnAlgoResults_ADT
};
#endif
#if __FUNC_TYPE_HR_ENABLE__
GS32 gpnAlgoResults_HR[GH3X_HR_CHNL_NUM] = {0};
STAlgoData gstAlgoData_HR =
{
    .uchEnableFlag = 0,
    .uchFlag = 0,
    .uchResultNum = 0,
    .unResultBits = 0,
    .pnResults = gpnAlgoResults_HR
};
#endif
#if __FUNC_TYPE_HRV_ENABLE__
GS32 gpnAlgoResults_HRV[GH3X_HRV_CHNL_NUM] = {0};
STAlgoData gstAlgoData_HRV =
{
    .uchEnableFlag = 0,
    .uchFlag = 0,
    .uchResultNum = 0,
    .unResultBits = 0,
    .pnResults = gpnAlgoResults_HRV
};
#endif
#if __FUNC_TYPE_HSM_ENABLE__
GS32 gpnAlgoResults_HSM[GH3X_HSM_CHNL_NUM] = {0};
STAlgoData gstAlgoData_HSM =
{
    .uchEnableFlag = 0,
    .uchFlag = 0,
    .uchResultNum = 0,
    .unResultBits = 0,
    .pnResults = gpnAlgoResults_HSM
};
#endif
#if __FUNC_TYPE_FPBP_ENABLE__
GS32 gpnAlgoResults_FPBP[GH3X_FPBP_CHNL_NUM] = {0};
STAlgoData gstAlgoData_FPBP =
{
    .uchEnableFlag = 0,
    .uchFlag = 0,
    .uchResultNum = 0,
    .unResultBits = 0,
    .pnResults = gpnAlgoResults_FPBP
};
#endif
#if __FUNC_TYPE_PWA_ENABLE__
GS32 gpnAlgoResults_PWA[GH3X_PWA_CHNL_NUM] = {0};
STAlgoData gstAlgoData_PWA =
{
    .uchEnableFlag = 0,
    .uchFlag = 0,
    .uchResultNum = 0,
    .unResultBits = 0,
    .pnResults = gpnAlgoResults_PWA
};
#endif
#if __FUNC_TYPE_SPO2_ENABLE__
GS32 gpnAlgoResults_SPO2[GH3X_SPO2_CHNL_NUM] = {0};
STAlgoData gstAlgoData_SPO2 =
{
    .uchEnableFlag = 0,
    .uchFlag = 0,
    .uchResultNum = 0,
    .unResultBits = 0,
    .pnResults = gpnAlgoResults_SPO2
};
#endif
#if __FUNC_TYPE_ECG_ENABLE__
GS32 gpnAlgoResults_ECG[GH3X_ECG_CHNL_NUM] = {0};
STAlgoData gstAlgoData_ECG =
{
    .uchEnableFlag = 0,
    .uchFlag = 0,
    .uchResultNum = 0,
    .unResultBits = 0,
    .pnResults = gpnAlgoResults_ECG
};
#endif
#if __FUNC_TYPE_PWTT_ENABLE__
GS32 gpnAlgoResults_PWTT[GH3X_PWTT_CHNL_NUM] = {0};
STAlgoData gstAlgoData_PWTT =
{
    .uchEnableFlag = 0,
    .uchFlag = 0,
    .uchResultNum = 0,
    .unResultBits = 0,
    .pnResults = gpnAlgoResults_PWTT
};
#endif
#if __FUNC_TYPE_SOFT_ADT_GREEN_ENABLE__
GS32 gpnAlgoResults_SOFT_ADT_GREEN[GH3X_SOFT_ADT_GREEN_CHNL_NUM] = {0};
STAlgoData gstAlgoData_SOFT_ADT_GREEN =
{
    .uchEnableFlag = 0,
    .uchFlag = 0,
    .uchResultNum = 0,
    .unResultBits = 0,
    .pnResults = gpnAlgoResults_SOFT_ADT_GREEN
};
#endif
#if __FUNC_TYPE_BT_ENABLE__
GS32 gpnAlgoResults_BT[GH3X_BT_CHNL_NUM] = {0};
STAlgoData gstAlgoData_BT =
{
    .uchEnableFlag = 0,
    .uchFlag = 0,
    .uchResultNum = 0,
    .unResultBits = 0,
    .pnResults = gpnAlgoResults_BT
};
#endif
#if __FUNC_TYPE_RESP_ENABLE__
GS32 gpnAlgoResults_RESP[GH3X_RESP_CHNL_NUM] = {0};
STAlgoData gstAlgoData_RESP =
{
    .uchEnableFlag = 0,
    .uchFlag = 0,
    .uchResultNum = 0,
    .unResultBits = 0,
    .pnResults = gpnAlgoResults_RESP
};
#endif
#if __FUNC_TYPE_AF_ENABLE__
GS32 gpnAlgoResults_AF[GH3X_AF_CHNL_NUM] = {0};
STAlgoData gstAlgoData_AF =
{
    .uchEnableFlag = 0,
    .uchFlag = 0,
    .uchResultNum = 0,
    .unResultBits = 0,
    .pnResults = gpnAlgoResults_AF
};
#endif
#if __FUNC_TYPE_TEST1_ENABLE__
GS32 gpnAlgoResults_TEST1[GH3X_TEST1_CHNL_NUM] = {0};
STAlgoData gstAlgoData_TEST1 =
{
    .uchEnableFlag = 0,
    .uchFlag = 0,
    .uchResultNum = 0,
    .unResultBits = 0,
    .pnResults = gpnAlgoResults_TEST1
};
#endif
#if __FUNC_TYPE_TEST2_ENABLE__
GS32 gpnAlgoResults_TEST2[GH3X_TEST2_CHNL_NUM] = {0};
STAlgoData gstAlgoData_TEST2 =
{
    .uchEnableFlag = 0,
    .uchFlag = 0,
    .uchResultNum = 0,
    .unResultBits = 0,
    .pnResults = gpnAlgoResults_TEST2
};
#endif
#if __FUNC_TYPE_SOFT_ADT_IR_ENABLE__
GS32 gpnAlgoResults_SOFT_ADT_IR[GH3X_SOFT_ADT_IR_CHNL_NUM] = {0};
STAlgoData gstAlgoData_SOFT_ADT_IR =
{
    .uchEnableFlag = 0,
    .uchFlag = 0,
    .uchResultNum = 0,
    .unResultBits = 0,
    .pnResults = gpnAlgoResults_SOFT_ADT_IR
};
#endif
#if __FUNC_TYPE_BIA_ENABLE__
GS32 gpnAlgoResults_BIA[GH3X_BIA_CHNL_NUM] = {0};
STAlgoData gstAlgoData_BIA =
{
    .uchEnableFlag = 0,
    .uchFlag = 0,
    .uchResultNum = 0,
    .unResultBits = 0,
    .pnResults = gpnAlgoResults_BIA
};
#endif
#if __FUNC_TYPE_GSR_ENABLE__
GS32 gpnAlgoResults_GSR[GH3X_BIA_CHNL_NUM] = {0};
STAlgoData gstAlgoData_GSR =
{
    .uchEnableFlag = 0,
    .uchFlag = 0,
    .uchResultNum = 0,
    .unResultBits = 0,
    .pnResults = gpnAlgoResults_GSR
};
#endif

#endif

#if __FUNC_TYPE_ADT_ENABLE__
GU32 gunFrameCnt_ADT = 0;
GU16 gpusChnlMap_ADT[GH3X_ADT_CHNL_NUM] = {0};
GU32 gpunFrameFlag_ADT[GH3X_FRAME_FLAG_NUM] = {0};
GU32 gpunFrameRawdata_ADT[GH3X_ADT_CHNL_NUM] = {0};
GU32 gpunFrameAgcInfo_ADT[GH3X_ADT_CHNL_NUM] = {0};
STFlagInfo gstFlagInfo_ADT =
{
    .uchFlagBits = 0,
    .punFlag = gpunFrameFlag_ADT
};
STGh3xData gstGh3xData_ADT =
{
    .uchChnlNum = 0,
    .uchMaxChnlNum = GH3X_ADT_CHNL_NUM,
    .uchChnlIndex = 0,
    .usSampleRate = 0,
    .pusChnlMap = gpusChnlMap_ADT,
    .punRawdata = gpunFrameRawdata_ADT,
    .punAgcInfo = gpunFrameAgcInfo_ADT
};
#endif
#if __FUNC_TYPE_HR_ENABLE__
GU32 gunFrameCnt_HR = 0;
GU16 gpusChnlMap_HR[GH3X_HR_CHNL_NUM] = {0};
GU32 gpunFrameFlag_HR[GH3X_FRAME_FLAG_NUM] = {0};
GU32 gpunFrameRawdata_HR[GH3X_HR_CHNL_NUM] = {0};
GU32 gpunFrameAgcInfo_HR[GH3X_HR_CHNL_NUM] = {0};
STFlagInfo gstFlagInfo_HR =
{
    .uchFlagBits = 0,
    .punFlag = gpunFrameFlag_HR
};
STGh3xData gstGh3xData_HR =
{
    .uchChnlNum = 0,
    .uchMaxChnlNum = GH3X_HR_CHNL_NUM,
    .uchChnlIndex = 0,
    .usSampleRate = 0,
    .pusChnlMap = gpusChnlMap_HR,
    .punRawdata = gpunFrameRawdata_HR,
    .punAgcInfo = gpunFrameAgcInfo_HR
};
#endif
#if __FUNC_TYPE_HRV_ENABLE__
GU32 gunFrameCnt_HRV = 0;
GU16 gpusChnlMap_HRV[GH3X_HRV_CHNL_NUM] = {0};
GU32 gpunFrameFlag_HRV[GH3X_FRAME_FLAG_NUM] = {0};
GU32 gpunFrameRawdata_HRV[GH3X_HRV_CHNL_NUM] = {0};
GU32 gpunFrameAgcInfo_HRV[GH3X_HRV_CHNL_NUM] = {0};
STFlagInfo gstFlagInfo_HRV =
{
    .uchFlagBits = 0,
    .punFlag = gpunFrameFlag_HRV
};
STGh3xData gstGh3xData_HRV =
{
    .uchChnlNum = 0,
    .uchMaxChnlNum = GH3X_HRV_CHNL_NUM,
    .uchChnlIndex = 0,
    .usSampleRate = 0,
    .pusChnlMap = gpusChnlMap_HRV,
    .punRawdata = gpunFrameRawdata_HRV,
    .punAgcInfo = gpunFrameAgcInfo_HRV
};
#endif
#if __FUNC_TYPE_HSM_ENABLE__
GU32 gunFrameCnt_HSM = 0;
GU16 gpusChnlMap_HSM[GH3X_HSM_CHNL_NUM] = {0};
GU32 gpunFrameFlag_HSM[GH3X_FRAME_FLAG_NUM] = {0};
GU32 gpunFrameRawdata_HSM[GH3X_HSM_CHNL_NUM] = {0};
GU32 gpunFrameAgcInfo_HSM[GH3X_HSM_CHNL_NUM] = {0};
STFlagInfo gstFlagInfo_HSM =
{
    .uchFlagBits = 0,
    .punFlag = gpunFrameFlag_HSM
};
STGh3xData gstGh3xData_HSM =
{
    .uchChnlNum = 0,
    .uchMaxChnlNum = GH3X_HSM_CHNL_NUM,
    .uchChnlIndex = 0,
    .usSampleRate = 0,
    .pusChnlMap = gpusChnlMap_HSM,
    .punRawdata = gpunFrameRawdata_HSM,
    .punAgcInfo = gpunFrameAgcInfo_HSM
};
#endif
#if __FUNC_TYPE_FPBP_ENABLE__
GU32 gunFrameCnt_FPBP = 0;
GU16 gpusChnlMap_FPBP[GH3X_FPBP_CHNL_NUM] = {0};
GU32 gpunFrameFlag_FPBP[GH3X_FRAME_FLAG_NUM] = {0};
GU32 gpunFrameRawdata_FPBP[GH3X_FPBP_CHNL_NUM] = {0};
GU32 gpunFrameAgcInfo_FPBP[GH3X_FPBP_CHNL_NUM] = {0};
STFlagInfo gstFlagInfo_FPBP =
{
    .uchFlagBits = 0,
    .punFlag = gpunFrameFlag_FPBP
};
STGh3xData gstGh3xData_FPBP =
{
    .uchChnlNum = 0,
    .uchMaxChnlNum = GH3X_FPBP_CHNL_NUM,
    .uchChnlIndex = 0,
    .usSampleRate = 0,
    .pusChnlMap = gpusChnlMap_FPBP,
    .punRawdata = gpunFrameRawdata_FPBP,
    .punAgcInfo = gpunFrameAgcInfo_FPBP
};
#endif
#if __FUNC_TYPE_PWA_ENABLE__
GU32 gunFrameCnt_PWA = 0;
GU16 gpusChnlMap_PWA[GH3X_PWA_CHNL_NUM] = {0};
GU32 gpunFrameFlag_PWA[GH3X_FRAME_FLAG_NUM] = {0};
GU32 gpunFrameRawdata_PWA[GH3X_PWA_CHNL_NUM] = {0};
GU32 gpunFrameAgcInfo_PWA[GH3X_PWA_CHNL_NUM] = {0};
STFlagInfo gstFlagInfo_PWA =
{
    .uchFlagBits = 0,
    .punFlag = gpunFrameFlag_PWA
};
STGh3xData gstGh3xData_PWA =
{
    .uchChnlNum = 0,
    .uchMaxChnlNum = GH3X_PWA_CHNL_NUM,
    .uchChnlIndex = 0,
    .usSampleRate = 0,
    .pusChnlMap = gpusChnlMap_PWA,
    .punRawdata = gpunFrameRawdata_PWA,
    .punAgcInfo = gpunFrameAgcInfo_PWA
};
#endif
#if __FUNC_TYPE_SPO2_ENABLE__
GU32 gunFrameCnt_SPO2 = 0;
GU16 gpusChnlMap_SPO2[GH3X_SPO2_CHNL_NUM] = {0};
GU32 gpunFrameFlag_SPO2[GH3X_FRAME_FLAG_NUM] = {0};
GU32 gpunFrameRawdata_SPO2[GH3X_SPO2_CHNL_NUM] = {0};
GU32 gpunFrameAgcInfo_SPO2[GH3X_SPO2_CHNL_NUM] = {0};
STFlagInfo gstFlagInfo_SPO2 =
{
    .uchFlagBits = 0,
    .punFlag = gpunFrameFlag_SPO2
};
STGh3xData gstGh3xData_SPO2 =
{
    .uchChnlNum = 0,
    .uchMaxChnlNum = GH3X_SPO2_CHNL_NUM,
    .uchChnlIndex = 0,
    .usSampleRate = 0,
    .pusChnlMap = gpusChnlMap_SPO2,
    .punRawdata = gpunFrameRawdata_SPO2,
    .punAgcInfo = gpunFrameAgcInfo_SPO2
};
#endif
#if __FUNC_TYPE_ECG_ENABLE__
GU32 gunFrameCnt_ECG = 0;
GU16 gpusChnlMap_ECG[GH3X_ECG_CHNL_NUM] = {0};
GU32 gpunFrameFlag_ECG[GH3X_FRAME_FLAG_NUM] = {0};
GU32 gpunFrameRawdata_ECG[GH3X_ECG_CHNL_NUM] = {0};
GU32 gpunFrameAgcInfo_ECG[GH3X_ECG_CHNL_NUM] = {0};
STFlagInfo gstFlagInfo_ECG =
{
    .uchFlagBits = 0,
    .punFlag = gpunFrameFlag_ECG
};
STGh3xData gstGh3xData_ECG =
{
    .uchChnlNum = 0,
    .uchMaxChnlNum = GH3X_ECG_CHNL_NUM,
    .uchChnlIndex = 0,
    .usSampleRate = 0,
    .pusChnlMap = gpusChnlMap_ECG,
    .punRawdata = gpunFrameRawdata_ECG,
    .punAgcInfo = GH3X_PTR_NULL
};
#endif
#if __FUNC_TYPE_PWTT_ENABLE__
GU32 gunFrameCnt_PWTT = 0;
GU16 gpusChnlMap_PWTT[GH3X_PWTT_CHNL_NUM] = {0};
GU32 gpunFrameFlag_PWTT[GH3X_FRAME_FLAG_NUM] = {0};
GU32 gpunFrameRawdata_PWTT[GH3X_PWTT_CHNL_NUM] = {0};
GU32 gpunFrameAgcInfo_PWTT[GH3X_PWTT_CHNL_NUM] = {0};
STFlagInfo gstFlagInfo_PWTT =
{
    .uchFlagBits = 0,
    .punFlag = gpunFrameFlag_PWTT
};
STGh3xData gstGh3xData_PWTT =
{
    .uchChnlNum = 0,
    .uchMaxChnlNum = GH3X_PWTT_CHNL_NUM,
    .uchChnlIndex = 0,
    .usSampleRate = 0,
    .pusChnlMap = gpusChnlMap_PWTT,
    .punRawdata = gpunFrameRawdata_PWTT,
    .punAgcInfo = gpunFrameAgcInfo_PWTT
};
#endif
#if __FUNC_TYPE_SOFT_ADT_GREEN_ENABLE__
GU32 gunFrameCnt_SOFT_ADT_GREEN = 0;
GU16 gpusChnlMap_SOFT_ADT_GREEN[GH3X_SOFT_ADT_GREEN_CHNL_NUM] = {0};
GU32 gpunFrameFlag_SOFT_ADT_GREEN[GH3X_FRAME_FLAG_NUM] = {0};
GU32 gpunFrameRawdata_SOFT_ADT_GREEN[GH3X_SOFT_ADT_GREEN_CHNL_NUM] = {0};
GU32 gpunFrameAgcInfo_SOFT_ADT_GREEN[GH3X_SOFT_ADT_GREEN_CHNL_NUM] = {0};
STFlagInfo gstFlagInfo_SOFT_ADT_GREEN =
{
    .uchFlagBits = 0,
    .punFlag = gpunFrameFlag_SOFT_ADT_GREEN
};
STGh3xData gstGh3xData_SOFT_ADT_GREEN =
{
    .uchChnlNum = 0,
    .uchMaxChnlNum = GH3X_SOFT_ADT_GREEN_CHNL_NUM,
    .uchChnlIndex = 0,
    .usSampleRate = 0,
    .pusChnlMap = gpusChnlMap_SOFT_ADT_GREEN,
    .punRawdata = gpunFrameRawdata_SOFT_ADT_GREEN,
    .punAgcInfo = gpunFrameAgcInfo_SOFT_ADT_GREEN
};
#endif
#if __FUNC_TYPE_BT_ENABLE__
GU32 gunFrameCnt_BT = 0;
GU16 gpusChnlMap_BT[GH3X_BT_CHNL_NUM] = {0};
GU32 gpunFrameFlag_BT[GH3X_FRAME_FLAG_NUM] = {0};
GU32 gpunFrameRawdata_BT[GH3X_BT_CHNL_NUM] = {0};
GU32 gpunFrameAgcInfo_BT[GH3X_BT_CHNL_NUM] = {0};
STFlagInfo gstFlagInfo_BT =
{
    .uchFlagBits = 0,
    .punFlag = gpunFrameFlag_BT
};
STGh3xData gstGh3xData_BT =
{
    .uchChnlNum = 0,
    .uchMaxChnlNum = GH3X_BT_CHNL_NUM,
    .uchChnlIndex = 0,
    .usSampleRate = 0,
    .pusChnlMap = gpusChnlMap_BT,
    .punRawdata = gpunFrameRawdata_BT,
    .punAgcInfo = GH3X_PTR_NULL
};
#endif
#if __FUNC_TYPE_RESP_ENABLE__
GU32 gunFrameCnt_RESP = 0;
GU16 gpusChnlMap_RESP[GH3X_RESP_CHNL_NUM] = {0};
GU32 gpunFrameFlag_RESP[GH3X_FRAME_FLAG_NUM] = {0};
GU32 gpunFrameRawdata_RESP[GH3X_RESP_CHNL_NUM] = {0};
GU32 gpunFrameAgcInfo_RESP[GH3X_RESP_CHNL_NUM] = {0};
STFlagInfo gstFlagInfo_RESP =
{
    .uchFlagBits = 0,
    .punFlag = gpunFrameFlag_RESP
};
STGh3xData gstGh3xData_RESP =
{
    .uchChnlNum = 0,
    .uchMaxChnlNum = GH3X_RESP_CHNL_NUM,
    .uchChnlIndex = 0,
    .usSampleRate = 0,
    .pusChnlMap = gpusChnlMap_RESP,
    .punRawdata = gpunFrameRawdata_RESP,
    .punAgcInfo = gpunFrameAgcInfo_RESP
};
#endif
#if __FUNC_TYPE_AF_ENABLE__
GU32 gunFrameCnt_AF = 0;
GU16 gpusChnlMap_AF[GH3X_AF_CHNL_NUM] = {0};
GU32 gpunFrameFlag_AF[GH3X_FRAME_FLAG_NUM] = {0};
GU32 gpunFrameRawdata_AF[GH3X_AF_CHNL_NUM] = {0};
GU32 gpunFrameAgcInfo_AF[GH3X_AF_CHNL_NUM] = {0};
STFlagInfo gstFlagInfo_AF =
{
    .uchFlagBits = 0,
    .punFlag = gpunFrameFlag_AF
};
STGh3xData gstGh3xData_AF =
{
    .uchChnlNum = 0,
    .uchMaxChnlNum = GH3X_AF_CHNL_NUM,
    .uchChnlIndex = 0,
    .usSampleRate = 0,
    .pusChnlMap = gpusChnlMap_AF,
    .punRawdata = gpunFrameRawdata_AF,
    .punAgcInfo = gpunFrameAgcInfo_AF
};
#endif
#if __FUNC_TYPE_TEST1_ENABLE__
GU32 gunFrameCnt_TEST1 = 0;
GU16 gpusChnlMap_TEST1[GH3X_AF_CHNL_NUM] = {0};
GU32 gpunFrameFlag_TEST1[GH3X_FRAME_FLAG_NUM] = {0};
GU32 gpunFrameRawdata_TEST1[GH3X_TEST1_CHNL_NUM] = {0};
GU32 gpunFrameAgcInfo_TEST1[GH3X_TEST1_CHNL_NUM] = {0};
STFlagInfo gstFlagInfo_TEST1 =
{
    .uchFlagBits = 0,
    .punFlag = gpunFrameFlag_TEST1
};
STGh3xData gstGh3xData_TEST1 =
{
    .uchChnlNum = 0,
    .uchMaxChnlNum = GH3X_AF_CHNL_NUM,
    .uchChnlIndex = 0,
    .usSampleRate = 0,
    .pusChnlMap = gpusChnlMap_TEST1,
    .punRawdata = gpunFrameRawdata_TEST1,
    .punAgcInfo = gpunFrameAgcInfo_TEST1
};
#endif
#if __FUNC_TYPE_TEST2_ENABLE__
GU32 gunFrameCnt_TEST2 = 0;
GU16 gpusChnlMap_TEST2[GH3X_TEST2_CHNL_NUM] = {0};
GU32 gpunFrameFlag_TEST2[GH3X_FRAME_FLAG_NUM] = {0};
GU32 gpunFrameRawdata_TEST2[GH3X_TEST2_CHNL_NUM] = {0};
GU32 gpunFrameAgcInfo_TEST2[GH3X_TEST2_CHNL_NUM] = {0};
STFlagInfo gstFlagInfo_TEST2 =
{
    .uchFlagBits = 0,
    .punFlag = gpunFrameFlag_TEST2
};
STGh3xData gstGh3xData_TEST2 =
{
    .uchChnlNum = 0,
    .uchMaxChnlNum = GH3X_TEST2_CHNL_NUM,
    .uchChnlIndex = 0,
    .usSampleRate = 0,
    .pusChnlMap = gpusChnlMap_TEST2,
    .punRawdata = gpunFrameRawdata_TEST2,
    .punAgcInfo = gpunFrameRawdata_TEST2
};
#endif
#if __FUNC_TYPE_SOFT_ADT_IR_ENABLE__
GU32 gunFrameCnt_SOFT_ADT_IR = 0;
GU16 gpusChnlMap_SOFT_ADT_IR[GH3X_SOFT_ADT_IR_CHNL_NUM] = {0};
GU32 gpunFrameFlag_SOFT_ADT_IR[GH3X_FRAME_FLAG_NUM] = {0};
GU32 gpunFrameRawdata_SOFT_ADT_IR[GH3X_SOFT_ADT_IR_CHNL_NUM] = {0};
GU32 gpunFrameAgcInfo_SOFT_ADT_IR[GH3X_SOFT_ADT_IR_CHNL_NUM] = {0};
STFlagInfo gstFlagInfo_SOFT_ADT_IR =
{
    .uchFlagBits = 0,
    .punFlag = gpunFrameFlag_SOFT_ADT_IR
};
STGh3xData gstGh3xData_SOFT_ADT_IR =
{
    .uchChnlNum = 0,
    .uchMaxChnlNum = GH3X_SOFT_ADT_IR_CHNL_NUM,
    .uchChnlIndex = 0,
    .usSampleRate = 0,
    .pusChnlMap = gpusChnlMap_SOFT_ADT_IR,
    .punRawdata = gpunFrameRawdata_SOFT_ADT_IR,
    .punAgcInfo = gpunFrameAgcInfo_SOFT_ADT_IR
};
#endif
#if __FUNC_TYPE_BIA_ENABLE__
GU32 gunFrameCnt_BIA = 0;
GU16 gpusChnlMap_BIA[GH3X_BIA_CHNL_NUM] = {0};
GU32 gpunFrameFlag_BIA[GH3X_FRAME_FLAG_NUM] = {0};
GU32 gpunFrameRawdata_BIA[GH3X_BIA_CHNL_NUM] = {0};
GU32 gpunFrameAgcInfo_BIA[GH3X_BIA_CHNL_NUM] = {0};
STFlagInfo gstFlagInfo_BIA =
{
    .uchFlagBits = 0,
    .punFlag = gpunFrameFlag_BIA
};
STGh3xData gstGh3xData_BIA =
{
    .uchChnlNum = 0,
    .uchMaxChnlNum = GH3X_BIA_CHNL_NUM,
    .uchChnlIndex = 0,
    .usSampleRate = 0,
    .pusChnlMap = gpusChnlMap_BIA,
    .punRawdata = gpunFrameRawdata_BIA,
    .punAgcInfo = gpunFrameAgcInfo_BIA
};
#endif
#if __FUNC_TYPE_GSR_ENABLE__
GU32 gunFrameCnt_GSR = 0;
GU16 gpusChnlMap_GSR[GH3X_GSR_CHNL_NUM] = {0};
GU32 gpunFrameFlag_GSR[GH3X_FRAME_FLAG_NUM] = {0};
GU32 gpunFrameRawdata_GSR[GH3X_GSR_CHNL_NUM] = {0};
GU32 gpunFrameAgcInfo_GSR[GH3X_GSR_CHNL_NUM] = {0};
STFlagInfo gstFlagInfo_GSR =
{
    .uchFlagBits = 0,
    .punFlag = gpunFrameFlag_GSR
};
STGh3xData gstGh3xData_GSR =
{
    .uchChnlNum = 0,
    .uchMaxChnlNum = GH3X_GSR_CHNL_NUM,
    .uchChnlIndex = 0,
    .usSampleRate = 0,
    .pusChnlMap = gpusChnlMap_GSR,
    .punRawdata = gpunFrameRawdata_GSR,
    .punAgcInfo = GH3X_PTR_NULL
};
STReSampleInfo gstReSampleInfo_GSR =
{
    .fRawdataFs = 3.90625,
    .fResampleFrameRest = 0,//init to 0 when start sample
    .sResampleFrameCnt = 0, //init to 0 when get fifo data
    .sRawdataFrameCnt = 0,  //init to 0 when get fifo data
    .sRawdataFrameNum = 0,
    .sResampleFrameNum =0
    
};
#endif

#if __FUNC_TYPE_ADT_ENABLE__
const STFrameInfo gstFrameInfo_ADT =
{
    .uchFuncID = GH3X_FUNC_OFFSET_ADT,
    .punFrameCnt = &gunFrameCnt_ADT,
    .pstFlagInfo = &gstFlagInfo_ADT,
    .pstGh3xData = &gstGh3xData_ADT,
#if __GSENSOR_ENABLE__
    .pstGsData = &gstGsData_ADT,
#endif
#if (__DRIVER_LIB_MODE__ == __DRV_LIB_WITH_ALGO__)
    .pstAlgoData= &gstAlgoData_ADT
#endif
};
#endif
#if __FUNC_TYPE_HR_ENABLE__
const STFrameInfo gstFrameInfo_HR =
{
    .uchFuncID = GH3X_FUNC_OFFSET_HR,
    .punFrameCnt = &gunFrameCnt_HR,
    .pstFlagInfo = &gstFlagInfo_HR,
    .pstGh3xData = &gstGh3xData_HR,
#if __GSENSOR_ENABLE__
    .pstGsData = &gstGsData_HR,
#endif
#if (__DRIVER_LIB_MODE__ == __DRV_LIB_WITH_ALGO__)
    .pstAlgoData= &gstAlgoData_HR
#endif
};
#endif
#if __FUNC_TYPE_HRV_ENABLE__
const STFrameInfo gstFrameInfo_HRV =
{
    .uchFuncID = GH3X_FUNC_OFFSET_HRV,
    .punFrameCnt = &gunFrameCnt_HRV,
    .pstFlagInfo = &gstFlagInfo_HRV,
    .pstGh3xData = &gstGh3xData_HRV,
#if __GSENSOR_ENABLE__
    .pstGsData = &gstGsData_HRV,
#endif
#if (__DRIVER_LIB_MODE__ == __DRV_LIB_WITH_ALGO__)
    .pstAlgoData= &gstAlgoData_HRV
#endif
};
#endif
#if __FUNC_TYPE_HSM_ENABLE__
const STFrameInfo gstFrameInfo_HSM =
{
    .uchFuncID = GH3X_FUNC_OFFSET_HSM,
    .punFrameCnt = &gunFrameCnt_HSM,
    .pstFlagInfo = &gstFlagInfo_HSM,
    .pstGh3xData = &gstGh3xData_HSM,
#if __GSENSOR_ENABLE__
    .pstGsData = &gstGsData_HSM,
#endif
#if (__DRIVER_LIB_MODE__ == __DRV_LIB_WITH_ALGO__)
    .pstAlgoData= &gstAlgoData_HSM
#endif
};
#endif
#if __FUNC_TYPE_FPBP_ENABLE__
const STFrameInfo gstFrameInfo_FPBP =
{
    .uchFuncID = GH3X_FUNC_OFFSET_FPBP,
    .punFrameCnt = &gunFrameCnt_FPBP,
    .pstFlagInfo = &gstFlagInfo_FPBP,
    .pstGh3xData = &gstGh3xData_FPBP,
#if __GSENSOR_ENABLE__
    .pstGsData = &gstGsData_FPBP,
#endif
#if (__DRIVER_LIB_MODE__ == __DRV_LIB_WITH_ALGO__)
    .pstAlgoData= &gstAlgoData_FPBP
#endif
};
#endif
#if __FUNC_TYPE_PWA_ENABLE__
const STFrameInfo gstFrameInfo_PWA =
{
    .uchFuncID = GH3X_FUNC_OFFSET_PWA,
    .punFrameCnt = &gunFrameCnt_PWA,
    .pstFlagInfo = &gstFlagInfo_PWA,
    .pstGh3xData = &gstGh3xData_PWA,
#if __GSENSOR_ENABLE__
    .pstGsData = &gstGsData_PWA,
#endif
#if (__DRIVER_LIB_MODE__ == __DRV_LIB_WITH_ALGO__)
    .pstAlgoData= &gstAlgoData_PWA
#endif
};
#endif
#if __FUNC_TYPE_SPO2_ENABLE__
const STFrameInfo gstFrameInfo_SPO2 =
{
    .uchFuncID = GH3X_FUNC_OFFSET_SPO2,
    .punFrameCnt = &gunFrameCnt_SPO2,
    .pstFlagInfo = &gstFlagInfo_SPO2,
    .pstGh3xData = &gstGh3xData_SPO2,
#if __GSENSOR_ENABLE__
    .pstGsData = &gstGsData_SPO2,
#endif
#if (__DRIVER_LIB_MODE__ == __DRV_LIB_WITH_ALGO__)
    .pstAlgoData= &gstAlgoData_SPO2
#endif
};
#endif
#if __FUNC_TYPE_ECG_ENABLE__
const STFrameInfo gstFrameInfo_ECG =
{
    .uchFuncID = GH3X_FUNC_OFFSET_ECG,
    .punFrameCnt = &gunFrameCnt_ECG,
    .pstFlagInfo = &gstFlagInfo_ECG,
    .pstGh3xData = &gstGh3xData_ECG,
#if __GSENSOR_ENABLE__
    .pstGsData = &gstGsData_ECG,
#endif
#if (__DRIVER_LIB_MODE__ == __DRV_LIB_WITH_ALGO__)
    .pstAlgoData= &gstAlgoData_ECG
#endif
};
#endif
#if __FUNC_TYPE_PWTT_ENABLE__
const STFrameInfo gstFrameInfo_PWTT =
{
    .uchFuncID = GH3X_FUNC_OFFSET_PWTT,
    .punFrameCnt = &gunFrameCnt_PWTT,
    .pstFlagInfo = &gstFlagInfo_PWTT,
    .pstGh3xData = &gstGh3xData_PWTT,
#if __GSENSOR_ENABLE__
    .pstGsData = &gstGsData_PWTT,
#endif
#if (__DRIVER_LIB_MODE__ == __DRV_LIB_WITH_ALGO__)
    .pstAlgoData= &gstAlgoData_PWTT
#endif
};
#endif
#if __FUNC_TYPE_SOFT_ADT_GREEN_ENABLE__
const STFrameInfo gstFrameInfo_SOFT_ADT_GREEN =
{
    .uchFuncID = GH3X_FUNC_OFFSET_SOFT_ADT_GREEN,
    .punFrameCnt = &gunFrameCnt_SOFT_ADT_GREEN,
    .pstFlagInfo = &gstFlagInfo_SOFT_ADT_GREEN,
    .pstGh3xData = &gstGh3xData_SOFT_ADT_GREEN,
#if __GSENSOR_ENABLE__
    .pstGsData = &gstGsData_SOFT_ADT_GREEN,
#endif
#if (__DRIVER_LIB_MODE__ == __DRV_LIB_WITH_ALGO__)
    .pstAlgoData= &gstAlgoData_SOFT_ADT_GREEN
#endif
};
#endif
#if __FUNC_TYPE_BT_ENABLE__
const STFrameInfo gstFrameInfo_BT =
{
    .uchFuncID = GH3X_FUNC_OFFSET_BT,
    .punFrameCnt = &gunFrameCnt_BT,
    .pstFlagInfo = &gstFlagInfo_BT,
    .pstGh3xData = &gstGh3xData_BT,
#if __GSENSOR_ENABLE__
    .pstGsData = &gstGsData_BT,
#endif
#if (__DRIVER_LIB_MODE__ == __DRV_LIB_WITH_ALGO__)
    .pstAlgoData= &gstAlgoData_BT
#endif
};
#endif
#if __FUNC_TYPE_RESP_ENABLE__
const STFrameInfo gstFrameInfo_RESP =
{
    .uchFuncID = GH3X_FUNC_OFFSET_RESP,
    .punFrameCnt = &gunFrameCnt_RESP,
    .pstFlagInfo = &gstFlagInfo_RESP,
    .pstGh3xData = &gstGh3xData_RESP,
#if __GSENSOR_ENABLE__
    .pstGsData = &gstGsData_RESP,
#endif
#if (__DRIVER_LIB_MODE__ == __DRV_LIB_WITH_ALGO__)
    .pstAlgoData= &gstAlgoData_RESP
#endif
};
#endif
#if __FUNC_TYPE_AF_ENABLE__
const STFrameInfo gstFrameInfo_AF =
{
    .uchFuncID = GH3X_FUNC_OFFSET_AF,
    .punFrameCnt = &gunFrameCnt_AF,
    .pstFlagInfo = &gstFlagInfo_AF,
    .pstGh3xData = &gstGh3xData_AF,
#if __GSENSOR_ENABLE__
    .pstGsData = &gstGsData_AF,
#endif
#if (__DRIVER_LIB_MODE__ == __DRV_LIB_WITH_ALGO__)
    .pstAlgoData= &gstAlgoData_AF
#endif
};
#endif
#if __FUNC_TYPE_TEST1_ENABLE__
const STFrameInfo gstFrameInfo_TEST1 =
{
    .uchFuncID = GH3X_FUNC_OFFSET_TEST1,
    .punFrameCnt = &gunFrameCnt_TEST1,
    .pstFlagInfo = &gstFlagInfo_TEST1,
    .pstGh3xData = &gstGh3xData_TEST1,
#if __GSENSOR_ENABLE__
    .pstGsData = &gstGsData_TEST1,
#endif
#if (__DRIVER_LIB_MODE__ == __DRV_LIB_WITH_ALGO__)
    .pstAlgoData= &gstAlgoData_TEST1
#endif
};
#endif
#if __FUNC_TYPE_TEST2_ENABLE__
const STFrameInfo gstFrameInfo_TEST2 =
{
    .uchFuncID = GH3X_FUNC_OFFSET_TEST2,
    .punFrameCnt = &gunFrameCnt_TEST2,
    .pstFlagInfo = &gstFlagInfo_TEST2,
    .pstGh3xData = &gstGh3xData_TEST2,
#if __GSENSOR_ENABLE__
    .pstGsData = &gstGsData_TEST2,
#endif
#if (__DRIVER_LIB_MODE__ == __DRV_LIB_WITH_ALGO__)
    .pstAlgoData= &gstAlgoData_TEST2
#endif
};
#endif
#if __FUNC_TYPE_SOFT_ADT_IR_ENABLE__
const STFrameInfo gstFrameInfo_SOFT_ADT_IR =
{
    .uchFuncID = GH3X_FUNC_OFFSET_SOFT_ADT_IR,
    .punFrameCnt = &gunFrameCnt_SOFT_ADT_IR,
    .pstFlagInfo = &gstFlagInfo_SOFT_ADT_IR,
    .pstGh3xData = &gstGh3xData_SOFT_ADT_IR,
#if __GSENSOR_ENABLE__
    .pstGsData = &gstGsData_SOFT_ADT_IR,
#endif
#if (__DRIVER_LIB_MODE__ == __DRV_LIB_WITH_ALGO__)
    .pstAlgoData= &gstAlgoData_SOFT_ADT_IR
#endif
};
#endif
#if __FUNC_TYPE_BIA_ENABLE__
const STFrameInfo gstFrameInfo_BIA =
{
    .uchFuncID = GH3X_FUNC_OFFSET_BIA,
    .punFrameCnt = &gunFrameCnt_BIA,
    .pstFlagInfo = &gstFlagInfo_BIA,
    .pstGh3xData = &gstGh3xData_BIA,
#if __GSENSOR_ENABLE__
    .pstGsData = &gstGsData_BIA,
#endif
#if (__DRIVER_LIB_MODE__ == __DRV_LIB_WITH_ALGO__)
    .pstAlgoData= &gstAlgoData_BIA
#endif
};
#endif
#if __FUNC_TYPE_GSR_ENABLE__
const STFrameInfo gstFrameInfo_GSR =
{
    .uchFuncID = GH3X_FUNC_OFFSET_GSR,
    .punFrameCnt = &gunFrameCnt_GSR,
    .pstFlagInfo = &gstFlagInfo_GSR,
    .pstGh3xData = &gstGh3xData_GSR,
#if __GSENSOR_ENABLE__
    .pstGsData = &gstGsData_GSR,
#endif
#if (__DRIVER_LIB_MODE__ == __DRV_LIB_WITH_ALGO__)
    .pstAlgoData= &gstAlgoData_GSR,
#endif
    .pstReSampleInfo = &gstReSampleInfo_GSR,
};
#endif

const STFrameInfo * const gpstFrameInfo[GH3X_FUNC_OFFSET_MAX] = 
{
//adt
#if __FUNC_TYPE_ADT_ENABLE__
    &gstFrameInfo_ADT,
#else
    0,
#endif
//hr
#if __FUNC_TYPE_HR_ENABLE__
    &gstFrameInfo_HR,
#else
    0,
#endif
//hrv
#if __FUNC_TYPE_HRV_ENABLE__
    &gstFrameInfo_HRV,
#else
    0,
#endif
//hsm
#if __FUNC_TYPE_HSM_ENABLE__
    &gstFrameInfo_HSM,
#else
    0,
#endif
//fpbp
#if __FUNC_TYPE_FPBP_ENABLE__
    &gstFrameInfo_FPBP,
#else
    0,
#endif
//pwa
#if __FUNC_TYPE_PWA_ENABLE__
    &gstFrameInfo_PWA,
#else
    0,
#endif
//spo2
#if __FUNC_TYPE_SPO2_ENABLE__
    &gstFrameInfo_SPO2,
#else
    0,
#endif
//ecg
#if __FUNC_TYPE_ECG_ENABLE__
    &gstFrameInfo_ECG,
#else
    0,
#endif
//pwtt
#if __FUNC_TYPE_PWTT_ENABLE__
    &gstFrameInfo_PWTT,
#else
    0,
#endif
//soft_adt_green
#if __FUNC_TYPE_SOFT_ADT_GREEN_ENABLE__
    &gstFrameInfo_SOFT_ADT_GREEN,
#else
    0,
#endif
//bt
#if __FUNC_TYPE_BT_ENABLE__
    &gstFrameInfo_BT,
#else
    0,
#endif
//resp
#if __FUNC_TYPE_RESP_ENABLE__
    &gstFrameInfo_RESP,
#else
    0,
#endif
//af
#if __FUNC_TYPE_AF_ENABLE__
    &gstFrameInfo_AF,
#else
    0,
#endif
//test1
#if __FUNC_TYPE_TEST1_ENABLE__
    &gstFrameInfo_TEST1,
#else
    0,
#endif
//test2
#if __FUNC_TYPE_TEST2_ENABLE__
    &gstFrameInfo_TEST2,
#else
    0,
#endif
//soft_adt_ir
#if __FUNC_TYPE_SOFT_ADT_IR_ENABLE__
    &gstFrameInfo_SOFT_ADT_IR,
#else
    0,
#endif
//bia
#if __FUNC_TYPE_BIA_ENABLE__
    &gstFrameInfo_BIA,
#else
    0,
#endif
//GSR
#if __FUNC_TYPE_GSR_ENABLE__
    &gstFrameInfo_GSR,
#else
    0,
#endif

 
};

const STFrameInfo * GetstFrameInfo(GU8 uchFuncOffset)
{
    return gpstFrameInfo[uchFuncOffset];
}
