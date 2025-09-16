// #ifndef Goodix_DSP_EXPORTS
#include "goodix_hba.h"
#include "heart_net_org_export.h"
const goodix_hba_config external_hba_cfg =
{
	.mode = HBA_TEST_DYNAMIC,			// ��Чͨ����
	.scence = HBA_SCENES_DEFAULT,
	.fs = 25,			    // ԭʼ����
	.valid_channel_num = 4,
	.back_track_len = 0,                       // ���ݵ�ʱ��,Ĭ��30s,���ʱ������Ϊ120s�����ʱ������Ϊ30s
	.hba_latest_output_time = 20,					// ������ֵʱ��
	.hba_earliest_output_time = 8,				// �����ֵʱ��
    .hba_lowerest_confidence = 0,				// ��ͳ�ֵ���Ŷ�
	.hba_out_step_second = 0,					// ��ֵ���
    .hba_convergence_rate = 0,					// ׷�ٹ����е���������
    .senseless_mode_step = 0,                   // �޸м��ʱ��������Ϊ0��ʾδ֪
    .senseless_mode_duration = 0               // �޸г���ʱ��������Ϊ0��ʾδ֪
};

const void *goodix_hba_config_get_arr()
{
	return &external_hba_cfg;
}

int32_t  goodix_hba_config_get_size()
{
	return sizeof(external_hba_cfg);
}

void goodix_hba_config_get_version(char* ver, uint8_t ver_len)
{
	uint32_t copy_num = ver_len < (strlen(HBA_INTERFACE_VERSION) + 1) ? ver_len : (strlen(HBA_INTERFACE_VERSION) + 1);
	memcpy(ver, HBA_INTERFACE_VERSION, copy_num);
	ver[copy_num - 1] = '\0'; //���NET_VERSION�ĳ��ȴ���size����Ҫ���ӽ�����
}

// #endif //Goodix_DSP_EXPORTS
