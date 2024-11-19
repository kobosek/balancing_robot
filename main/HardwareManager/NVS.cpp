#include "include/NVS.hpp"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_err.h"

esp_err_t NVS::init() {
    ESP_LOGI(TAG, "Initializing NVS");

    esp_err_t l_ret = nvs_flash_init();
    if (l_ret == ESP_ERR_NVS_NO_FREE_PAGES || l_ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "NVS partition was truncated and needs to be erased");
        l_ret = nvs_flash_erase();
        if (l_ret != ESP_OK) {
            setStateError();
            ESP_LOGE(TAG, "Failed to erase NVS: %s", esp_err_to_name(ret));
            return l_ret;
        }
    }
    
    l_ret = nvs_flash_init();
    if (l_ret != ESP_OK) {
        setStateError();
        ESP_LOGE(TAG, "Failed to initialize NVS: %s", esp_err_to_name(ret));
        return l_ret;
    }
    setStateInitialized();
    ESP_LOGI(TAG, "NVS initialized successfully");
    return ESP_OK;
}

esp_err_t NVS::notInitialized() const {
    ESP_LOGE(TAG, "NVS is not initialized: %s", esp_err_to_name(ESP_ERR_INVALID_STATE));
    return ESP_ERR_INVALID_STATE;
}