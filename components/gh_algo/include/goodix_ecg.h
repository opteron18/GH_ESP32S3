#ifndef __GOODIX_ECG_H
#define __GOODIX_ECG_H

#include "stdint.h"
#include "goodix_type.h"


#define ECG_INTERFACE_VERSION "pv_v1.0.0"
#define ECG_INTERFACE_VERSION_LEN_MAX (20)
#define ECG_CHANNEL_NUM (2)

/**
* @brief ECG �㷨��������״ֵ̬
*/
typedef enum
{
	GX_ALGO_ECG_SUCCESS = 0x00000000,			/**< �ɹ�     */	

	GX_ALGO_ECG_RWONG_INPUT = 0x00000001,			/**< �������ݸ�ʽ���Ϸ�     */

	GX_ALGO_ECG_NO_MEMORY = 0x00000002,			/**< �ڴ�ռ䲻��     */

}goodix_ecg_ret;


typedef struct 
{
	uint32_t fs;									// ������
	uint32_t valid_channel_num;						// ��Чͨ����
	uint32_t qrs_flag;                              // QRS���flag��0�������������
	uint32_t mode_type;								// ECG��Ϸ�ʽ��0: AC mode,1: DC mode
	uint32_t ic_type;								// оƬ���ͣ�0��CardiffA��1��CardiffB
}goodix_ecg_config;


typedef struct
{
	uint32_t out_delay;                              //�˲��ӳ�
}goodix_ecg_init_result;


typedef struct
{
	uint32_t frameid;			//֡���
	int32_t *ecg_rawdata;        //ԭʼ����
	int32_t acc_x;              //���ٶȼ�x��
	int32_t acc_y;              //���ٶȼ�y��
	int32_t acc_z;              //���ٶȼ�z��
	int32_t ecg_qc_flag;        //���ٻָ����
	int32_t pace;               //pace����
}goodix_ecg_input_rawdata;

typedef struct
{
	float32_t *ecg_out;               //�ĵ�ֵ
	goodix_stage *ecg_stage;            //P-QRS-J-T ��״̬
	float32_t ecg_bpm;
		
	float32_t *ecg_snr;               //�����
	float32_t *ecg_powerline_out;	// �˳���Ƶ���� ���
	float32_t *ecg_blw_out;			// �˳�����Ư�� ���
	float32_t *ecg_bpm_ch;               //����ֵ
}goodix_ecg_result;


#ifdef __cplusplus
extern "C" {
#endif


/**
* @brief ECG �����㷨�汾��
* @param[in]  version     �ⲿ����汾�ſռ䣬����20�ֽ�
* @param[out] ��
* @return     ����ִ�����
*/
DRVDLL_API goodix_ecg_ret goodix_ecg_version(uint8_t version[100]);


/**
* @brief ECG �����㷨��ʼ��
* @param[in]  cfg         ������Ϣ
* @param[in]  cfg_size    ������Ϣ��С
* @param[out] res         ���صĽ��
* @return     ����ִ�����
*/
DRVDLL_API goodix_ecg_ret goodix_ecg_init(const void *cfg_instance, goodix_ecg_init_result *res, int32_t cfg_size, const char *interface_ver);


/**
* @brief ECG �㷨����
* @param[in] raw    ECG ����������Ϣ
* @param[out] res    ECG ���صĽ��
* @return     ����ִ�����
*/
DRVDLL_API goodix_ecg_ret goodix_ecg_update(goodix_ecg_input_rawdata* raw, goodix_ecg_result *res);

/**
*@brief ECG ��������
* @param[in] ��
* @param[out] res    ECG ������
* @return     ����ִ�����
*/
DRVDLL_API goodix_ecg_ret goodix_ecg_classification_update(goodix_classification_flag *res);

/**
* @brief ECG �㷨���٣��ͷ��ڲ����ڴ�ռ䣬��������ģ���㷨ʱʹ��
* @param[out] �� 
* @return     ����ִ�����
*/
DRVDLL_API goodix_ecg_ret goodix_ecg_deinit(void);

/**
* @brief ECG  ���ò�����ȡ����ʼ��ǰ���ã��������ýṹ���ڴ�
* @param[out] ��
* @return     ����ִ�����
*/
DRVDLL_API const void *goodix_ecg_config_get_arr(void);

/**
* @brief ECG  ���ò�����С��ȡ����ʼ��ǰ���ã������㷨�ڲ�����У��
* @param[out] ��
* @return     ��������ò�����С
*/
DRVDLL_API int32_t goodix_ecg_config_get_size(void);

/**
* @brief ECG  ���ð汾�Ż�ȡ����ʼ��ǰ���ã������㷨�ڲ�����У��
* param[in] �汾�ų���
* @param[out] ���ڴ��config�İ汾������
* @return     ��
*/
DRVDLL_API void goodix_ecg_config_get_version(char* ver, uint8_t ver_len);

#ifdef __cplusplus
}
#endif

#endif
