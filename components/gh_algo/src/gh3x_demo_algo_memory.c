/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 * 
 * @file    gh3x_demo_algorithm.c
 * 
 * @brief   gh3x driver lib demo code for algorithm
 * 
 * @author  Gooidx Iot Team
 * 
 */
#include "gh3x_demo_algo_call.h"
#include "gh3x_demo_algo_memory.h"
#include "gh3x_demo_algo_config.h"
#include "gh3x_demo_algo_call_inner.h"
#include "goodix_log.h"
#include "goodix_mem.h"

#if (__GOODIX_ALGO_CALL_MODE__)
GU32 gpunAlgoMemBuffer[ (GH3X_ALGORITHMS_MEMORY_SIZE_FINAL ? ((GH3X_ALGORITHMS_MEMORY_SIZE_FINAL + 3) / 4) : 1) ] = {0};
GU32 *gpunAlgoMemBufferPointer = gpunAlgoMemBuffer;
#else  //without algorithm
GU32 *gpunAlgoMemBufferPointer = 0;
#endif

void GH3X_MemFree(void* mem_addr)
{
    goodix_mem_free(mem_addr);
}

void* GH3X_MemMalloc(GS32 nSize)
{
    return goodix_mem_malloc(nSize);
}

void Gh3x2xPoolIsNotEnough(void)
{
    while(1)
    {
        GH3X_ALGO_LOG_PARAM("MEMORY ERROR!!!PLEASE RESET HARDWARE!!!\n");
    }
}

#if (__GOODIX_ALGO_CALL_MODE__)
void* GH3XAlgoMemApply(GU32 unMemSize)
{
    return &gpunAlgoMemBuffer[0];
}

void GH3XAlgoMemFree(void* MemAddr)
{

}

/// algorithm memory is inited or not
GU8 guchAlgoMemStatus = ALGO_MEM_NO_INIT;

/// buffer for algorithm, this addr is import from outer 
//void* gunAlgorithmMemBuffer = 0;

/// buffer size(byte) for algorithm, this size is import from outer
GU32 gunAlgorithmMemBufferSize = 0;

/**
 * @fn     GS8 GH3X_AlgoMemConfig( GU32 unMemSize)
 *
 * @brief  Config algorithm memory.
 *
 * @attention   Mem can only be static,such as global array
 *
 * @param[in]  unMemSize       memory size(unit:byte)
 *
 * @return  error code
 * @retval  #GH3X_RET_OK                      return successfully
 * @retval  #GH3X_RET_RESOURCE_ERROR          return algorithm mem config error
 */
GS8 GH3X_AlgoMemConfig( GU32 unMemSize)
{
    GS8 chRet = GH3X_RET_OK;
    if (unMemSize > 0)
    {
        gunAlgorithmMemBufferSize = unMemSize;
        chRet = GH3X_RET_OK;
        GH3X_ALGO_LOG_PARAM("Algorithm mem Config complete!Size:%d\r\n", (int)unMemSize);
        GH3X_ALGO_LOG_PARAM("Algorithm mem init start...\r\n");
        chRet = GH3X_AlgoMemInit();
        GH3X_AlgoDeinit(0xFFFFFFFF);
    }
    else
    {
        GH3X_ALGO_LOG_PARAM("Algorithm mem config fail!\r\n");
        chRet = GH3X_RET_RESOURCE_ERROR;
    }
    return chRet;
}

/**
 * @fn     GS8 GH3X_AlgoMemInit(void)
 *
 * @brief  algorithm memory init
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  error code
 * @retval  #GH3X_RET_OK                      return successfully
 * @retval  #GH3X_RET_RESOURCE_ERROR          return algorithm mem init error
 */
GS8 GH3X_AlgoMemInit(void)
{
    GS8 chRet = GH3X_RET_OK;
    GH3X_ALGO_LOG_PARAM("Algorithm mem init start...\r\n");
    if (ALGO_MEM_NO_INIT == guchAlgoMemStatus)
    {
        gpunAlgoMemBufferPointer = (GU32*)GH3XAlgoMemApply(gunAlgorithmMemBufferSize);
        if ((gpunAlgoMemBufferPointer != 0) && (gunAlgorithmMemBufferSize > 0))
        {
            if(MEMPOOL_IS_OK_E != goodix_mem_init((void*)gpunAlgoMemBufferPointer, gunAlgorithmMemBufferSize))
            {
                chRet = GH3X_RET_RESOURCE_ERROR;
                GH3X_ALGO_LOG_PARAM("Algorithm mem init error!\r\n");
                
            }
            else
            {
                guchAlgoMemStatus = ALGO_MEM_INIT_OK;
                chRet = GH3X_RET_OK;
                GH3X_ALGO_LOG_PARAM("Algorithm mem init success.Addr:0x%x,Size:%d\r\n", (unsigned int)gpunAlgoMemBufferPointer, \
                                                                                        (int)gunAlgorithmMemBufferSize);
            }
        }
        else
        {
            GH3X_ALGO_LOG_PARAM("Algorithm mem para error!\r\n");
            chRet = GH3X_RET_RESOURCE_ERROR;
        }
    }
    return chRet;
}

/**
* @fn     void GH3X_AlgoMemDeinit(void)
*
* @brief  algorithm memory deinit
*
* @attention   None
*
* @param[in]   None
* @param[out]  None
*
* @return  None
*/
void GH3X_AlgoMemDeinit(void)
{
    if (ALGO_MEM_INIT_OK == guchAlgoMemStatus)
    {
        goodix_mem_deinit();  //destroy all memory
        if(gpunAlgoMemBufferPointer)
        {
            GH3XAlgoMemFree((void*)gpunAlgoMemBufferPointer);
            gpunAlgoMemBufferPointer = 0;
        }
        guchAlgoMemStatus = ALGO_MEM_NO_INIT;
        GH3X_ALGO_LOG_PARAM("Algorithm mem deinit!\r\n");
    }    
}

GU8 GH3X_GetAlgoMemStatus(void)
{
    return guchAlgoMemStatus;
}

GU8 GH3X_GetHrAlgoSupportChnl(void)
{
    return __HR_ALGORITHM_SUPPORT_CHNL_NUM__;
}

GU8 GH3X_GetSpo2AlgoSupportChnl(void)
{
    return __SPO2_ALGORITHM_SUPPORT_CHNL_NUM__;
}



#endif



