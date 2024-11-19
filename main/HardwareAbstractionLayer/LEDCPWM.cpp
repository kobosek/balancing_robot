#include "include/LEDCPWM.hpp"
#include "esp_err.h"
#include "esp_log.h"
#include <cmath>

LEDCPWM::LEDCPWM(const LEDCChannelConfig& p_channelConfig) : m_config(p_channelConfig) {}

esp_err_t LEDCPWM::init() {
    ESP_LOGD(TAG, "Initializing LEDC Channel Num: %d, Speed Mode: %d, Timer Num: %d, GPIO Num: %d, Initial Duty: %d, Hpoint: %d",
             m_config.channelNum, m_config.speedMode, m_config.timerNum, m_config.pinNum, m_config.duty, m_config.hpoint);

    ledc_channel_config_t l_channelConfig = {
        .gpio_num = m_config.pinNum,
        .speed_mode = m_config.speedMode,
        .channel = m_config.channelNum,
        .intr_type = m_config.interruptType,
        .timer_sel = m_config.timerNum,
        .duty = m_config.duty,
        .hpoint = m_config.hpoint
    };

    esp_err_t l_ret = ledc_channel_config(&l_channelConfig);
    if (l_ret != ESP_OK) {
        setStateError();
        ESP_LOGE(TAG, "Failed to initialize LEDC Channel: %s", esp_err_to_name(l_ret));
        return l_ret;
    }
    setStateInitialized();
    ESP_LOGI(TAG, "LEDC Channel %d initialized successfully with timer %d", m_config.channelNum, m_config.timerNum);
    return ESP_OK;  
}

int LEDCPWM::getPinNum() const {
    return m_config.pinNum;
}

esp_err_t LEDCPWM::setDuty(float p_duty) const {
    ESP_LOGD(TAG, "Setting duty for LEDC Channel Num: %d, Speed Mode: %d, Timer Num: %d, GPIO Num: %d, Duty: %.2f", 
        m_config.channelNum, m_config.speedMode, m_config.timerNum, m_config.pinNum, p_duty);

    if(!isInitialized()) {
        return notInitialized();
    }

    if (p_duty < 0) {
        ESP_LOGE(TAG, "Duty cannot be negative");
        return ESP_ERR_INVALID_ARG;
    }

    uint32_t l_duty = static_cast<uint32_t>(p_duty * m_config.maxDuty);
        
    esp_err_t l_ret = ledc_set_duty(m_config.speedMode, m_config.channelNum, l_duty);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set duty for LEDC %d: %s", m_config.channelNum, esp_err_to_name(l_ret));
        return l_ret;
    }

    l_ret = ledc_update_duty(m_config.speedMode, m_config.channelNum);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to update duty for LEDC %d: %s", m_config.channelNum, esp_err_to_name(l_ret));
        return l_ret;
    }
    ESP_LOGV(TAG, "Duty for LEDC Channel %d set to %.2f", m_config.channelNum, p_duty);
    return ESP_OK;
}

esp_err_t LEDCPWM::notInitialized() const {
    ESP_LOGE(TAG, "LEDC Channel %d is not initialized: %s", m_config.channelNum, esp_err_to_name(ESP_ERR_INVALID_STATE));
    return ESP_ERR_INVALID_STATE;
}