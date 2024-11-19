#include "Include/LEDCConfigValidator.hpp"
#include "esp_log.h"
#include <algorithm>
#include <unordered_set>

esp_err_t LEDCConfigValidator::validateConfig(const LEDCConfig& p_config) {
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

esp_err_t LEDCConfigValidator::validateNumberOfTimers(const LEDCConfig& p_config, ledc_mode_t p_speedMode) {
    auto l_count = std::count_if(p_config.timerConfigs.begin(), p_config.timerConfigs.end(), 
        [p_speedMode](const LEDCTimerConfig& p_timerConfig) { return p_timerConfig.speedMode == p_speedMode;}); 

    ESP_LOGD(TAG, "Number of timers found in configuration for speed mode %d: %d", p_speedMode, l_count);

    if (l_count > LEDC_TIMER_MAX) {
        ESP_LOGE(TAG, "Too many timers configured for speed mode %d", p_speedMode);
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

esp_err_t LEDCConfigValidator::validateTimerIds(const LEDCConfig& p_config) {
    ESP_LOGD(TAG, "Validating timer IDs in configuration");
    auto l_invalidTimer = std::find_if(p_config.timerConfigs.begin(), p_config.timerConfigs.end(), 
        [](const LEDCTimerConfig& p_timerConfig) { return p_timerConfig.timerNum >= LEDC_TIMER_MAX; });

    if (l_invalidTimer != p_config.timerConfigs.end()) {
        ESP_LOGE(TAG, "Invalid timer id %d", l_invalidTimer->timerNum);
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

esp_err_t LEDCConfigValidator::validateUniqueTimerIds(const LEDCConfig& p_config) {
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

esp_err_t LEDCConfigValidator::validateNumberOfChannels(const LEDCConfig& p_config, ledc_mode_t p_speedMode) {
    auto l_count = std::count_if(p_config.channelConfigs.begin(), p_config.channelConfigs.end(),
        [p_speedMode](const LEDCChannelConfig& p_channelConfig) { return p_channelConfig.speedMode == p_speedMode; });

    ESP_LOGD(TAG, "Number of channels found in configuration for speed mode %d: %d", p_speedMode, l_count);

    if (l_count > LEDC_CHANNEL_MAX) {
        ESP_LOGE(TAG, "Too many channels configured for speed mode %d", p_speedMode);
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

esp_err_t LEDCConfigValidator::validateChannelIds(const LEDCConfig& p_config) {
    ESP_LOGD(TAG, "Validating channel IDs in configuration");
    auto invalidChannel = std::find_if(p_config.channelConfigs.begin(), p_config.channelConfigs.end(),
        [](const LEDCChannelConfig& p_channelConfig) { return p_channelConfig.channelNum >= LEDC_CHANNEL_MAX; });

    if (invalidChannel != p_config.channelConfigs.end()) {
        ESP_LOGE(TAG, "Invalid channel ID: %d", invalidChannel->channelNum);
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

esp_err_t LEDCConfigValidator::validateUniqueChannelIds(const LEDCConfig& p_config) {
    std::unordered_set<ledc_channel_t> l_highSpeedChannelIds, l_lowSpeedChannelIds;
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
