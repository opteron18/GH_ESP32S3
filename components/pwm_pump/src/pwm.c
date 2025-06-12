// pwm.c
#include "pwm.h"
#include "esp_log.h"
#include "driver/mcpwm_prelude.h"

static const char *TAG = "MCPWM";

#define PWM_GPIO_A  2
#define PWM_GPIO_B  3

static mcpwm_timer_handle_t hb_timer = NULL;
static mcpwm_oper_handle_t hb_oper = NULL;
static mcpwm_cmpr_handle_t hb_comparator = NULL;
static mcpwm_gen_handle_t gen_a = NULL, gen_b = NULL;

static mcpwm_timer_handle_t single_timer = NULL;
static mcpwm_oper_handle_t single_oper = NULL;
static mcpwm_cmpr_handle_t single_comparator = NULL;
static mcpwm_gen_handle_t single_generator = NULL;

esp_err_t pwm_halfbridge_init(uint32_t pwm_freq_hz) {
    mcpwm_timer_config_t timer_config = {
        .group_id = 0,
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = 1000000,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
        .period_ticks = 1000000 / pwm_freq_hz,
    };
    ESP_ERROR_CHECK(mcpwm_new_timer(&timer_config, &hb_timer));

    mcpwm_operator_config_t operator_config = {.group_id = 0};
    ESP_ERROR_CHECK(mcpwm_new_operator(&operator_config, &hb_oper));
    ESP_ERROR_CHECK(mcpwm_operator_connect_timer(hb_oper, hb_timer));

    mcpwm_comparator_config_t comparator_config = {
        .flags.update_cmp_on_tez = true,
    };
    ESP_ERROR_CHECK(mcpwm_new_comparator(hb_oper, &comparator_config, &hb_comparator));
    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(hb_comparator, timer_config.period_ticks / 2));

    mcpwm_generator_config_t gen_config_a = {.gen_gpio_num = PWM_GPIO_A};
    ESP_ERROR_CHECK(mcpwm_new_generator(hb_oper, &gen_config_a, &gen_a));

    mcpwm_generator_config_t gen_config_b = {.gen_gpio_num = PWM_GPIO_B};
    ESP_ERROR_CHECK(mcpwm_new_generator(hb_oper, &gen_config_b, &gen_b));

    mcpwm_gen_timer_event_action_t timer_action_a = {
        .direction = MCPWM_TIMER_DIRECTION_UP,
        .event = MCPWM_TIMER_EVENT_EMPTY,
        .action = MCPWM_GEN_ACTION_HIGH,
    };
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(gen_a, timer_action_a));

    mcpwm_gen_compare_event_action_t cmp_action_a = {
        .direction = MCPWM_TIMER_DIRECTION_UP,
        .comparator = hb_comparator,
        .action = MCPWM_GEN_ACTION_LOW,
    };
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(gen_a, cmp_action_a));

    mcpwm_gen_timer_event_action_t timer_action_b = {
        .direction = MCPWM_TIMER_DIRECTION_UP,
        .event = MCPWM_TIMER_EVENT_EMPTY,
        .action = MCPWM_GEN_ACTION_LOW,
    };
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(gen_b, timer_action_b));

    mcpwm_gen_compare_event_action_t cmp_action_b = {
        .direction = MCPWM_TIMER_DIRECTION_UP,
        .comparator = hb_comparator,
        .action = MCPWM_GEN_ACTION_HIGH,
    };
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(gen_b, cmp_action_b));

    return ESP_OK;
}

esp_err_t pwm_halfbridge_start(void) {
    ESP_ERROR_CHECK(mcpwm_timer_enable(hb_timer));
    ESP_ERROR_CHECK(mcpwm_timer_start_stop(hb_timer, MCPWM_TIMER_START_NO_STOP));
    return ESP_OK;
}

esp_err_t pwm_halfbridge_stop(void) {
    ESP_ERROR_CHECK(mcpwm_timer_start_stop(hb_timer, MCPWM_TIMER_STOP_EMPTY));
    return ESP_OK;
}

esp_err_t pwm_single_init(uint32_t pwm_freq_hz, float duty_cycle, gpio_num_t gpio) {
    if (duty_cycle < 0.0f || duty_cycle > 1.0f || pwm_freq_hz == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    mcpwm_timer_config_t timer_config = {
        .group_id = 1,
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = 1000000,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
        .period_ticks = 1000000 / pwm_freq_hz,
    };
    ESP_ERROR_CHECK(mcpwm_new_timer(&timer_config, &single_timer));

    mcpwm_operator_config_t operator_config = {.group_id = 1};
    ESP_ERROR_CHECK(mcpwm_new_operator(&operator_config, &single_oper));
    ESP_ERROR_CHECK(mcpwm_operator_connect_timer(single_oper, single_timer));

    mcpwm_comparator_config_t comparator_config = {
        .flags.update_cmp_on_tez = true,
    };
    ESP_ERROR_CHECK(mcpwm_new_comparator(single_oper, &comparator_config, &single_comparator));
    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(single_comparator,
        (uint32_t)(timer_config.period_ticks * duty_cycle)));

    mcpwm_generator_config_t gen_config = {.gen_gpio_num = gpio};
    ESP_ERROR_CHECK(mcpwm_new_generator(single_oper, &gen_config, &single_generator));

    mcpwm_gen_timer_event_action_t timer_action = {
        .direction = MCPWM_TIMER_DIRECTION_UP,
        .event = MCPWM_TIMER_EVENT_EMPTY,
        .action = MCPWM_GEN_ACTION_HIGH,
    };
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(single_generator, timer_action));

    mcpwm_gen_compare_event_action_t cmp_action = {
        .direction = MCPWM_TIMER_DIRECTION_UP,
        .comparator = single_comparator,
        .action = MCPWM_GEN_ACTION_LOW,
    };
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(single_generator, cmp_action));

    return ESP_OK;
}

esp_err_t pwm_single_start(void) {
    ESP_ERROR_CHECK(mcpwm_timer_enable(single_timer));
    ESP_ERROR_CHECK(mcpwm_timer_start_stop(single_timer, MCPWM_TIMER_START_NO_STOP));
    return ESP_OK;
}

esp_err_t pwm_single_stop(void) {
    ESP_ERROR_CHECK(mcpwm_timer_start_stop(single_timer, MCPWM_TIMER_STOP_EMPTY));
    return ESP_OK;
}
