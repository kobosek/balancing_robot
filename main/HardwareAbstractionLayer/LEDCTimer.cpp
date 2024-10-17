#include "include/LEDCTimer.hpp"
#include "esp_err.h"
#include "esp_log.h"


esp_err_t LEDCTimer::init(const LEDCTimerConfig& p_config) {
    ESP_LOGD(TAG, "Initializing LEDC Timer Num: %d, Speed Mode: %d, Duty Resolution: %d, Frequency: %d, Clock: %d",
             p_config.timerNum, p_config.speedMode, p_config.dutyResolution, p_config.frequency, p_config.clock);

    ledc_timer_config_t l_timerConfig = {
        .speed_mode = p_config.speedMode,
        .duty_resolution = p_config.dutyResolution,
        .timer_num = p_config.timerNum,
        .freq_hz = p_config.frequency,
        .clk_cfg = p_config.clock
    };

    esp_err_t l_ret = ledc_timer_config(&l_timerConfig);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize LEDC Timer: %s", esp_err_to_name(l_ret));
    }
    return l_ret;  
}
