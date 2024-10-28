#include "include/LEDCPWM.hpp"
#include "esp_err.h"
#include "esp_log.h"
#include <cmath> 

esp_err_t LEDCPWM::init(const LEDCChannelConfig& p_channelConfig, uint32_t p_maxDuty) {
    ESP_LOGD(TAG, "Initializing LEDC Channel Num: %d, Speed Mode: %d, Timer Num: %d, GPIO Num: %d, Initial Duty: %d, Hpoint: %d",
             p_channelConfig.channelNum, p_channelConfig.speedMode, p_channelConfig.timerNum, p_channelConfig.pinNum, p_channelConfig.duty, p_channelConfig.hpoint);

    ledc_channel_config_t l_channelConfig = {
        .gpio_num = p_channelConfig.pinNum,
        .speed_mode = p_channelConfig.speedMode,
        .channel = p_channelConfig.channelNum,
        .intr_type = p_channelConfig.interruptType,
        .timer_sel = p_channelConfig.timerNum,
        .duty = p_channelConfig.duty,
        .hpoint = p_channelConfig.hpoint
    };

    esp_err_t l_ret = ledc_channel_config(&l_channelConfig);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize LEDC Channel: %s", esp_err_to_name(l_ret));
    }

    m_setAndUpdateDuty = [=](float p_duty) {
        ESP_LOGD(TAG, "Setting duty for LEDC Channel Num: %d, Speed Mode: %d, Timer Num: %d, GPIO Num: %d, Duty: %.2f", 
            p_channelConfig.channelNum, p_channelConfig.speedMode, p_channelConfig.timerNum, p_channelConfig.pinNum, p_duty);

        if (p_duty < 0) {
            ESP_LOGE(TAG, "Duty cannot be negative");
            return ESP_ERR_INVALID_ARG;
        }

        uint32_t l_duty = static_cast<uint32_t>(p_duty * p_maxDuty);
        
        esp_err_t l_ret = ledc_set_duty(p_channelConfig.speedMode, p_channelConfig.channelNum, l_duty);
        if (l_ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to set duty for LEDC %d: %s", p_channelConfig.channelNum, esp_err_to_name(l_ret));
            return l_ret;
        }

        l_ret = ledc_update_duty(p_channelConfig.speedMode, p_channelConfig.channelNum);
        if (l_ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to update duty for LEDC %d: %s", p_channelConfig.channelNum, esp_err_to_name(l_ret));
            return l_ret;
        }
        return ESP_OK;
    };
    return l_ret;  
}

esp_err_t LEDCPWM::setDuty(float p_duty) const {
    return m_setAndUpdateDuty(p_duty);
}
