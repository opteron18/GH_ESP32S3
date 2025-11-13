/*
 * 数据协议测试头文件
 */

#ifndef _PROTOCOL_TEST_H_
#define _PROTOCOL_TEST_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 运行所有协议测试
 */
void run_protocol_tests(void);

/**
 * @brief 位操作测试
 */
void bit_operation_test(void);

/**
 * @brief 协议测试主函数
 */
void protocol_test_all(void);

#ifdef __cplusplus
}
#endif

#endif /* _PROTOCOL_TEST_H_ */