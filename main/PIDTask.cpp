#include "include/PIDTask.hpp"
#include "include/StateMachine.hpp"
#include "interfaces/IPIDController.hpp"

PIDTask::PIDTask(IPIDController& p_pid, QueueHandle_t p_sensorQueue, QueueHandle_t p_outputQueue, 
                 QueueHandle_t p_cfgQueue, IStateMachine& p_sm)
    : m_pidController(p_pid), m_sensorDataQueue(p_sensorQueue), m_pidOutputQueue(p_outputQueue),
      m_configQueue(p_cfgQueue), m_stateMachine(p_sm), m_taskHandle(nullptr), 
      m_integral(0.0f), m_lastError(0.0f) {}

PIDTask::~PIDTask() {
    if (m_taskHandle != nullptr) {
        vTaskDelete(m_taskHandle);
    }
}

esp_err_t PIDTask::init(const IRuntimeConfig&) {
    BaseType_t result = xTaskCreate(
        taskFunction,
        TAG,
        STACK_SIZE,
        this,
        PRIORITY,
        &m_taskHandle
    );

    if (result != pdPASS) {
        ESP_LOGE(TAG, "Failed to create PIDTask");
    }
    return ESP_OK;
}

void PIDTask::taskFunction(void* pvParameters) {
    auto* task = static_cast<PIDTask*>(pvParameters);
    task->run();
}

void PIDTask::run() {
    TickType_t lastWakeTime = xTaskGetTickCount();
    
    while (true) {
        updateConfig();

        if (m_stateMachine.getState() == StateMachine::State::BALANCING) {
            SensorData sensorData;
            if (xQueueReceive(m_sensorDataQueue, &sensorData, 0) == pdTRUE) {
                float dt = CONTROL_PERIOD / 1000.0f;  // Convert to seconds
                float output = m_pidController.compute(m_integral, m_lastError, sensorData.pitch, dt);
                
                PIDOutput pidOutput = { output };
                xQueueOverwrite(m_pidOutputQueue, &pidOutput);
                
                ESP_LOGV(TAG, "PID Output: %.2f", output);
            }
        } else {
            // Reset integral term when not balancing
            m_integral = 0.0f;
            m_lastError = 0.0f;
        }

        vTaskDelayUntil(&lastWakeTime, CONTROL_PERIOD);
    }
}

void PIDTask::updateConfig() {
    PIDConfig newConfig;
    if (xQueueReceive(m_configQueue, &newConfig, 0) == pdTRUE) {
        m_pidController.setConfig(newConfig);
    }
}
