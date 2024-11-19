#include "include/LEDCPWMManager.hpp"
#include "include/LEDCTimer.hpp"
#include "include/LEDCPWM.hpp"
#include <algorithm>
#include <unordered_set>
#include "esp_err.h"
#include "esp_log.h"

esp_err_t LEDCPWMManager::configureTimersAndChannels(const LEDCConfig& p_config) {
    esp_err_t l_ret = validateConfig(p_config);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to validate config");
        return l_ret;
    }

    l_ret = configureAndInitializeTimers(p_config);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure timers");
        return l_ret;
    }

    l_ret = configureAndInitializeChannels(p_config);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure channels");
        return l_ret;
    }
    return ESP_OK;
}   

esp_err_t LEDCPWMManager::configureAndInitializeTimers(const LEDCConfig& p_config) {
    for (const auto& l_timerConfig : p_config.timerConfigs) {
        auto l_timer = std::make_shared<LEDCTimer>(l_timerConfig);
   
        esp_err_t l_ret = l_timer->init();
        if (l_ret != ESP_OK) { return l_ret; }

        if (l_timerConfig.speedMode == LEDC_HIGH_SPEED_MODE) {
            m_highSpeedTimers.emplace(l_timerConfig.timerNum, l_timer); 
        } else {
            m_lowSpeedTimers.emplace(l_timerConfig.timerNum, l_timer); 
        }
    }
    return ESP_OK;
}

esp_err_t LEDCPWMManager::configureAndInitializeChannels(const LEDCConfig& p_config) {
    ESP_LOGD(TAG, "Configuring and initializing channels");
    for (const auto& l_channelConfig : p_config.channelConfigs) {
        ESP_LOGD(TAG, "Configuring channel %d", l_channelConfig.channelNum);
        auto l_matchingTimer = std::find_if(p_config.timerConfigs.begin(), p_config.timerConfigs.end(),
            [&l_channelConfig](const LEDCTimerConfig& p_timerConfig) {
                return p_timerConfig.timerNum == l_channelConfig.timerNum 
                    && p_timerConfig.speedMode == l_channelConfig.speedMode;
            });
        if (l_matchingTimer == p_config.timerConfigs.end()) {
            ESP_LOGE(TAG, "Channel %d references non-existent or mismatched timer %d", l_channelConfig.channelNum, l_channelConfig.timerNum);
            return ESP_ERR_INVALID_ARG;
        }
        else {
            ESP_LOGD(TAG, "Found matching timer %d", l_channelConfig.timerNum);
            const auto l_maxDuty = 1 << l_matchingTimer->dutyResolution - 1;
            
            auto l_pwm = std::make_shared<LEDCPWM>();
            esp_err_t l_ret = l_pwm->init(l_channelConfig, l_maxDuty);

            if (l_channelConfig.speedMode == LEDC_HIGH_SPEED_MODE) {
                m_highSpeedChannels.emplace(l_channelConfig.channelNum, l_pwm);
            } else {
                m_lowSpeedChannels.emplace(l_channelConfig.channelNum, l_pwm);
            }
        }
    }
    return ESP_OK;
}
esp_err_t LEDCPWMManager::validateConfig(const LEDCConfig& p_config) const {
    esp_err_t l_ret = validateNumberOfTimers(p_config, LEDC_HIGH_SPEED_MODE);
    if (l_ret != ESP_OK) { return l_ret; }

    l_ret = validateNumberOfTimers(p_config, LEDC_LOW_SPEED_MODE);
    if (l_ret != ESP_OK) { return l_ret; }

    l_ret = validateTimerIds(p_config);
    if (l_ret != ESP_OK) { return l_ret; }

    l_ret = validateUniqueTimerIds(p_config);
    if (l_ret != ESP_OK) { return l_ret; }

    l_ret = validateNumberOfChannels(p_config, LEDC_HIGH_SPEED_MODE);
    if (l_ret != ESP_OK) { return l_ret; }

    l_ret = validateNumberOfChannels(p_config, LEDC_LOW_SPEED_MODE);
    if (l_ret != ESP_OK) { return l_ret; }

    l_ret = validateChannelIds(p_config);
    if (l_ret != ESP_OK) { return l_ret; }

    l_ret = validateUniqueChannelIds(p_config);
    if (l_ret != ESP_OK) { return l_ret; }

    return ESP_OK;
}

esp_err_t LEDCPWMManager::validateNumberOfTimers(const LEDCConfig& p_config, ledc_mode_t p_speedMode) const {
    auto l_count = std::count_if(p_config.timerConfigs.begin(), p_config.timerConfigs.end(), 
        [p_speedMode](const LEDCTimerConfig& p_timerConfig) { return p_timerConfig.speedMode == p_speedMode;}); 

    ESP_LOGD(TAG, "Number of timers found in configuration for speed mode %d: %d", p_speedMode, l_count);

    if (l_count > LEDC_TIMER_MAX) {
        ESP_LOGE(TAG, "Too many timers configured for speed mode %d", p_speedMode);
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}


esp_err_t LEDCPWMManager::validateTimerIds(const LEDCConfig& p_config) const {
    ESP_LOGD(TAG, "Validating timer IDs in configuration");
    auto l_invalidTimer = std::find_if(p_config.timerConfigs.begin(), p_config.timerConfigs.end(), 
        [](const LEDCTimerConfig& p_timerConfig) { return p_timerConfig.timerNum > LEDC_TIMER_MAX; });

    if (l_invalidTimer != p_config.timerConfigs.end()) {
        ESP_LOGE(TAG, "Invalid timer id %d", l_invalidTimer->timerNum);
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

esp_err_t LEDCPWMManager::validateUniqueTimerIds(const LEDCConfig& p_config) const {
    std::unordered_set<ledc_timer_t> l_highSpeedTimerIds, l_lowSpeedTimerIds;
    ESP_LOGD(TAG, "Validating unique timer IDs in configuration");
    for (const auto& l_timerConfig : p_config.timerConfigs) {
        auto& l_timerSet = (l_timerConfig.speedMode == LEDC_HIGH_SPEED_MODE) ? l_highSpeedTimerIds : l_lowSpeedTimerIds;
        if (!l_timerSet.insert(l_timerConfig.timerNum).second) {
            ESP_LOGE(TAG, "Duplicate timer ID %d for speed mode %d", l_timerConfig.timerNum, l_timerConfig.speedMode);
            return ESP_ERR_INVALID_ARG;
        }
    }
    return ESP_OK;
}

esp_err_t LEDCPWMManager::validateNumberOfChannels(const LEDCConfig& p_config, ledc_mode_t p_speedMode) const {
    auto l_count = std::count_if(p_config.channelConfigs.begin(), p_config.channelConfigs.end(),
        [p_speedMode](const LEDCChannelConfig& p_channelConfig) { return p_channelConfig.speedMode == p_speedMode; });

    ESP_LOGD(TAG, "Number of channels found in configuration for speed mode %d: %d", p_speedMode, l_count);

    if (l_count > LEDC_CHANNEL_MAX) {
        ESP_LOGE(TAG, "Too many channels configured for speed mode %d", p_speedMode);
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

esp_err_t LEDCPWMManager::validateChannelIds(const LEDCConfig& p_config) const {
    ESP_LOGD(TAG, "Validating channel IDs in configuration");
    auto invalidChannel = std::find_if(p_config.channelConfigs.begin(), p_config.channelConfigs.end(),
        [](const LEDCChannelConfig& p_channelConfig) { return p_channelConfig.channelNum >= LEDC_CHANNEL_MAX; });

    if (invalidChannel != p_config.channelConfigs.end()) {
        ESP_LOGE(TAG, "Invalid channel ID: %d", invalidChannel->channelNum);
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

esp_err_t LEDCPWMManager::validateUniqueChannelIds(const LEDCConfig& p_config) const {
    std::unordered_set<ledc_timer_t> l_highSpeedChannelIds, l_lowSpeedChannelIds;
    ESP_LOGD(TAG, "Validating unique channel IDs in configuration");
    for (const auto& l_channelConfig : p_config.channelConfigs) {
        auto& l_channelSet = (l_channelConfig.speedMode == LEDC_HIGH_SPEED_MODE) ? l_highSpeedChannelIds : l_lowSpeedChannelIds;
        if (!l_channelSet.insert(l_channelConfig.channelNum).second) {
            ESP_LOGE(TAG, "Duplicate channel ID %d for speed mode %d", l_channelConfig.channelNum, l_channelConfig.speedMode);
            return ESP_ERR_INVALID_ARG;
        }
    }
    return ESP_OK;
}
