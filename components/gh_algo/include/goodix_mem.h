#ifndef __GOODIX_MEM_H
#define __GOODIX_MEM_H

#include "stdint.h"
#include "goodix_type.h"


typedef enum
{
	MEMPOOL_IS_OK_E =						0, /*< MEM POOL is ok >*/
	ERR_MEMPOOL_NOT_EXIST_E =				-1, /*< mempool not exist >*/
	ERR_PARAMS_MEMPOOL_SIZE_E = 			-2, /*< Input parameter is invalid: mempool size is invalid >*/
	ERR_PARAMS_MALLOC_SIZE_E = 			-3, /*< Input parameter is invalid: malloc size is invalid >*/
	ERR_MEMPOOL_NOT_ENOUGH_MEM_E =		-4, /*< Memory size does not meet the required memory >*/
	ERR_PARAMS_ADDR_WRONG_E = 			-5, /*< Input parameter is invalid: free addr is invalid >*/
	ERR_OP_CHUNK_TREAD_E = 				-6, /*< Occur memory tread >*/
	ERR_OP_CHUNK_LEAKY_E = 				-7, /*< Occur memory leaky >*/
}goodix_mem_ret;

#ifdef __cplusplus
extern "C" {
#endif

//�ڴ治��ʱ�Ľӿڣ�ֻ֧���ڴ����ʱ����ã���֧���ڴ�Խ��ʱ����
#if !(_WIN32 || _WIN64)
extern void Gh3x2xPoolIsNotEnough(void);
#endif

/**
* @brief		�ڴ�س�ʼ������
* @param[in]	mem_addr �ڴ��ַ
				size �ڴ��С
* @param[out]	��
* @return		��ʼ�����  0   < Operation success.
							-1  < Input parameter is invalid
							-2  < Memory size does not meet the minimum required memory
							-3  < Memory manager has been initialized.
*/
DRVDLL_API int32_t goodix_mem_init(void* mem_addr, int32_t size);

/**
* @brief		�ڴ�����ٲ���
* @param[in]	��
* @param[out]	��
* @return		��
*/
DRVDLL_API void goodix_mem_deinit(void);

/**
* @brief		�ڴ��������
* @param[in]	�����ڴ��С(�ֽ�)
* @param[out]	��
* @return		���뵽���ڴ���׵�ַ���ں���������������ת����
*/
DRVDLL_API void* goodix_mem_malloc(int32_t size);

/**
* @brief		�ͷ��ڴ����
* @param[in]	�ͷ��ڴ�ε��׵�ַ
* @param[out]	��
* @return		��
*/
DRVDLL_API void goodix_mem_free(void* mem_addr);

/**
* @brief		��ȡ�ڴ�ص�ǰ�����ڴ�����
* @param[in]	��
* @param[out]	��
* @return		�ڴ�ص�ǰ��������(byte)
*/
DRVDLL_API uint32_t goodix_mem_get_free_size(void);

#ifdef __cplusplus
}
#endif


#endif
