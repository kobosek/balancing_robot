#include "Include/GPIOConfigValidator.hpp"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"

esp_err_t GPIOConfigValidator::validateConfig(const GPIOConfig& p_config) {
    esp_err_t l_ret = validatePinNumber(p_config);
    if (l_ret != ESP_OK) { return l_ret; }

    l_ret = validateGPIOMode(p_config);
    if (l_ret != ESP_OK) { return l_ret; }

    l_ret = validateInterruptType(p_config);
    if (l_ret != ESP_OK) { return l_ret; }

    return ESP_OK;
}

esp_err_t GPIOConfigValidator::validatePinNumber(const GPIOConfig& p_config) {
    ESP_LOGD(TAG, "Validating GPIO pin number: %d", p_config.pinNum);
    if (p_config.pinNum < GPIO_NUM_0 || p_config.pinNum >= GPIO_NUM_MAX) {
        ESP_LOGE(TAG, "Invalid GPIO pin number: %d", p_config.pinNum);
        return ESP_ERR_INVALID_ARG;
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
