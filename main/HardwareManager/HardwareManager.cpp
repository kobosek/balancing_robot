#include "include/HardwareManager.hpp"

#include "Components/include/LEDCTimer.hpp"
#include "Components/include/LEDCPWM.hpp"
#include "Components/Include/GPIO.hpp"
#include "Components/Include/I2CBus.hpp"
#include "Components/Include/I2CDevice.hpp"
#include "Components/Include/WIFIController.hpp"
#include "Components/Include/NVS.hpp"

#include "ConfigValidation/Include/IConfigValidator.hpp"
#include "esp_err.h"
#include "esp_log.h"
#include <algorithm>
#include <unordered_set>

esp_err_t HardwareManager::configure(const HardwareConfig& p_config) {
    esp_err_t l_ret = ESP_OK;

    l_ret = initializeNVS();
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize NVS");
        return l_ret;
    }

    for (const auto& l_configValidator : m_configValidators) {
        l_ret = l_configValidator->validateConfig(p_config);
        if (l_ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to validate config");
            return l_ret;
        }
    }
    l_ret = configureLEDCPWM(p_config.ledcConfigs);
    if (l_ret != ESP_OK) {  
        ESP_LOGE(TAG, "Failed to configure LEDC PWM");
        return l_ret;
    }

    l_ret = configureGPIO(p_config.gpioConfigs);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure GPIO");
        return l_ret;
    }

    l_ret = configureI2C(p_config.i2cConfigs);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure I2C");
        return l_ret;
    }

    l_ret = configureWIFI(p_config.wifiConfig);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure WiFi");
        return l_ret;
    }

    return ESP_OK;
}

esp_err_t HardwareManager::configureLEDCPWM(const LEDCConfig& p_config) {
    esp_err_t l_ret = configureAndInitializeTimers(p_config);
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

esp_err_t HardwareManager::configureGPIO(const GPIOSConfig& p_config) {
    esp_err_t l_ret = ESP_OK;
    for (const auto& l_gpioConfig : p_config) {
        auto l_gpio = std::make_shared<GPIO>(l_gpioConfig);
        l_ret = l_gpio->init();
        if (l_ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to initialize GPIO");
            return l_ret;
        }
        m_gpios[l_gpioConfig.pinNum] = l_gpio;
        ESP_LOGI(TAG, "GPIO pin %d configured successfully", l_gpioConfig.pinNum);
    }
    return ESP_OK;
}

esp_err_t HardwareManager::configureAndInitializeTimers(const LEDCConfig& p_config) {
    esp_err_t l_ret = ESP_OK;
    for (const auto& l_timerConfig : p_config.timerConfigs) {
        auto l_timer = std::make_shared<LEDCTimer>(l_timerConfig);
        
        l_ret = l_timer->init();
        if (l_ret != ESP_OK) { return l_ret; }

        m_ledcTimers.emplace(std::make_pair(l_timerConfig.speedMode, l_timerConfig.timerNum), l_timer);
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
            l_channelConfig.maxDuty = l_maxDuty;

            auto l_pwm = std::make_shared<LEDCPWM>(l_channelConfig);
            esp_err_t l_ret = l_pwm->init();

            m_ledcChannels.emplace(std::make_pair(l_channelConfig.speedMode, l_channelConfig.channelNum), l_pwm);
        }
    }
    return ESP_OK;
}

esp_err_t HardwareManager::configureI2C(const I2CConfig& p_config) {
    ESP_LOGD(TAG, "Configuring I2C");

    esp_err_t l_ret = configureAndInitializeBuses(p_config);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure I2C buses");
        return l_ret;
    }

    l_ret = configureAndInitializeDevices(p_config);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure I2C devices");
        return l_ret;
    }

    return ESP_OK;
}

esp_err_t HardwareManager::configureAndInitializeBuses(const I2CConfig& p_config) {
    ESP_LOGD(TAG, "Configuring and initializing I2C buses");

    for (const auto& l_busConfig : p_config.busConfigs) {
        ESP_LOGD(TAG, "Configuring I2C bus %d", l_busConfig.port);
        
        auto l_bus = std::make_shared<I2CBus>(l_busConfig);
        esp_err_t l_ret = l_bus->init();
        if (l_ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to initialize I2C bus %d", l_busConfig.port);
            return l_ret;
        }

        m_i2cBuses.emplace(l_busConfig.port, l_bus);
        ESP_LOGD(TAG, "I2C bus %d configured successfully", l_busConfig.port);
    }

    return ESP_OK;
}

esp_err_t HardwareManager::configureAndInitializeDevices(const I2CConfig& p_config) {
    ESP_LOGD(TAG, "Configuring and initializing I2C devices");

    for (const auto& l_deviceConfig : p_config.deviceConfigs) {
        ESP_LOGD(TAG, "Configuring I2C device 0x%02X on bus %d", 
                l_deviceConfig.deviceAddress, l_deviceConfig.busPort);

        // Find the corresponding bus
        auto l_busIt = m_i2cBuses.find(l_deviceConfig.busPort);
        if (l_busIt == m_i2cBuses.end()) {
            ESP_LOGE(TAG, "I2C bus %d not found for device 0x%02X", 
                    l_deviceConfig.busPort, l_deviceConfig.deviceAddress);
            return ESP_ERR_NOT_FOUND;
        }

        auto l_device = std::make_shared<I2CDevice>(l_deviceConfig, l_busIt->second);
        esp_err_t l_ret = l_device->init();
        if (l_ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to initialize I2C device 0x%02X on bus %d", 
                    l_deviceConfig.deviceAddress, l_deviceConfig.busPort);
            return l_ret;
        }

        m_i2cDevices.emplace(std::make_pair(l_deviceConfig.busPort, l_deviceConfig.deviceAddress), l_device);
        ESP_LOGD(TAG, "I2C device 0x%02X on bus %d configured successfully", 
                l_deviceConfig.deviceAddress, l_deviceConfig.busPort);
    }

    return ESP_OK;
}

esp_err_t HardwareManager::configureWIFI(const WIFIConfig& p_config) {
    ESP_LOGD(TAG, "Configuring WiFi");
    
    if (!m_wifiController) {
        m_wifiController = std::make_shared<WIFIController>(p_config);
    }
    esp_err_t l_ret = l_wifiController->init();
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize WiFi controller");
        return l_ret;
    }

    l_ret = m_wifiController->connect();
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to connect to WiFi");
        return l_ret;
    }

    return ESP_OK;
}

esp_err_t HardwareManager::initializeNVS() {
    ESP_LOGD(TAG, "Initializing NVS");
    
    if (!m_nvs) {
        m_nvs = std::make_shared<NVS>();
    }
    
    esp_err_t l_ret = m_nvs->init();
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize NVS");
        return l_ret;
    }

    return ESP_OK;
}
