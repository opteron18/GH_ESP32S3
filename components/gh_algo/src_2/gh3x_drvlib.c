#include <stdio.h>
#include "gh3x_demo_config.h"
#include "gh3x_drv_version.h"
#include "gh3x_drv.h"
#include "gh3x_drv_interface.h"
#include "gh3x_drv_control.h"
#include "gh3x_drv_registers.h"
#include "gh3x_fifo_analyze.h"




GS32 GetLen(unsigned int data)
{
    int lenBin = 0;
    unsigned int temp = data;
    while(temp != 0)
    {
        lenBin++;
        temp = (temp >> 1);
    }
    return lenBin;
}
/**
 * @fn     GU32 DRV_DSP_RawdataCalc(GU32 *punRawdata)
 *
 * @brief  calc byte check rawdata
 *
 * @attention
 *
 * @param[in]   punRawdata  one rawdata addr
 * @param[out]  None
 *
 * @return  target rawdata
 */
GU32 DRV_DSP_RawdataCalc(GU32 unRawdata)
{
    #if 1
    GU32 key[4] = {0x6B851EB7, 0x4147AE13, 0x28F5C28F, 0x15C28F5B};
    GU8 uchBitsNum = 0;
    GU32 unAValue = (unRawdata >> 1) & 0x7FFFFF;
    GU32 unIndex = (unRawdata >> 1) & 0x3;
    GU32 unBValue = (unAValue & ~(key[unIndex])) + (~(unAValue) & key[unIndex]);
    while(unBValue)
    {
        unBValue &= unBValue - 1;
        uchBitsNum++;
    }
    unRawdata = (unRawdata&(~(0x1)))|(uchBitsNum%2);
    return unRawdata;
    #else
      
    #endif

}
/**
 * @fn     GU32 DRV_DSP_ChipRawdataCalc(GU32 *punRawdata)
 *
 * @brief  calc byte check rawdata
 *
 * @attention
 *
 * @param[in]   punRawdata  one rawdata addr
 * @param[out]  None
 *
 * @return  target rawdata
 */
GU32 DRV_DSP_ChipRawdataCalc(GU32 unRawdata)
{
    unsigned int key[4] = {0x6B851EB7, 0x4147AE13, 0x28F5C28F, 0x15C28F5B};
    unsigned int orig_data = unRawdata;
    unRawdata = unRawdata >>1;
    for(int i = 0; i < 4; i++)
    {
        key[i] &= 0x7FFFFF;
    }
    int lenBin = GetLen(unRawdata);
    int index = 0;
    if (lenBin < 2){
        index = unRawdata;
    }
    else{
        index = ((unRawdata >> 0) & 0x03);
    }
    unsigned int currkeyInt = key[index];
    unsigned int B  = (currkeyInt ^ unRawdata);
    int lenB = GetLen(B);
    unsigned int Num1inB = 0;
    for(int icountTwo = 0;icountTwo < lenB;icountTwo++)
    {
        if(B & (1<< icountTwo))
            Num1inB = Num1inB + 1;
    }
    unsigned int remainder = Num1inB % 2;
    unRawdata = (orig_data&(~(0x1)))|(remainder);
    return unRawdata;
}

/**
 * @fn     GU8 DRV_DSP_RawdataCheck(GU32 unRawdata)
 *
 * @brief  rawdata check last byte in algo lib
 *
 * @attention   none
 *
 * @param[in]   unRawdata   single rawdata
 * @param[out]  None
 *
 * @return  errcode
 * @retval  0               check success
 * @retval  1               check failed
 */
GU8 DRV_DSP_RawdataCheck(GU32 unRawdata)
{
    GU32 unRawdataTemp = DRV_DSP_RawdataCalc(unRawdata);
    return(unRawdataTemp != unRawdata);
}
/**
 * @fn     void DRV_DSP_RawdataTransform(GU32 *punRawdata)
 *
 * @brief  calc new rawdata
 *
 * @attention
 *
 * @param[in]   punRawdata
 *
 * @return  none
 */
void DRV_DSP_RawdataTransform(GU32 *punRawdata)
{
    GU32 unRawdata = punRawdata[0];
    GU32 unRawdataTemp = DRV_DSP_RawdataCalc(unRawdata);
    *punRawdata = unRawdataTemp;
    SET_VAL_VIA_PTR(punRawdata, unRawdataTemp);
}


void GH3X_CalRawdataBuf(GU8 *puchRawdata, GU16 usRawdataLen)
{
    for (GU16 usIndex = 0; usIndex < usRawdataLen; usIndex += GH3X_FIFO_RAWDATA_SIZE)
    {
        GU16 usTag = ((((GU16)puchRawdata[4 + usIndex]) << 8) | ((GU16)puchRawdata[3 + usIndex]));
        GU8 uchDatatype = ((usTag >> 13) & 0x7);
        if ((uchDatatype == 0) ||
            ((uchDatatype == 4 && ((usTag >> 11) & 0x3) != 1)))
        {
            GU32 usRawdataTemp = GH3X_MAKEUP_DWORD(0,
                                                    puchRawdata[usIndex + 2],
                                                    puchRawdata[usIndex + 1],
                                                    puchRawdata[usIndex + 0]);
            DRV_DSP_RawdataTransform(&usRawdataTemp);
            puchRawdata[usIndex] = (GU8)(usRawdataTemp & 0xFF);
        }
    }
}
