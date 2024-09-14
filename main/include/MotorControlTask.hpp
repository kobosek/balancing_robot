#pragma once

#include "interfaces/ITask.hpp"

class IMotorDriver;
class IStateMachine;

class MotorControlTask : public IMotorControlTask {
public:
    MotorControlTask(IMotorDriver&, QueueHandle_t, IStateMachine&);
    ~MotorControlTask();

    esp_err_t init(const IRuntimeConfig&) override;
private:
    static constexpr const char* TAG = "MotorControlTask";
    static constexpr int STACK_SIZE = 4096;
    static constexpr UBaseType_t PRIORITY = 5;  // High priority
    static constexpr TickType_t CONTROL_PERIOD = pdMS_TO_TICKS(10);  // 100Hz
    static constexpr float MAX_SAFE_ANGLE = 45.0f;  // Maximum safe angle in degrees
    static constexpr float MAX_MOTOR_SPEED = 1023.0f;  // Assuming 10-bit PWM

    IMotorDriver& m_motorDriver;
    QueueHandle_t m_pidOutputQueue;
    IStateMachine& m_stateMachine;
    TaskHandle_t m_taskHandle;

    float currentSpeed;

    static void taskFunction(void* pvParameters);
    void run();

    float applySpeedLimits(float speed);
    bool isSafeToOperate();
};