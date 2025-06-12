#ifndef __GOODIX_HBA_H
#define __GOODIX_HBA_H

#include <stdint.h>
#ifndef _ARM_MATH_H
#include "goodix_type.h"
#endif
// HBA 采样率
#ifndef HBA_FS
#define HBA_FS 25
#endif
#define PPG_CHANNEL_NUM 4

#define HBA_INTERFACE_VERSION "pv_v1.0.0"
#define HBA_INTERFACE_VERSION_LEN_MAX (20)

/**
* @brief HBA 通道类型
*/
typedef enum
{
    CH_ENABLE_DEFAULT=0,//按照默认配置
    //单通道
    CH1_ENABLE_1000 = 1000,	
    CH1_ENABLE_0100 = 100,
    CH1_ENABLE_0010 = 10,
    CH1_ENABLE_0001 = 1,
    //双通道,对侧配置
    CH2_ENABLE_1010 = 1010,
    CH2_ENABLE_0101 = 101,
    //三通道
    CH3_ENABLE_0111 = 111,
    CH3_ENABLE_1011 = 1011,
    CH3_ENABLE_1101 = 1101,
    CH3_ENABLE_1110 = 1110,
    //四通道
    CH4_ENABLE_1111 = 1111,
}goodix_hba_ch_enable;

/**
* @brief HBA 算法函数返回状态值
*/
typedef enum
{
	GX_ALGO_HBA_SUCCESS = 0x00000000,			/**< 成功     */
	GX_ALGO_HBA_RWONG_INPUT = 0x00000001,		/**< 输入数据格式不合法     */
	GX_ALGO_HBA_NO_MEMORY = 0x00008000,			/**< 内存空间不够     */
}goodix_hba_ret;

typedef enum {
	HBA_SCENES_DEFAULT = 0,				// 默认：由算法内部识别处理

	HBA_SCENES_DAILY_LIFE = 1,			// 日常生活
	HBA_SCENES_RUNNING_INSIDE = 2,		// 室内跑步机
	HBA_SCENES_WALKING_INSIDE = 3,		// 室内步行
	HBA_SCENES_STAIRS = 4,				// 上下楼梯

	HBA_SCENES_RUNNING_OUTSIDE = 5,		// 室外跑步
	HBA_SCENES_WALKING_OUTSIDE = 6,		// 室外步行

	HBA_SCENES_STILL_REST = 7,			// 静息
	HBA_SCENES_REST = 8,				// 休息
	HBA_SCENES_STILLRADON = 9,			// 憋气

	HBA_SCENES_BIKING_INSIDE = 10,		//室内自行车
	HBA_SCENES_BIKING_OUTSIDE = 11,		//室外自行车
	HBA_SCENES_BIKING_MOUNTAIN= 12,		//室外自行车越野
	HBA_SCENES_RUNNING_HIGH_HR = 13,	//高心率跑步

	HBA_SCENES_RUNNING_TREADMILL_CCOMBINE= 14,		// 跑步机组合跑

	HBA_SCENES_HIGH_INTENSITY_COMBINE = 15,		// 高强度运动组合
	HBA_SCENES_TRADITIONAL_STRENGTH_COMBINE = 16,		// 传统力量训练组合
	HBA_SCENES_STEP_TEST = 17,		    // 台阶测试

	HBA_SCENES_BALL_SPORTS = 18,		// 球类运动
	HBA_SCENES_AEROBICS = 19,		    // 健身操

    HBA_SCENES_SLEEP = 20,              // 睡眠场景
    HBA_SCENES_JUMP = 21,               //手腕跳绳（LS ROMA）
    HBA_SCENES_CORDLESS_JUMP = 22,	    // 万机无绳跳绳测试
	HBA_SCENES_SWIMMING = 23,           // 游泳场景
    HBA_SCENES_SIZE = 24,               // 场景数目
}hba_scenes_e;

typedef enum {
    HBA_TEST_DYNAMIC = 0,		// 默认：动态测试
    HBA_TEST_DOT = 1,			// 点测
    HBA_TEST_SENSELESS = 2,		// 无感模式
}hba_test_mode;

typedef struct {
    hba_test_mode mode;                             // 测试模式
	hba_scenes_e scence;							// 场景
	uint32_t fs;									// 采样率
	int32_t valid_channel_num;						// 有效通道数
	// 用于外部控制算法倾向性的参数
    uint32_t back_track_len;                       // 回溯的时长,默认30s,最大时长限制为120s，最短时长限制为30s
	int32_t hba_latest_output_time;					// 最晚出值时间
	int32_t hba_earliest_output_time;				// 最早出值时间
    uint32_t hba_lowerest_confidence;				// 最低出值置信度
	uint32_t hba_out_step_second;					// 出值间隔
    uint32_t hba_convergence_rate;					// 追踪过程中的收敛速率
    uint32_t senseless_mode_step;                   // 无感间隔时间秒数，为0表示未知
    uint32_t senseless_mode_duration;               // 无感持续时间秒数，为0表示未知
}goodix_hba_config;

typedef struct
{
	uint32_t frameid;								// 帧序号
	uint32_t total_channel_num;                     // 通道总数
	int32_t *ppg_rawdata;		// PPG原始数据，依次绿光1234-红外1234-红光1234
	int32_t *cur_adj_flg;		// 电流调光标志位
	int32_t *gain_adj_flg;		// 增益调光标志位
	int32_t *enable_flg;		// 通道使能标志位

	int32_t acc_x;									// 加速度计x轴
	int32_t acc_y;									// 加速度计y轴
	int32_t acc_z;									// 加速度计z轴

    uint32_t sleep_flg;                             //睡眠flg
}goodix_hba_input_rawdata;

typedef struct
{
	uint32_t hba_out_flag;				// 出值标记：为 1 有效
	int32_t hba_out;					// 心率值
	float32_t hba_confi;				// 置信度
	float32_t hba_snr;					// 信噪比   : 滤波去除干扰后，频谱主峰能量处于总能量
    uint32_t valid_level;			    // 置信等级 : 0 -> 1 -> 2 ，越大越可靠
    uint32_t valid_score;				// 置信分数 : 0->100 越大越可靠
    uint32_t hba_acc_info;              // 运动状态 : 0-静息，小运动；1-步行-中运动；2-跑步-大运动；
    hba_scenes_e hba_acc_scence;            // 运动场景 : 参考 hba_scenes_e 枚举类型
	// 每一路的心率值，用于调试
	uint32_t *hba_out_flag_channel;				// 出值标记：为 1 有效
	uint32_t *hba_out_channel;					// 心率值
	float32_t *hba_confi_channel;				// 置信度
	float32_t *hba_snr_channel;					// 信噪比
	float32_t **p_hba_td_filter;				// 时域滤波结果指针数组
	float32_t **p_hba_fd_filter;			// 频域(维纳)滤波结果指针数组，4路加1路最优
}goodix_hba_result;

#ifdef __cplusplus
extern "C" {
#endif


/**
* @brief HBA 处理算法版本号
* @param[in]  version     外部储存版本号空间，分配20字节
* @param[out] 无
* @return     函数执行情况
*/
DRVDLL_API goodix_hba_ret goodix_hba_version(uint8_t version[120]);


/**
* @brief HBA 处理算法初始化
* @param[in]  scence     使用场景
* @param[in]  fs         HBA 和 ACC 的采样率【注：两者需要同频】
* @param[out] 无
* @return     函数执行情况
*/
DRVDLL_API goodix_hba_ret  goodix_hba_init(const void *cfg_instance, uint32_t cfg_size, const char *interface_ver);


/**
* @brief HBA 算法更新
* @param[in] raw    心率数据
* @param[out] res   心率输出结果
* @return     函数执行情况
*/
DRVDLL_API goodix_hba_ret goodix_hba_update(goodix_hba_input_rawdata* raw, goodix_hba_result *res);

/**
* @brief HBA 算法销毁，释放内部的内存空间，调用其他模块算法时使用
* @param[out] 无
* @return     函数执行情况
*/
DRVDLL_API goodix_hba_ret goodix_hba_deinit(void);

/**
* @brief 获取hba的默认配置参数
* @return     hba的默认配置参数
*/
DRVDLL_API const void *goodix_hba_config_get_arr(void);

/**
* @brief 获取hba的配置size
* @return     hba的配置size
*/
DRVDLL_API int32_t  goodix_hba_config_get_size(void);

/**
* @brief 获取HBA配置文件的版本号
* @param[in/out]  ver       版本号保存数组
* @param[in] 	  ver_len   版本号长度，建议使用HBA_INTERFACE_VERSION_LEN_MAX
* @return     	  无
*/
DRVDLL_API void goodix_hba_config_get_version(char* ver, uint8_t ver_len);


/**
* @brief 用户调用打印HBA重要信息，需要提前初始化打印函数
* @param[in]  raw    心率输入数据，NULL == raw时，不打印数据
* @return     	  无
*/
DRVDLL_API void hba_essential_info_print(goodix_hba_input_rawdata* raw);

#ifdef __cplusplus
}
#endif

#endif
