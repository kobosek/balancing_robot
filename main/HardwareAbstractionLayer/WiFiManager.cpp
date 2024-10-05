#include "include/WifiManager.hpp"
#include "include/RuntimeConfig.hpp"

#include "nvs_flash.h"
#include "esp_netif.h"
#include <cstring>

EventGroupHandle_t WiFiManager::s_wifiEventGroup = nullptr;
int WiFiManager::s_retryNum = 0;


esp_err_t WiFiManager::initNVS() {
    ESP_LOGI(TAG, "Initializing NVS");
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "NVS partition was truncated and needs to be erased");
        ret = nvs_flash_erase();
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to erase NVS: %s", esp_err_to_name(ret));
            return ret;
        }
        ret = nvs_flash_init();
    }
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize NVS: %s", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "NVS initialized successfully");
    }
    return ret;
}

esp_err_t WiFiManager::init(const IRuntimeConfig& config) {
    ESP_LOGI(TAG, "Initializing WiFiManager");

    esp_err_t ret = initNVS();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize NVS");
        return ret;
    }

    s_wifiEventGroup = xEventGroupCreate();
    ret = esp_netif_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize ESP-NETIF: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = esp_event_loop_create_default();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create event loop: %s", esp_err_to_name(ret));
        return ret;
    }

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ret = esp_wifi_init(&cfg);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize WiFi: %s", esp_err_to_name(ret));
        return ret;
    }

    esp_event_handler_instance_t instanceAnyId;
    esp_event_handler_instance_t instanceGotIp;
    ret = esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &eventHandler, this, &instanceAnyId);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register WIFI_EVENT handler: %s", esp_err_to_name(ret));
        return ret;
    }
    ret = esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &eventHandler, this, &instanceGotIp);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register IP_EVENT handler: %s", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(TAG, "WiFiManager initialized successfully");
    return connect(config);
}

esp_err_t WiFiManager::connect(const IRuntimeConfig& config) {
    std::string ssid = config.getWifiSsid();
    std::string password = config.getWifiPassword();

    if (ssid.empty()) {
        ESP_LOGE(TAG, "WiFi SSID is empty. Please check your configuration.");
        return ESP_ERR_INVALID_ARG;
    }

    ESP_LOGI(TAG, "Connecting to WiFi network: %s", ssid.c_str());
    wifi_config_t wifi_config = {};
    strncpy((char*)wifi_config.sta.ssid, ssid.c_str(), sizeof(wifi_config.sta.ssid) - 1);
    strncpy((char*)wifi_config.sta.password, password.c_str(), sizeof(wifi_config.sta.password) - 1);
    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

    esp_err_t ret = esp_wifi_set_mode(WIFI_MODE_STA);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set WiFi mode: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set WiFi configuration: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = esp_wifi_start();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start WiFi: %s", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(TAG, "WiFi initialization finished. Waiting for connection result...");

    EventBits_t bits = xEventGroupWaitBits(s_wifiEventGroup,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "Successfully connected to SSID: %s", ssid.c_str());
        return ESP_OK;
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGE(TAG, "Failed to connect to SSID: %s", ssid.c_str());
        return ESP_FAIL;
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
        return ESP_ERR_INVALID_STATE;
    }
}

void WiFiManager::eventHandler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    WiFiManager* self = static_cast<WiFiManager*>(arg);
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        ESP_LOGI(TAG, "WiFi station started, attempting to connect");
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retryNum < MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retryNum++;
            ESP_LOGW(TAG, "Retry %d/%d to connect to the AP", s_retryNum, MAXIMUM_RETRY);
        } else {
            xEventGroupSetBits(s_wifiEventGroup, WIFI_FAIL_BIT);
            ESP_LOGE(TAG, "Failed to connect to the AP after maximum retries");
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Got IP address: " IPSTR, IP2STR(&event->ip_info.ip));
        s_retryNum = 0;
        xEventGroupSetBits(s_wifiEventGroup, WIFI_CONNECTED_BIT);
    }
}