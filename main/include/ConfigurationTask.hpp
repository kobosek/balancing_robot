#pragma once

#include "interfaces/ITask.hpp"

class IRuntimeConfig;
class IWebServer;

class ConfigurationTask : public IConfigurationTask {
public:
    ConfigurationTask(IRuntimeConfig&, IWebServer&, QueueHandle_t);
    ~ConfigurationTask();
    
    esp_err_t init(const IRuntimeConfig&) override;

private:
    static constexpr const char* TAG = "ConfigurationTask";
    static constexpr int STACK_SIZE = 4096;
    static constexpr UBaseType_t PRIORITY = 1;
    static constexpr TickType_t CHECK_PERIOD = pdMS_TO_TICKS(1000); // 1Hz

    IRuntimeConfig& m_runtimeConfig;
    IWebServer& m_webServer;

    QueueHandle_t m_configUpdateQueue;
    TaskHandle_t m_taskHandle;

    static void taskFunction(void* pvParameters);
    void run();

    void applyConfigUpdate(const PIDConfig&);
    void broadcastConfig();
};