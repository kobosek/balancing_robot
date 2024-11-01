#pragma once

#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "freertos/event_groups.h"
#include "nvs_flash.h"

class IWiFiManager {
    public:
        virtual ~IWiFiManager() = default;
};

class WiFiManager : public IWiFiManager{
public:
    esp_err_t init(const IRuntimeConfig&) override;

private:
    static constexpr const char* TAG = "WiFiManager";

    static void eventHandler(void*, esp_event_base_t, int32_t, void*);
    esp_err_t connect(const IRuntimeConfig&);
    esp_err_t initNVS();
    
    static EventGroupHandle_t s_wifiEventGroup;
    static const int WIFI_CONNECTED_BIT = BIT0;
    static const int WIFI_FAIL_BIT = BIT1;
    static const int MAXIMUM_RETRY = 5;
    static int s_retryNum;
};