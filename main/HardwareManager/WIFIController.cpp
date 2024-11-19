#include "include/WIFIController.hpp"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "esp_err.h"

#include <cstring>

void WIFIController::eventHandler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI("WIFIController", "Retrying connection...");
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI("WIFIController", "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
    }
}

WIFIController::WIFIController(const WIFIConfig& p_config) : m_config(p_config){}
esp_err_t WIFIController::init() {
    ESP_LOGD(TAG, "Initializing WiFiManager");

    //Initialize TCP/IP STACK
    esp_err_t l_ret = esp_netif_init();
    if (l_ret != ESP_OK) {
        setStateError();
        ESP_LOGE(TAG, "Failed to initialize ESP-NETIF: %s", esp_err_to_name(l_ret));
        return l_ret;
    }

    //Initialize event loop
    l_ret = esp_event_loop_create_default();
    if (l_ret != ESP_OK) {
        setStateError();
        ESP_LOGE(TAG, "Failed to create event loop: %s", esp_err_to_name(l_ret));
        return l_ret;
    }

    l_ret = esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &eventHandler, NULL, NULL);
    if (l_ret != ESP_OK) {
        setStateError();
        ESP_LOGE(TAG, "Failed to register WIFI_EVENT handler: %s", esp_err_to_name(l_ret));
        return l_ret;
    }
    l_ret = esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &eventHandler, NULL, NULL);
    if (l_ret != ESP_OK) {
        setStateError();
        ESP_LOGE(TAG, "Failed to register IP_EVENT handler: %s", esp_err_to_name(l_ret));
        return l_ret;
    }
    setStateInitialized();
    ESP_LOGI(TAG, "WIFIController initialized successfully");
}

esp_err_t WIFIController::connect() {
    if(!isInitialized()) {
        return notInitialized();
    }

    // Create default Wi-Fi station network interface
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t l_cfg = WIFI_INIT_CONFIG_DEFAULT();
    l_ret = esp_wifi_init(&l_cfg);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize WiFi: %s", esp_err_to_name(l_ret));
        return l_ret;
    }

    wifi_config_t l_wifiConfig = {};

    strncpy((char*)l_wifiConfig.sta.ssid, m_config.staSSID.c_str(), sizeof(l_wifiConfig.sta.ssid) - 1);
    strncpy((char*)l_wifiConfig.sta.password, m_config.staPassword.c_str(), sizeof(l_wifiConfig.sta.password) - 1);
    l_wifiConfig.sta.threshold.authmode = m_config.authMode;

    ESP_LOGI(TAG, "Connecting to SSID: %s", m_config.staSSID.c_str());
    
    esp_err_t ret = esp_wifi_set_mode(WIFI_MODE_STA);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set WiFi mode: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = esp_wifi_set_config(WIFI_IF_STA, &l_wifiConfig);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set WiFi configuration: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = esp_wifi_start();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start WiFi: %s", esp_err_to_name(ret));
        return ret;
    }
    ESP_LOGI(TAG, "WiFi STA connection finished. Waiting for connection result...");
    return ESP_OK;
}

esp_err_t WIFIController::notInitialized() const {
    ESP_LOGE(TAG, "WIFI Controller is not initialized: %s", esp_err_to_name(ESP_ERR_INVALID_STATE));
    return ESP_ERR_INVALID_STATE;
}