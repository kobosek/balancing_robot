#pragma once

#include "interfaces/ITask.hpp"

class IPIDController;
class IStateMachine;

class PIDTask : public IPIDTask {
public:
    PIDTask(const IPIDController&, QueueHandle_t, QueueHandle_t, 
            QueueHandle_t, const IStateMachine&);
    ~PIDTask();

    esp_err_t init(const IRuntimeConfig&) override;
    QueueHandle_t getPIDOutputQueue() const { return m_pidOutputQueue; }

private:
    static constexpr const char* TAG = "PIDTask";
    static constexpr int STACK_SIZE = 4096;
    static constexpr UBaseType_t PRIORITY = 4;  // High priority, but lower than sensor task
    static constexpr TickType_t CONTROL_PERIOD = pdMS_TO_TICKS(10);  // 100Hz

    IPIDController& m_pidController;
    QueueHandle_t m_sensorDataQueue;
    QueueHandle_t m_pidOutputQueue;
    QueueHandle_t m_configQueue;
    IStateMachine& m_stateMachine;
    TaskHandle_t m_taskHandle;

    float m_integral;
    float m_lastError;

    static void taskFunction(void* pvParameters);
    void run();

    void updateConfig();
};