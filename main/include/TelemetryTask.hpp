#pragma once

#include "interfaces/ITask.hpp"

class IWebServer;

class TelemetryTask : public ITelemetryTask {
public:
    TelemetryTask(IWebServer&, QueueHandle_t, QueueHandle_t, QueueHandle_t);
    ~TelemetryTask();

    esp_err_t init(const IRuntimeConfig&) override;

private:
    static constexpr const char* TAG = "TelemetryTask";
    static constexpr int STACK_SIZE = 4096;
    static constexpr UBaseType_t PRIORITY = 2;
    static constexpr TickType_t UPDATE_PERIOD = pdMS_TO_TICKS(100); // 10Hz

    IWebServer& m_webServer;
    QueueHandle_t m_sensorDataQueue;
    QueueHandle_t m_pidOutputQueue;
    QueueHandle_t m_motorSpeedQueue;
    TaskHandle_t m_taskHandle;

    static void taskFunction(void* pvParameters);
    void run();

    void collectAndSendTelemetry();
};