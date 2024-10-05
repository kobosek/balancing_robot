#include "include/MotorControlTask.hpp"
#include "interfaces/IMotorDriver.hpp"
#include "include/StateMachine.hpp"

#include <algorithm>

MotorControlTask::MotorControlTask(IMotorDriver& p_motor, QueueHandle_t p_pidQueue, IStateMachine& p_sm)
    : m_motorDriver(p_motor), m_pidOutputQueue(p_pidQueue), m_stateMachine(p_sm), m_taskHandle(nullptr), currentSpeed(0.0f) {}

MotorControlTask::~MotorControlTask() {
    if (m_taskHandle != nullptr) {
        vTaskDelete(m_taskHandle);
    }
}

esp_err_t MotorControlTask::init(const IRuntimeConfig&) {
    BaseType_t result = xTaskCreate(
        taskFunction,
        TAG,
        STACK_SIZE,
        this,
        PRIORITY,
        &m_taskHandle
    );

    if (result != pdPASS) {
        ESP_LOGE(TAG, "Failed to create MotorControlTask");
    }
    return ESP_OK;
}

void MotorControlTask::taskFunction(void* pvParameters) {
    auto* task = static_cast<MotorControlTask*>(pvParameters);
    task->run();
}

void MotorControlTask::run() {
    TickType_t lastWakeTime = xTaskGetTickCount();
    
    while (true) {
        if (m_stateMachine.getState() == StateMachine::State::BALANCING) {
            float pidOutput;
            if (xQueueReceive(m_pidOutputQueue, &pidOutput, 0) == pdTRUE) {
                if (isSafeToOperate()) {
                    currentSpeed = applySpeedLimits(pidOutput);
                    m_motorDriver.setSpeed(currentSpeed);
                } else {
                    m_motorDriver.setSpeed(0);
                    m_stateMachine.setState(StateMachine::State::FALLING);
                }
            }
        } else {
            // If not in BALANCING state, ensure motors are stopped
            m_motorDriver.setSpeed(0);
            currentSpeed = 0;
        }

        vTaskDelayUntil(&lastWakeTime, CONTROL_PERIOD);
    }
}

float MotorControlTask::applySpeedLimits(float speed) {
    return std::clamp(speed, -MAX_MOTOR_SPEED, MAX_MOTOR_SPEED);
}

bool MotorControlTask::isSafeToOperate() {
    // This is a simplistic safety check. In a real implementation,
    // you might want to consider more factors, such as:
    // - Rate of angle change
    // - Time spent at extreme angles
    // - Battery voltage
    // - Motor temperature (if you have sensors for this)
    return true;
}