#include "Include/GPIOConfigValidator.hpp"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include <unordered_set>

esp_err_t GPIOConfigValidator::validateConfig(const HardwareConfig& p_config) {
    ESP_LOGD(TAG, "Validating GPIO configuration");

    const GPIOSConfig& l_configs = p_config.gpioConfigs;
    
    for (const auto& l_gpioConfig : l_configs) {
        esp_err_t l_ret = validateSingleGPIOConfig(l_gpioConfig);
        if (l_ret != ESP_OK) {
            return l_ret;
        }
    }
    return validateUniquePinNumbers(l_configs);
}

esp_err_t GPIOConfigValidator::validateSingleGPIOConfig(const GPIOConfig& p_config) {
    esp_err_t l_ret = validateGPIOMode(p_config);
    if (l_ret != ESP_OK) { return l_ret; }

    l_ret = validatePinNumber(p_config);
    if (l_ret != ESP_OK) { return l_ret; }

    l_ret = validateInterruptType(p_config);
    if (l_ret != ESP_OK) { return l_ret; }

    return ESP_OK;
}

esp_err_t GPIOConfigValidator::validateUniquePinNumbers(const GPIOSConfig& p_config) {
    std::unordered_set<int> l_usedPins;
    ESP_LOGD(TAG, "Validating unique pin numbers in GPIO configuration");
    
    for (const auto& l_gpioConfig : p_config) {
        if (!l_usedPins.insert(l_gpioConfig.pinNum).second) {
            ESP_LOGE(TAG, "Duplicate GPIO pin number: %d", l_gpioConfig.pinNum);
            return ESP_ERR_INVALID_ARG;
        }
    }
    return ESP_OK;
}

esp_err_t GPIOConfigValidator::validatePinNumber(const GPIOConfig& p_config) {
    ESP_LOGD(TAG, "Validating GPIO pin number: %d", p_config.pinNum);

    // Non-existent pins
    const std::unordered_set<int> l_nonExistentPins = {20, 24, 28, 29, 30, 31};
    
    // Flash interface pins (6-11) - generally unavailable
    const std::unordered_set<int> l_flashPins = {6, 7, 8, 9, 10, 11};
    
    // Input-only GPIOs (34-39)
    const std::unordered_set<int> l_inputOnlyPins = {34, 35, 36, 37, 38, 39};

    if (p_config.pinNum < GPIO_NUM_0 || p_config.pinNum >= GPIO_NUM_MAX) {
        ESP_LOGE(TAG, "GPIO pin number out of range: %d", p_config.pinNum);
        return ESP_ERR_INVALID_ARG;
    }

    if (l_nonExistentPins.find(p_config.pinNum) != l_nonExistentPins.end()) {
        ESP_LOGE(TAG, "Non-existent GPIO pin number: %d", p_config.pinNum);
        return ESP_ERR_INVALID_ARG;
    }

    if (l_flashPins.find(p_config.pinNum) != l_flashPins.end()) {
        ESP_LOGW(TAG, "GPIO pin %d is connected to the flash interface, use with caution", p_config.pinNum);
    }

    if (l_inputOnlyPins.find(p_config.pinNum) != l_inputOnlyPins.end()) {
        if (p_config.gpioMode == GPIO_MODE_OUTPUT || 
            p_config.gpioMode == GPIO_MODE_OUTPUT_OD || 
            p_config.gpioMode == GPIO_MODE_INPUT_OUTPUT || 
            p_config.gpioMode == GPIO_MODE_INPUT_OUTPUT_OD) {
            ESP_LOGE(TAG, "GPIO pin %d is input-only but configured for output", p_config.pinNum);
            return ESP_ERR_INVALID_ARG;
        }
    }
    return ESP_OK;
}

esp_err_t GPIOConfigValidator::validateGPIOMode(const GPIOConfig& p_config) {
    ESP_LOGD(TAG, "Validating GPIO mode: %d", p_config.gpioMode);
    switch (p_config.gpioMode) {
        case GPIO_MODE_DISABLE:
        case GPIO_MODE_INPUT:
        case GPIO_MODE_OUTPUT:
        case GPIO_MODE_OUTPUT_OD:
        case GPIO_MODE_INPUT_OUTPUT:
        case GPIO_MODE_INPUT_OUTPUT_OD:
            return ESP_OK;
        default:
            ESP_LOGE(TAG, "Invalid GPIO mode: %d", p_config.gpioMode);
            return ESP_ERR_INVALID_ARG;
    }
}

esp_err_t GPIOConfigValidator::validateInterruptType(const GPIOConfig& p_config) {
    ESP_LOGD(TAG, "Validating GPIO interrupt type: %d", p_config.interruptType);
    switch (p_config.interruptType) {
        case GPIO_INTR_DISABLE:
        case GPIO_INTR_POSEDGE:
        case GPIO_INTR_NEGEDGE:
        case GPIO_INTR_ANYEDGE:
        case GPIO_INTR_LOW_LEVEL:
        case GPIO_INTR_HIGH_LEVEL:
            return ESP_OK;
        default:
            ESP_LOGE(TAG, "Invalid GPIO interrupt type: %d", p_config.interruptType);
            return ESP_ERR_INVALID_ARG;
    }
}
