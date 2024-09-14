#pragma once

#include "interfaces/IComponentHandler.hpp"
#include <vector>
#include <memory>

class ComponentHandler : public IComponentHandler {
public:
    ComponentHandler();

    esp_err_t init(const IRuntimeConfig&) override;

    IWiFiManager& getWifiManager() override { return *m_wifiManager; }
    IWebServer& getWebServer() override { return *m_webServer; }
    IMotorDriver& getMotorDriver() override {return *m_motorDriver; }
    IPIDController& getPIDController() override { return *m_pidController; }
    IMPU6050Manager& getMPU6050Manager() override { return *m_mpu6050Manager; }

    IStateMachine& getStateMachine() { return *m_stateMachine; }
    ISensorTask& getSensorTask() { return *m_sensorTask; }
    IPIDTask& getPIDTask() { return *m_pidTask; }
    IMotorControlTask& getMotorControlTask() { return *m_motorControlTask; }
    ITelemetryTask& getTelemetryTask() { return *m_telemetryTask; }
    IConfigurationTask& getConfigurationTask() { return *m_configurationTask; }
    
private:
    static constexpr const char* TAG = "Component Handler";

    std::unique_ptr<IWiFiManager> m_wifiManager;
    std::unique_ptr<IWebServer> m_webServer;
    std::unique_ptr<IMotorDriver> m_motorDriver;
    std::unique_ptr<IPIDController> m_pidController;
    std::unique_ptr<IMPU6050Manager> m_mpu6050Manager;

    std::unique_ptr<IStateMachine> m_stateMachine;
    std::unique_ptr<ISensorTask> m_sensorTask;
    std::unique_ptr<IPIDTask> m_pidTask;
    std::unique_ptr<IMotorControlTask> m_motorControlTask;
    std::unique_ptr<ITelemetryTask> m_telemetryTask;
    std::unique_ptr<IConfigurationTask> m_configurationTask;

    QueueHandle_t m_sensorDataQueue;
    QueueHandle_t m_pidOutputQueue;
    QueueHandle_t m_motorControlQueue;
    QueueHandle_t m_telemetryQueue;
    QueueHandle_t m_configQueue;
};
