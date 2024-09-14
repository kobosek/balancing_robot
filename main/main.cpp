#include "esp_log.h"

#include "include/RuntimeConfig.hpp"
#include "include/ComponentHandler.hpp"

extern "C" void app_main(void)
{
    esp_log_level_set("*", ESP_LOG_INFO);
    ESP_LOGI("main", "Starting Balancing Robot");

    // Initialize RuntimeConfig
    RuntimeConfig config;
    config.init("/spiffs/config.json");

    // Initialize ComponentHandler
    ComponentHandler handler;
    handler.init();

    ESP_LOGI("main", "All tasks and components initialized");

    // The FreeRTOS scheduler will now take over
}