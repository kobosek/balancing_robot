#include "include/LEDCTimer.hpp"
#include "esp_err.h"
#include "esp_log.h"

LEDCTimer::LEDCTimer(const LEDCTimerConfig& p_config) : m_config(p_config) {}

esp_err_t LEDCTimer::init() {
    ESP_LOGD(TAG, "Initializing LEDC Timer Num: %d, Speed Mode: %d, Duty Resolution: %d, Frequency: %d, Clock: %d",
             m_config.timerNum, m_config.speedMode, m_config.dutyResolution, m_config.frequency, m_config.clock);

    ledc_timer_config_t l_timerConfig = {
        .speed_mode = m_config.speedMode,
        .duty_resolution = m_config.dutyResolution,
        .timer_num = m_config.timerNum,
        .freq_hz = m_config.frequency,
        .clk_cfg = m_config.clock
    };

    esp_err_t l_ret = ledc_timer_config(&l_timerConfig);
    if (l_ret != ESP_OK) {
        setStateError();
        ESP_LOGE(TAG, "Failed to initialize LEDC Timer: %s", esp_err_to_name(l_ret));
        return l_ret;
    }
    setStateInitialized();
    ESP_LOGI(TAG, "LEDC Timer %d initialized successfully", m_config.timerNum);
    return ESP_OK;  
}

esp_err_t LEDCTimer::notInitialized() const {
    ESP_LOGE(TAG, "LEDC Timer %d is not initialized: %s", m_config.timerNum, esp_err_to_name(ESP_ERR_INVALID_STATE));
    return ESP_ERR_INVALID_STATE;
}   