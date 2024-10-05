#include "include/LEDCTimer.hpp"
#include "esp_err.h"
#include "esp_log.h"

LEDCTimer::LEDCTimer(const LEDCTimerConfig& p_config) : m_config(p_config) {}

esp_err_t LEDCTimer::init() {
    ledc_timer_config_t l_timerConfig = {
        .speed_mode = m_config.speedMode,
        .duty_resolution = m_config.dutyResolution,
        .timer_num = m_config.timerNum,
        .freq_hz = m_config.frequency,
        .clk_cfg = m_config.clock
    };
    
    esp_err_t l_ret = ledc_timer_config(&l_timerConfig);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure LEDC Timer");
    }
    return l_ret;  
}


const LEDCTimerConfig& LEDCTimer::getConfig() const {
    return m_config;
}