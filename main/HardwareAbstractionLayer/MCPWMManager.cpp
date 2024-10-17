#include "include/MCPWMManager.hpp"
#include "esp_err.h"
#include "esp_log.h"

esp_err_t MCPWMManager::configureTimersAndChannels(const LEDCConfig& p_config) {
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
