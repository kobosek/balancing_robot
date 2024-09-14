#include "include/ComponentHandler.hpp"
#include "include/WebServer.hpp"
#include "include/WifiManager.hpp"
#include "include/PIDController.hpp"
#include "include/MotorDriver.hpp"
#include "include/MPU6050Manager.hpp"
#include "include/StateMachine.hpp"
#include "include/SensorTask.hpp"
#include "include/PIDTask.hpp"
#include "include/MotorControlTask.hpp"
#include "include/TelemetryTask.hpp"
#include "include/ConfigurationTask.hpp"

#include <algorithm>

ComponentHandler::ComponentHandler() {
    QueueHandle_t m_sensorDataQueue = xQueueCreate(10, sizeof(SensorData));
    QueueHandle_t m_pidOutputQueue = xQueueCreate(10, sizeof(float));
    QueueHandle_t m_motorControlQueue = xQueueCreate(10, sizeof(float));
    QueueHandle_t m_telemetryQueue = xQueueCreate(10, sizeof(TelemetryData));
    QueueHandle_t m_configQueue = xQueueCreate(1, sizeof(PIDConfig));
}

esp_err_t ComponentHandler::init(const IRuntimeConfig& p_runtimeConfig)  {
    ESP_LOGI(TAG, "Initializing ComponentHandler");

    m_wifiManager = std::make_unique<WiFiManager>();
    esp_err_t l_ret = m_wifiManager->init(p_runtimeConfig);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize WiFiManager");
        return l_ret;
    }

    m_webServer = std::make_unique<WebServer>();
    l_ret = m_webServer->init(p_runtimeConfig);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize WebServer");
        return l_ret;
    }

    m_configurationTask = std::make_unique<ConfigurationTask>(p_runtimeConfig, m_webServer, m_configQueue);
    l_ret = m_configurationTask->init(p_runtimeConfig);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize WebServer");
        return l_ret;
    }

    m_telemetryTask = std::make_unique<TelemetryTask>(*m_webServer, m_sensorDataQueue, m_pidOutputQueue, m_telemetryQueue);
    l_ret = m_telemetryTask->init(p_runtimeConfig);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize WebServer");
        return l_ret;
    }

    m_stateMachine = std::make_unique<StateMachine>(m_sensorDataQueue, m_pidOutputQueue, m_motorControlQueue, m_telemetryQueue, m_configQueue);
    esp_err_t l_ret = m_stateMachine->init(p_runtimeConfig);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize StateMachine");
        return l_ret;
   
    }

    m_mpu6050Manager = std::make_unique<MPU6050Manager>();
    l_ret = m_mpu6050Manager->init(p_runtimeConfig);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize MPU6050Manager");
        return l_ret;
    }

    m_sensorTask = std::make_unique<SensorTask>(*m_mpu6050Manager, m_sensorDataQueue, *m_stateMachine);
    esp_err_t l_ret = m_sensorTask->init(p_runtimeConfig);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize SensorTask");
        return l_ret;
    }

    m_pidController = std::make_unique<PIDController>();
    l_ret = m_pidController->init(p_runtimeConfig);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize PIDController");
        return l_ret;
    }

    m_pidTask = std::make_unique<PIDTask>(*m_pidController, m_sensorDataQueue, m_pidOutputQueue, m_configQueue, m_stateMachine);
    l_ret = m_pidTask->init(p_runtimeConfig);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize PIDTask");
        return l_ret;
    }

    m_motorDriver = std::make_unique<MX1616H>();
    l_ret = m_motorDriver->init(p_runtimeConfig);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize MotorDriver");
        return l_ret;
    }

    m_motorControlTask = std::make_unique<MotorControlTask>(*m_motorDriver, m_pidOutputQueue, *m_stateMachine);
    l_ret = m_motorDriver->init(p_runtimeConfig);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize MotorDriver");
        return l_ret;
    }

    ESP_LOGI(TAG, "ComponentHandler initialization complete");
    return ESP_OK;
}