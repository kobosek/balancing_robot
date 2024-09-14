#include "include/ConfigurationTask.hpp"

#include "interfaces/IRuntimeConfig.hpp"
#include "interfaces/IWebServer.hpp"

ConfigurationTask::ConfigurationTask(IRuntimeConfig& p_config, IWebServer& p_server, QueueHandle_t p_configQueue)
    : m_runtimeConfig(p_config), m_webServer(p_server), m_configUpdateQueue(p_configQueue), m_taskHandle(nullptr) {}

ConfigurationTask::~ConfigurationTask() {
    if (m_taskHandle != nullptr) {
        vTaskDelete(m_taskHandle);
    }
}

esp_err_t ConfigurationTask::init(const IRuntimeConfig&) {
    BaseType_t result = xTaskCreate(
        taskFunction,
        TAG,
        STACK_SIZE,
        this,
        PRIORITY,
        &m_taskHandle
    );

    if (result != pdPASS) {
        ESP_LOGE(TAG, "Failed to create ConfigurationTask");
    }
    return ESP_OK;
}

void ConfigurationTask::taskFunction(void* pvParameters) {
    auto* task = static_cast<ConfigurationTask*>(pvParameters);
    task->run();
}

void ConfigurationTask::run() {
    TickType_t lastWakeTime = xTaskGetTickCount();

    while (true) {
        PIDConfig update;
        if (xQueueReceive(m_configUpdateQueue, &update, 0) == pdTRUE) {
            applyConfigUpdate(update);
        }

        // Check for configuration requests from WebServer
        if (m_webServer.hasConfigurationRequest()) {
            PIDConfig webUpdate = m_webServer.getConfigurationRequest();
            applyConfigUpdate(webUpdate);
        }

        // Periodically broadcast current configuration
        broadcastConfig();

        vTaskDelayUntil(&lastWakeTime, CHECK_PERIOD);
    }
}

void ConfigurationTask::applyConfigUpdate(const PIDConfig& update) {
    ESP_LOGI(TAG, "Applying configuration update");

    // Update RuntimeConfig
    m_runtimeConfig.setPidConfig(update);


    // Save updated configuration to NVS
    esp_err_t ret = m_runtimeConfig.save();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to save updated configuration: %s", esp_err_to_name(ret));
    }

    // Broadcast the new configuration to other tasks
    broadcastConfig();

    // Notify WebServer of successful update
    m_webServer.notifyConfigurationUpdated();
}

void ConfigurationTask::broadcastConfig() {
    PIDConfig l_currentConfig = m_runtimeConfig.getPidConfig();

    if (xQueueOverwrite(m_configUpdateQueue, &l_currentConfig) != pdTRUE) {
        ESP_LOGW(TAG, "Failed to broadcast configuration update");
    }
}