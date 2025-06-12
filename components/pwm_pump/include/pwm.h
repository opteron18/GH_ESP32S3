#ifndef _PWM_H_
#define _PWM_H_

#include "esp_err.h"
#include "driver/gpio.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 初始化互补 PWM，50% 占空比互斥输出
 * @param pwm_freq_hz 输出频率（建议 10kHz）
 * @return esp_err_t
 */
esp_err_t pwm_halfbridge_init(uint32_t pwm_freq_hz);
esp_err_t pwm_halfbridge_start(void);
esp_err_t pwm_halfbridge_stop(void);
esp_err_t pwm_single_init(uint32_t pwm_freq_hz, float duty_cycle, gpio_num_t gpio);
esp_err_t pwm_single_start(void);
esp_err_t pwm_single_stop(void);

#ifdef __cplusplus
}
#endif

#endif // _PWM_HALFBRIDGE_H_
