/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 * 
 * @file    gh3x_demo_strategy.c
 * 
 * @brief   
 * 
 * @author  
 * 
 */

/* includes */
#include "stdint.h"
#include "string.h"
#include "goodix_log.h"
#include "gh3x_demo_algo_config.h"
#include "gh3x_demo_algo_call.h"


GU8 guchPreHrValue = 0;


void GH3XOutputValueStrategyInit(GU32 unFunctionID)
{
#if __GH3X_HR_OUTPUT_VALUE_STRATEGY_EN__
    if((GH3X_FUNCTION_HR & unFunctionID) == GH3X_FUNCTION_HR)
    {
        guchPreHrValue = 0;
    }
#endif
}


#define GH3X_DIFF_ABS(x,y)  (x > y ? (x - y):(y - x))
#if __GH3X_HR_OUTPUT_VALUE_STRATEGY_EN__
void GH3XHrOutputValueStrategyPro(STAlgoData * pstAlgoResult, GU32 lubFrameId)
{
    GU8 uchCurrentOriginalHrValue = (GU8)(pstAlgoResult->pnResults[0]);
    GU8 uchConf = (GU8)(pstAlgoResult->pnResults[1]);
    GU8 uchNewHrValue = uchCurrentOriginalHrValue;
    
    if(0 == guchPreHrValue)
    {
        guchPreHrValue = uchCurrentOriginalHrValue;
    }
    else
    {
        if((GH3X_DIFF_ABS(guchPreHrValue,uchCurrentOriginalHrValue) > 50) &&(uchConf <= 50))   //there is big difference between pre hr and current orignal hr AND confidence is low
        {
            //smooth vaule
            uchNewHrValue = (((GU16)guchPreHrValue) *2 + (GU16)uchCurrentOriginalHrValue)/3;
        }
        guchPreHrValue = uchNewHrValue;

        //write back to result
        pstAlgoResult->pnResults[0] = uchNewHrValue;
    }
}
#endif






















