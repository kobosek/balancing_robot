#include "include/MCPWMTimer.hpp"


esp_err_t MCPWMTimer::init() {
    mcpwm_config_t l_pwmConfig;
    pwm_config.frequency = frequency;
    pwm_config.cmpr_a = 0;
    pwm_config.cmpr_b = 0;
    pwm_config.counter_mode = counter_mode;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;

    esp_err_t l_ret = mcpwm_init(unit, timer, &l_pwmConfig);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure MCPWM Timer");
    }
}

mcpwm_unit_t MCPWMTimer::getUnit() const {

}

mcpwm_timer_t MCPWMTimer::getTimer() const {

}
