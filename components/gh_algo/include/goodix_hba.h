#ifndef __GOODIX_HBA_H
#define __GOODIX_HBA_H

#include <stdint.h>
#ifndef _ARM_MATH_H
#include "goodix_type.h"
#endif
// HBA ������
#ifndef HBA_FS
#define HBA_FS 25
#endif
#define PPG_CHANNEL_NUM 4

#define HBA_INTERFACE_VERSION "pv_v1.0.0"
#define HBA_INTERFACE_VERSION_LEN_MAX (20)

/**
* @brief HBA ͨ������
*/
typedef enum
{
    CH_ENABLE_DEFAULT=0,//����Ĭ������
    //��ͨ��
    CH1_ENABLE_1000 = 1000,	
    CH1_ENABLE_0100 = 100,
    CH1_ENABLE_0010 = 10,
    CH1_ENABLE_0001 = 1,
    //˫ͨ��,�Բ�����
    CH2_ENABLE_1010 = 1010,
    CH2_ENABLE_0101 = 101,
    //��ͨ��
    CH3_ENABLE_0111 = 111,
    CH3_ENABLE_1011 = 1011,
    CH3_ENABLE_1101 = 1101,
    CH3_ENABLE_1110 = 1110,
    //��ͨ��
    CH4_ENABLE_1111 = 1111,
}goodix_hba_ch_enable;

/**
* @brief HBA �㷨��������״ֵ̬
*/
typedef enum
{
	GX_ALGO_HBA_SUCCESS = 0x00000000,			/**< �ɹ�     */
	GX_ALGO_HBA_RWONG_INPUT = 0x00000001,		/**< �������ݸ�ʽ���Ϸ�     */
	GX_ALGO_HBA_NO_MEMORY = 0x00008000,			/**< �ڴ�ռ䲻��     */
}goodix_hba_ret;

typedef enum {
	HBA_SCENES_DEFAULT = 0,				// Ĭ�ϣ����㷨�ڲ�ʶ����

	HBA_SCENES_DAILY_LIFE = 1,			// �ճ�����
	HBA_SCENES_RUNNING_INSIDE = 2,		// �����ܲ���
	HBA_SCENES_WALKING_INSIDE = 3,		// ���ڲ���
	HBA_SCENES_STAIRS = 4,				// ����¥��

	HBA_SCENES_RUNNING_OUTSIDE = 5,		// �����ܲ�
	HBA_SCENES_WALKING_OUTSIDE = 6,		// ���ⲽ��

	HBA_SCENES_STILL_REST = 7,			// ��Ϣ
	HBA_SCENES_REST = 8,				// ��Ϣ
	HBA_SCENES_STILLRADON = 9,			// ����

	HBA_SCENES_BIKING_INSIDE = 10,		//�������г�
	HBA_SCENES_BIKING_OUTSIDE = 11,		//�������г�
	HBA_SCENES_BIKING_MOUNTAIN= 12,		//�������г�ԽҰ
	HBA_SCENES_RUNNING_HIGH_HR = 13,	//�������ܲ�

	HBA_SCENES_RUNNING_TREADMILL_CCOMBINE= 14,		// �ܲ��������

	HBA_SCENES_HIGH_INTENSITY_COMBINE = 15,		// ��ǿ���˶����
	HBA_SCENES_TRADITIONAL_STRENGTH_COMBINE = 16,		// ��ͳ����ѵ�����
	HBA_SCENES_STEP_TEST = 17,		    // ̨�ײ���

	HBA_SCENES_BALL_SPORTS = 18,		// �����˶�
	HBA_SCENES_AEROBICS = 19,		    // ������

    HBA_SCENES_SLEEP = 20,              // ˯�߳���
    HBA_SCENES_JUMP = 21,               //����������LS ROMA��
    HBA_SCENES_CORDLESS_JUMP = 22,	    // ���������������
	HBA_SCENES_SWIMMING = 23,           // ��Ӿ����
    HBA_SCENES_SIZE = 24,               // ������Ŀ
}hba_scenes_e;

typedef enum {
    HBA_TEST_DYNAMIC = 0,		// Ĭ�ϣ���̬����
    HBA_TEST_DOT = 1,			// ���
    HBA_TEST_SENSELESS = 2,		// �޸�ģʽ
}hba_test_mode;

typedef struct {
    hba_test_mode mode;                             // ����ģʽ
	hba_scenes_e scence;							// ����
	uint32_t fs;									// ������
	int32_t valid_channel_num;						// ��Чͨ����
	// �����ⲿ�����㷨�����ԵĲ���
    uint32_t back_track_len;                       // ���ݵ�ʱ��,Ĭ��30s,���ʱ������Ϊ120s�����ʱ������Ϊ30s
	int32_t hba_latest_output_time;					// ������ֵʱ��
	int32_t hba_earliest_output_time;				// �����ֵʱ��
    uint32_t hba_lowerest_confidence;				// ��ͳ�ֵ���Ŷ�
	uint32_t hba_out_step_second;					// ��ֵ���
    uint32_t hba_convergence_rate;					// ׷�ٹ����е���������
    uint32_t senseless_mode_step;                   // �޸м��ʱ��������Ϊ0��ʾδ֪
    uint32_t senseless_mode_duration;               // �޸г���ʱ��������Ϊ0��ʾδ֪
}goodix_hba_config;

typedef struct
{
	uint32_t frameid;								// ֡���
	uint32_t total_channel_num;                     // ͨ������
	int32_t *ppg_rawdata;		// PPGԭʼ���ݣ������̹�1234-����1234-���1234
	int32_t *cur_adj_flg;		// ���������־λ
	int32_t *gain_adj_flg;		// ��������־λ
	int32_t *enable_flg;		// ͨ��ʹ�ܱ�־λ

	int32_t acc_x;									// ���ٶȼ�x��
	int32_t acc_y;									// ���ٶȼ�y��
	int32_t acc_z;									// ���ٶȼ�z��

    uint32_t sleep_flg;                             //˯��flg
}goodix_hba_input_rawdata;

typedef struct
{
	uint32_t hba_out_flag;				// ��ֵ��ǣ�Ϊ 1 ��Ч
	int32_t hba_out;					// ����ֵ
	float32_t hba_confi;				// ���Ŷ�
	float32_t hba_snr;					// �����   : �˲�ȥ�����ź�Ƶ��������������������
    uint32_t valid_level;			    // ���ŵȼ� : 0 -> 1 -> 2 ��Խ��Խ�ɿ�
    uint32_t valid_score;				// ���ŷ��� : 0->100 Խ��Խ�ɿ�
    uint32_t hba_acc_info;              // �˶�״̬ : 0-��Ϣ��С�˶���1-����-���˶���2-�ܲ�-���˶���
    hba_scenes_e hba_acc_scence;            // �˶����� : �ο� hba_scenes_e ö������
	// ÿһ·������ֵ�����ڵ���
	uint32_t *hba_out_flag_channel;				// ��ֵ��ǣ�Ϊ 1 ��Ч
	uint32_t *hba_out_channel;					// ����ֵ
	float32_t *hba_confi_channel;				// ���Ŷ�
	float32_t *hba_snr_channel;					// �����
	float32_t **p_hba_td_filter;				// ʱ���˲����ָ������
	float32_t **p_hba_fd_filter;			// Ƶ��(ά��)�˲����ָ�����飬4·��1·����
}goodix_hba_result;

#ifdef __cplusplus
extern "C" {
#endif


/**
* @brief HBA �����㷨�汾��
* @param[in]  version     �ⲿ����汾�ſռ䣬����20�ֽ�
* @param[out] ��
* @return     ����ִ�����
*/
DRVDLL_API goodix_hba_ret goodix_hba_version(uint8_t version[120]);


/**
* @brief HBA �����㷨��ʼ��
* @param[in]  scence     ʹ�ó���
* @param[in]  fs         HBA �� ACC �Ĳ����ʡ�ע��������ҪͬƵ��
* @param[out] ��
* @return     ����ִ�����
*/
DRVDLL_API goodix_hba_ret  goodix_hba_init(const void *cfg_instance, uint32_t cfg_size, const char *interface_ver);


/**
* @brief HBA �㷨����
* @param[in] raw    ��������
* @param[out] res   ����������
* @return     ����ִ�����
*/
DRVDLL_API goodix_hba_ret goodix_hba_update(goodix_hba_input_rawdata* raw, goodix_hba_result *res);

/**
* @brief HBA �㷨���٣��ͷ��ڲ����ڴ�ռ䣬��������ģ���㷨ʱʹ��
* @param[out] ��
* @return     ����ִ�����
*/
DRVDLL_API goodix_hba_ret goodix_hba_deinit(void);

/**
* @brief ��ȡhba��Ĭ�����ò���
* @return     hba��Ĭ�����ò���
*/
DRVDLL_API const void *goodix_hba_config_get_arr(void);

/**
* @brief ��ȡhba������size
* @return     hba������size
*/
DRVDLL_API int32_t  goodix_hba_config_get_size(void);

/**
* @brief ��ȡHBA�����ļ��İ汾��
* @param[in/out]  ver       �汾�ű�������
* @param[in] 	  ver_len   �汾�ų��ȣ�����ʹ��HBA_INTERFACE_VERSION_LEN_MAX
* @return     	  ��
*/
DRVDLL_API void goodix_hba_config_get_version(char* ver, uint8_t ver_len);


/**
* @brief �û����ô�ӡHBA��Ҫ��Ϣ����Ҫ��ǰ��ʼ����ӡ����
* @param[in]  raw    �����������ݣ�NULL == rawʱ������ӡ����
* @return     	  ��
*/
DRVDLL_API void hba_essential_info_print(goodix_hba_input_rawdata* raw);

#ifdef __cplusplus
}
#endif

#endif
