#include "include/TelemetryTask.hpp"
#include "interfaces/IWebServer.hpp"

TelemetryTask::TelemetryTask(IWebServer& server, QueueHandle_t sensorQueue, QueueHandle_t pidQueue, QueueHandle_t motorQueue)
    : m_webServer(server), m_sensorDataQueue(sensorQueue), m_pidOutputQueue(pidQueue), m_motorSpeedQueue(motorQueue), m_taskHandle(nullptr) {}

TelemetryTask::~TelemetryTask() {
    if (m_taskHandle != nullptr) {
        vTaskDelete(m_taskHandle);
    }
}

esp_err_t TelemetryTask::init(const IRuntimeConfig&) {
    BaseType_t result = xTaskCreate(
        taskFunction,
        TAG,
        STACK_SIZE,
        this,
        PRIORITY,
        &m_taskHandle
    );

    if (result != pdPASS) {
        ESP_LOGE(TAG, "Failed to create TelemetryTask");
    }
    return ESP_OK;
}

void TelemetryTask::taskFunction(void* pvParameters) {
    auto* task = static_cast<TelemetryTask*>(pvParameters);
    task->run();
}

void TelemetryTask::run() {
    TickType_t lastWakeTime = xTaskGetTickCount();

    while (true) {
        collectAndSendTelemetry();
        vTaskDelayUntil(&lastWakeTime, UPDATE_PERIOD);
    }
}

void TelemetryTask::collectAndSendTelemetry() {

    TelemetryData telemetryData;

    // Collect latest data from queues
    if (xQueuePeek(m_sensorDataQueue, &telemetryData.sensorData, 0) != pdTRUE) {
        ESP_LOGW(TAG, "Failed to read sensor data");
    }

    if (xQueuePeek(m_pidOutputQueue, &telemetryData.pidOutput, 0) != pdTRUE) {
        ESP_LOGW(TAG, "Failed to read PID output");
    }

    if (xQueuePeek(m_motorSpeedQueue, &telemetryData.motorSpeed, 0) != pdTRUE) {
        ESP_LOGW(TAG, "Failed to read motor speed");
    }

    m_webServer.update_telemetry(telemetryData);

    ESP_LOGD(TAG, "Telemetry sent - Pitch: %.2f, PID Output: %.2f, Motor Speed: %.2f",
             telemetryData.sensorData.pitch, telemetryData.pidOutput, telemetryData.motorSpeed);
}