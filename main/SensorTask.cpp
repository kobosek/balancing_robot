#include "interfaces/IMPU6050Manager.hpp"

#include "include/SensorTask.hpp"
#include "include/StateMachine.hpp"

SensorTask::SensorTask(IMPU6050Manager& p_mpu, QueueHandle_t p_dataQueue, IStateMachine& p_sm)
    : m_mpu6050(p_mpu), m_sensorDataQueue(p_dataQueue), m_stateMachine(p_sm), m_taskHandle(nullptr) {}

SensorTask::~SensorTask() {
    if (m_taskHandle != nullptr) {
        vTaskDelete(m_taskHandle);
    }
}

esp_err_t SensorTask::init(const IRuntimeConfig&) {
    BaseType_t result = xTaskCreate(
        taskFunction,
        TAG,
        STACK_SIZE,
        this,
        PRIORITY,
        &m_taskHandle
    );

    if (result != pdPASS) {
        ESP_LOGE(TAG, "Failed to create SensorTask");
    }
    return ESP_OK;
}

void SensorTask::taskFunction(void* p_pvParameters) {
    static_cast<SensorTask*>(p_pvParameters)->run();
}

void SensorTask::run() {
    TickType_t lastWakeTime = xTaskGetTickCount();
    SensorData l_sensorData {0.0f, 0.0f, 0.0f, esp_timer_get_time() }; 

    while (true) {
        // Get processed sensor data directly from MPU6050Manager
        l_sensorData.pitch = m_mpu6050.calculatePitch(l_sensorData.pitch);
        l_sensorData.roll = m_mpu6050.calculateRoll(l_sensorData.roll);
        l_sensorData.yaw = m_mpu6050.calculateYaw(l_sensorData.yaw);
        l_sensorData.timestamp = esp_timer_get_time();
        
        // Send data to queue
        if (xQueueSend(m_sensorDataQueue, &l_sensorData, 0) != pdTRUE) {
            ESP_LOGW(TAG, "Failed to send sensor data to queue");
        }
        
        // Update state machine if necessary
        if (std::abs(l_sensorData.pitch) > 45.0f) {  // Example condition
            m_stateMachine.setState(StateMachine::State::FALLING);
        }

        ESP_LOGV(TAG, "Sensor data: Pitch: %.2f, Roll: %.2f, Yaw: %.2f", 
                 l_sensorData.pitch, l_sensorData.roll, l_sensorData.yaw);

        vTaskDelayUntil(&lastWakeTime, SAMPLING_PERIOD);
    }
}