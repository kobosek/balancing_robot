#pragma once

#include "include/StateMachine.hpp"
#include "esp_log.h"

StateMachine::StateMachine(QueueHandle_t sensorQueue, QueueHandle_t pidQueue, QueueHandle_t motorQueue,
                           QueueHandle_t telemetryQueue, QueueHandle_t configQueue)
    : currentState(State::INIT), eventGroup(nullptr), taskHandle(nullptr),
      sensorDataQueue(sensorQueue), pidOutputQueue(pidQueue), motorControlQueue(motorQueue),
      telemetryQueue(telemetryQueue), configQueue(configQueue), targetAngle(0.0f) {}

StateMachine::~StateMachine() {
    if (eventGroup) {
        vEventGroupDelete(eventGroup);
    }
    if (taskHandle) {
        vTaskDelete(taskHandle);
    }
}

esp_err_t StateMachine::init(const IRuntimeConfig&) { 
    eventGroup = xEventGroupCreate();
    if (eventGroup == nullptr) {
        ESP_LOGE(TAG, "Failed to create event group");
        return ESP_FAIL;
    }

    xTaskCreate(taskFunction, TAG, STACK_SIZE, this, PRIORITY, &taskHandle);
    if (taskHandle == nullptr) {
        ESP_LOGE(TAG, "Failed to create StateMachine task");
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t StateMachine::setState(State newState) {
    if (currentState != newState) {
        ESP_LOGI(TAG, "State transition: %d -> %d", static_cast<int>(currentState), static_cast<int>(newState));
        currentState = newState;
        xEventGroupSetBits(eventGroup, STATE_CHANGE_BIT);
    }
    return ESP_OK;
}

StateMachine::State StateMachine::getState() const {
    return currentState;
}

void StateMachine::taskFunction(void* pvParameters) {
    StateMachine* stateMachine = static_cast<StateMachine*>(pvParameters);
    stateMachine->run();
}

void StateMachine::run() {
    TickType_t lastWakeTime = xTaskGetTickCount();

    while (true) {
        checkConfigUpdate();

        switch (currentState) {
            case State::INIT:      handleInit(); break;
            case State::IDLE:      handleIdle(); break;
            case State::BALANCING: handleBalancing(); break;
            case State::FALLING:   handleFalling(); break;
            case State::ERROR:     handleError(); break;
        }

        updateTelemetry();

        // Wait for state change, config update, or timeout
        xEventGroupWaitBits(eventGroup, STATE_CHANGE_BIT | CONFIG_UPDATE_BIT, pdTRUE, pdFALSE, STATE_CHECK_PERIOD);
        vTaskDelayUntil(&lastWakeTime, STATE_CHECK_PERIOD);
    }
}

void StateMachine::handleInit() {
    ESP_LOGI(TAG, "Handling INIT state");
    // Assume all tasks are already initialized
    setState(State::IDLE);
}

void StateMachine::handleIdle() {
    ESP_LOGV(TAG, "Handling IDLE state");
    
    SensorData sensorData;
    if (xQueuePeek(sensorDataQueue, &sensorData, 0) == pdTRUE) {
        if (std::abs(sensorData.pitch - targetAngle) < BALANCE_THRESHOLD) {
            setState(State::BALANCING);
        } else {
            float zeroSpeed = 0.0f;
            xQueueOverwrite(motorControlQueue, &zeroSpeed);
        }
    }
}

void StateMachine::handleBalancing() {
    ESP_LOGV(TAG, "Handling BALANCING state");
    
    SensorData sensorData;
    if (xQueuePeek(sensorDataQueue, &sensorData, 0) == pdTRUE) {
        if (std::abs(sensorData.pitch - targetAngle) > FALL_THRESHOLD) {
            setState(State::FALLING);
            return;
        }
    }

    // PIDTask and MotorControlTask handle the actual balancing
}

void StateMachine::handleFalling() {
    ESP_LOGW(TAG, "Handling FALLING state");
    
    float zeroSpeed = 0.0f;
    xQueueOverwrite(motorControlQueue, &zeroSpeed);
    
    vTaskDelay(pdMS_TO_TICKS(2000));
    
    SensorData sensorData;
    if (xQueuePeek(sensorDataQueue, &sensorData, 0) == pdTRUE) {
        if (std::abs(sensorData.pitch - targetAngle) < BALANCE_THRESHOLD) {
            setState(State::IDLE);
        } else {
            ESP_LOGE(TAG, "Robot failed to recover from fall. Pitch: %.2f", sensorData.pitch);
            setState(State::ERROR);
        }
    }
}

void StateMachine::handleError() {
    ESP_LOGE(TAG, "Handling ERROR state");
    
    float zeroSpeed = 0.0f;
    xQueueOverwrite(motorControlQueue, &zeroSpeed);
    
    vTaskDelay(pdMS_TO_TICKS(5000));
    setState(State::INIT);
}

void StateMachine::checkConfigUpdate() {
    PIDConfig update;
    if (xQueueReceive(configQueue, &update, 0) == pdTRUE) {
        targetAngle = update.targetAngle;
        // Forward the PID parameters to the PID task
        xQueueSend(pidOutputQueue, &update, 0);
        xEventGroupSetBits(eventGroup, CONFIG_UPDATE_BIT);
        ESP_LOGI(TAG, "Configuration updated. New target angle: %.2f", targetAngle);
    }
}

void StateMachine::updateTelemetry() {
    TelemetryData telemetryData;
    xQueuePeek(sensorDataQueue, &telemetryData.sensorData, 0);
    xQueuePeek(pidOutputQueue, &telemetryData.pidOutput, 0);
    xQueuePeek(motorControlQueue, &telemetryData.motorSpeed, 0);
    xQueueOverwrite(telemetryQueue, &telemetryData);
}