#include "include/WifiManager.hpp"
#include "include/RuntimeConfig.hpp"

#include "nvs_flash.h"
#include "esp_netif.h"
#include <cstring>

EventGroupHandle_t WiFiManager::s_wifiEventGroup = nullptr;
int WiFiManager::s_retryNum = 0;


esp_err_t WiFiManager::initNVS() {
    Logger::info(TAG, "Initializing NVS");
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        Logger::warn(TAG, "NVS partition was truncated and needs to be erased");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    Logger::info(TAG, "NVS initialized successfully");
    return ret;
}

esp_err_t WiFiManager::init(const IRuntimeConfig& config) {
    Logger::info(TAG, "Initializing WiFiManager");

    // Initialize NVS
    esp_err_t ret = initNVS();
    if (ret != ESP_OK) {
        Logger::error(TAG, "Failed to initialize NVS");
        return ret;
    }

    s_wifiEventGroup = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instanceAnyId;
    esp_event_handler_instance_t instanceGotIp;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &eventHandler, this, &instanceAnyId));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &eventHandler, this, &instanceGotIp));

    Logger::info(TAG, "WiFiManager initialized successfully");
    return connect(config);
}

esp_err_t WiFiManager::connect(const IRuntimeConfig& config) {
    std::string ssid = config.getWifiSsid();
    std::string password = config.getWifiPassword();

    if (ssid.empty()) {
        Logger::error(TAG, "WiFi SSID is empty. Please check your configuration.");
        return ESP_ERR_INVALID_ARG;
    }

    Logger::info(TAG, "Connecting to WiFi network: %s", ssid.c_str());
    wifi_config_t wifi_config = {};
    strncpy((char*)wifi_config.sta.ssid, ssid.c_str(), sizeof(wifi_config.sta.ssid) - 1);
    strncpy((char*)wifi_config.sta.password, password.c_str(), sizeof(wifi_config.sta.password) - 1);
    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    Logger::info(TAG, "WiFi initialization finished. Waiting for connection result...");

    EventBits_t bits = xEventGroupWaitBits(s_wifiEventGroup,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT) {
        Logger::info(TAG, "Successfully connected to SSID: %s", ssid.c_str());
        return ESP_OK;
    } else if (bits & WIFI_FAIL_BIT) {
        Logger::error(TAG, "Failed to connect to SSID: %s", ssid.c_str());
        return ESP_FAIL;
    } else {
        Logger::error(TAG, "UNEXPECTED EVENT");
        return ESP_ERR_INVALID_STATE;
    }
}

void WiFiManager::eventHandler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        Logger::info(TAG, "WiFi station started, attempting to connect");
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retryNum < MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retryNum++;
            Logger::warn(TAG, "Retry %d/%d to connect to the AP", s_retryNum, MAXIMUM_RETRY);
        } else {
            xEventGroupSetBits(s_wifiEventGroup, WIFI_FAIL_BIT);
            Logger::error(TAG, "Failed to connect to the AP after maximum retries");
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        Logger::info(TAG, "Got IP address: " IPSTR, IP2STR(&event->ip_info.ip));
        s_retryNum = 0;
        xEventGroupSetBits(s_wifiEventGroup, WIFI_CONNECTED_BIT);
    }
}

esp_err_t WiFiManager::onConfigUpdate(const IRuntimeConfig& config) {
    Logger::info(TAG, "Updating WiFi configuration");
    // Disconnect from current network
    // Connect to new network with updated configuration
    return ESP_OK;
}