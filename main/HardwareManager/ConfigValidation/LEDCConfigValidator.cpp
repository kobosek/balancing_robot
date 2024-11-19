#include "Include/LEDCConfigValidator.hpp"
#include "esp_log.h"
#include <algorithm>
#include <unordered_set>

esp_err_t LEDCConfigValidator::validateConfig(const HardwareConfig& p_config) {
    ESP_LOGD(TAG, "Validating LEDC configuration");

    const LEDCConfig& l_config = p_config.ledcConfigs;

    esp_err_t l_ret = validateNumberOfTimers(l_config, LEDC_HIGH_SPEED_MODE);
    if (l_ret != ESP_OK) { return l_ret; }

    l_ret = validateNumberOfTimers(l_config, LEDC_LOW_SPEED_MODE);
    if (l_ret != ESP_OK) { return l_ret; }

    l_ret = validateTimerIds(l_config);
    if (l_ret != ESP_OK) { return l_ret; }

    l_ret = validateUniqueTimerIds(l_config);
    if (l_ret != ESP_OK) { return l_ret; }

    l_ret = validateNumberOfChannels(l_config, LEDC_HIGH_SPEED_MODE);
    if (l_ret != ESP_OK) { return l_ret; }

    l_ret = validateNumberOfChannels(l_config, LEDC_LOW_SPEED_MODE);
    if (l_ret != ESP_OK) { return l_ret; }

    l_ret = validateChannelIds(l_config);
    if (l_ret != ESP_OK) { return l_ret; }

    l_ret = validateUniqueChannelIds(l_config);
    if (l_ret != ESP_OK) { return l_ret; }

    l_ret = validatePinNumbers(l_config);
    if (l_ret != ESP_OK) { return l_ret; }

    l_ret = validateUniquePinNumbers(l_config);
    if (l_ret != ESP_OK) { return l_ret; }

    ESP_LOGI(TAG, "LEDC configuration validated successfully");
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

esp_err_t LEDCConfigValidator::validatePinNumbers(const LEDCConfig& p_config) {
    ESP_LOGD(TAG, "Validating pin numbers in configuration");
    
    // Non-existent pins
    const std::unordered_set<int> l_nonExistentPins = {20, 24, 28, 29, 30, 31};
    
    // Flash interface pins (6-11) - generally unavailable
    const std::unordered_set<int> l_flashPins = {6, 7, 8, 9, 10, 11};
    
    // Input-only GPIOs (34-39)
    const std::unordered_set<int> l_inputOnlyPins = {34, 35, 36, 37, 38, 39};

    auto l_invalidPin = std::find_if(p_config.channelConfigs.begin(), p_config.channelConfigs.end(),
        [&](const LEDCChannelConfig& p_channelConfig) { 
            const int pin = p_channelConfig.pinNum;
            
            if (pin < 0 || pin >= GPIO_NUM_MAX) {
                ESP_LOGE(TAG, "LEDC pin number out of range: %d", pin);
                return true;
            }
            
            if (l_nonExistentPins.find(pin) != l_nonExistentPins.end()) {
                ESP_LOGE(TAG, "Non-existent GPIO pin number for LEDC: %d", pin);
                return true;
            }
            
            if (l_flashPins.find(pin) != l_flashPins.end()) {
                ESP_LOGE(TAG, "Flash interface pin cannot be used for LEDC: %d", pin);
                return true;
            }
            
            if (l_inputOnlyPins.find(pin) != l_inputOnlyPins.end()) {
                ESP_LOGE(TAG, "Input-only pin cannot be used for LEDC: %d", pin);
                return true;
            }
            
            return false;
        });

    if (l_invalidPin != p_config.channelConfigs.end()) {
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

esp_err_t LEDCConfigValidator::validateUniquePinNumbers(const LEDCConfig& p_config) {
    std::unordered_set<int> l_usedPins;
    ESP_LOGD(TAG, "Validating unique pin numbers in configuration");
    
    for (const auto& l_channelConfig : p_config.channelConfigs) {
        if (!l_usedPins.insert(l_channelConfig.pinNum).second) {
            ESP_LOGE(TAG, "Duplicate GPIO pin number: %d", l_channelConfig.pinNum);
            return ESP_ERR_INVALID_ARG;
        }
    }
    return ESP_OK;
}