#pragma once

#include "interfaces/IStateMachine.hpp"
#include "interfaces/ITask.hpp"


class StateMachine : public IStateMachine {
public:
    StateMachine(QueueHandle_t sensorQueue, QueueHandle_t pidQueue, QueueHandle_t motorQueue,
                 QueueHandle_t telemetryQueue, QueueHandle_t configQueue);
    ~StateMachine();

    esp_err_t init(const IRuntimeConfig&) override;
    esp_err_t setState(State) override;
    
    State getState() const override;

private:
    static constexpr const char* TAG = "StateMachine";
    static constexpr uint32_t STATE_CHANGE_BIT = BIT0;
    static constexpr uint32_t CONFIG_UPDATE_BIT = BIT1;
    static constexpr int STACK_SIZE = 4096;
    static constexpr UBaseType_t PRIORITY = 10;  // High priority
    static constexpr float FALL_THRESHOLD = 45.0f;  // Degrees
    static constexpr float BALANCE_THRESHOLD = 5.0f;  // Degrees
    static constexpr TickType_t STATE_CHECK_PERIOD = pdMS_TO_TICKS(10);  // 100Hz

    State currentState;
    EventGroupHandle_t eventGroup;
    TaskHandle_t taskHandle;

    QueueHandle_t sensorDataQueue;
    QueueHandle_t pidOutputQueue;
    QueueHandle_t motorControlQueue;
    QueueHandle_t telemetryQueue;
    QueueHandle_t configQueue;

    float targetAngle;

    static void taskFunction(void* pvParameters);
    void run();

    void handleInit();
    void handleIdle();
    void handleBalancing();
    void handleFalling();
    void handleError();
    void checkConfigUpdate();
    void updateTelemetry();
};

