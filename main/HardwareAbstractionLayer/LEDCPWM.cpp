#include "include/LEDCPWM.hpp"
#include "esp_err.h"
#include "esp_log.h"

LEDCPWM::LEDCPWM(const LEDCTimerConfig& p_timerConfig, const LEDCCHannelConfig& p_config) : m_timerConfig(p_timerConfig), m_config(p_config) {}

esp_err_t LEDCPWM::init() {
    ledc_channel_config_t l_channelConfig = {
        .gpio_num = m_config.pinNum,
        .speed_mode = m_timerConfig.speedMode,
        .channel = m_config.channelNum,
        .intr_type = m_config.interruptType,
        .timer_sel = m_timerConfig.timerNum,
        .duty = m_config.duty,
        .hpoint = m_config.hpoint
    };

    esp_err_t l_ret = ledc_channel_config(&l_channelConfig);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure LEDC Channel");
    }
    return l_ret;  
}

uint32_t LEDCPWM::calculateMaxDuty() const {
    return (1 << m_timerConfig.dutyResolution) - 1;
}

esp_err_t LEDCPWM::setDuty(float p_duty) const {
    p_duty = static_cast<uint32_t>(std::abs(p_duty) * calculateMaxDuty());
    
    esp_err_t l_ret = ledc_set_duty(m_timerConfig.speedMode, m_config.channelNum, p_duty);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set duty for LEDC %d", m_config.channelNum);
        return l_ret;
    }
    l_ret = ledc_update_duty(m_timerConfig.speedMode, m_config.channelNum);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to update duty for LEDC %d", m_config.channelNum);
        return l_ret;
    }

}

