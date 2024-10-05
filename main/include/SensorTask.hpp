#pragma once

#include "interfaces/ITask.hpp"

class IMPU6050Manager;
class IStateMachine;


class SensorTask : public ISensorTask {
    public:
        SensorTask(IMPU6050Manager&, QueueHandle_t, IStateMachine&);
        ~SensorTask();
        
        esp_err_t init(const IRuntimeConfig&) override;
        QueueHandle_t getSensorDataQueue() const override { return m_sensorDataQueue; };

    private:
        static constexpr const char* TAG = "SensorTask";
        static constexpr int STACK_SIZE = 4096;
        static constexpr UBaseType_t PRIORITY = 5;  // High priority
        static constexpr TickType_t SAMPLING_PERIOD = pdMS_TO_TICKS(10);  // 100Hz

        IMPU6050Manager& m_mpu6050;
        QueueHandle_t m_sensorDataQueue;
        IStateMachine& m_stateMachine;
        TaskHandle_t m_taskHandle;

        static void taskFunction(void*);
        void run();
};