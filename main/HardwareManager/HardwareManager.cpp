#include "include/HardwareManager.hpp"
#include "include/LEDCTimer.hpp"
#include "include/LEDCPWM.hpp"
#include "Components/Include/GPIO.hpp"
#include "ConfigValidation/Include/LEDCConfigValidator.hpp"
#include "ConfigValidation/Include/GPIOConfigValidator.hpp"
#include "esp_err.h"
#include "esp_log.h"
#include <algorithm>
#include <unordered_set>

esp_err_t HardwareManager::configureLEDCPWM(const LEDCConfig& p_config) {
    esp_err_t l_ret = LEDCConfigValidator::validateConfig(p_config);
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

esp_err_t HardwareManager::configureAndInitializeTimers(const LEDCConfig& p_config) {
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

esp_err_t HardwareManager::configureAndInitializeChannels(const LEDCConfig& p_config) {
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

esp_err_t HardwareManager::configureGPIO(const GPIOConfig& p_config) {
    esp_err_t l_ret = GPIOConfigValidator::validateConfig(p_config);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to validate GPIO config");
        return l_ret;
    }

    auto l_gpio = std::make_shared<GPIO>(p_config);
    l_ret = l_gpio->init();
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize GPIO");
        return l_ret;
    }

    m_gpios[p_config.pinNum] = l_gpio;
    ESP_LOGI(TAG, "GPIO pin %d configured successfully", p_config.pinNum);
    return ESP_OK;
}

std::shared_ptr<IGPIO> HardwareManager::getGPIO(gpio_num_t p_pinNum) const {
    auto l_it = m_gpios.find(p_pinNum);
    if (l_it != m_gpios.end()) {
        return l_it->second;
    }
    ESP_LOGW(TAG, "GPIO pin %d not found", p_pinNum);
    return nullptr;
}
