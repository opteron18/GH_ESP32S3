#ifndef Goodix_DSP_EXPORTS
#include "goodix_hba.h"

const goodix_hba_config external_hba_cfg =
{
	.mode = HBA_TEST_DYNAMIC,			// 有效通道数
	.scence = HBA_SCENES_DEFAULT,
	.fs = 25,			    // 原始采样
	.valid_channel_num = 4,
	.back_track_len = 0,                       // 回溯的时长,默认30s,最大时长限制为120s，最短时长限制为30s
	.hba_latest_output_time = 20,					// 最晚出值时间
	.hba_earliest_output_time = 8,				// 最早出值时间
    .hba_lowerest_confidence = 0,				// 最低出值置信度
	.hba_out_step_second = 0,					// 出值间隔
    .hba_convergence_rate = 0,					// 追踪过程中的收敛速率
    .senseless_mode_step = 0,                   // 无感间隔时间秒数，为0表示未知
    .senseless_mode_duration = 0               // 无感持续时间秒数，为0表示未知
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
	ver[copy_num - 1] = '\0'; //如果NET_VERSION的长度大于size，需要添加结束符
}

#endif //Goodix_DSP_EXPORTS
