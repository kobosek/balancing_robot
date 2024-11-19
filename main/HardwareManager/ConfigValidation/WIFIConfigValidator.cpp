#include "WIFIConfigValidator.hpp"
#include "esp_log.h"
#include <algorithm>

esp_err_t WIFIConfigValidator::validateConfig(const HardwareConfig& p_config) {
    ESP_LOGD(TAG, "Validating WIFI configuration");
    WIFIConfig l_config = p_config.wifiConfig;

    esp_err_t l_ret = validateSSID(l_config);
    if (l_ret != ESP_OK) return l_ret;

    l_ret = validatePassword(l_config);
    if (l_ret != ESP_OK) return l_ret;

    l_ret = validateAuthMode(l_config);
    if (l_ret != ESP_OK) return l_ret;

    return ESP_OK;
}

esp_err_t WIFIConfigValidator::validateSSID(const WIFIConfig& p_config) {
    if (p_config.staSSID.empty()) {
        ESP_LOGE(TAG, "SSID cannot be empty");
        return ESP_ERR_INVALID_ARG;
    }

    if (p_config.staSSID.length() > 32) {
        ESP_LOGE(TAG, "SSID length cannot exceed 32 characters");
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

esp_err_t WIFIConfigValidator::validatePassword(const WIFIConfig& p_config) {
    if (p_config.staPassword.empty() && p_config.authMode != WIFI_AUTH_OPEN) {
        ESP_LOGE(TAG, "Password cannot be empty for non-open authentication mode");
        return ESP_ERR_INVALID_ARG;
    }

    if (p_config.staPassword.length() > 64) {
        ESP_LOGE(TAG, "Password length cannot exceed 64 characters");
        return ESP_ERR_INVALID_ARG;
    }

    if (p_config.authMode != WIFI_AUTH_OPEN && p_config.staPassword.length() < 8) {
        ESP_LOGE(TAG, "Password must be at least 8 characters for secure authentication modes");
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

esp_err_t WIFIConfigValidator::validateAuthMode(const WIFIConfig& p_config) {
    if(p_config.authMode >= WIFI_AUTH_MAX) {
        ESP_LOGE(TAG, "Invalid authentication mode");
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}