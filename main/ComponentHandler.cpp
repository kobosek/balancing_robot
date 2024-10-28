#include "include/ComponentHandler.hpp"
#include "include/WebServer.hpp"
#include "include/WifiManager.hpp"
#include "include/PIDController.hpp"
#include "include/MotorDriver.hpp"
#include "include/MPU6050Manager.hpp"

#include <algorithm>
#include "esp_log.h"

esp_err_t ComponentHandler::init() {
    ESP_LOGI(TAG, "Initializing ComponentHandler");

    wifiManager = std::make_shared<WiFiManager>();
    esp_err_t ret = wifiManager->init(runtimeConfig);
    webServer = std::make_shared<WebServer>(*this, runtimeConfig);
    if (ret == ESP_OK) {
        webServer->init(runtimeConfig);
        registerObserver(wifiManager);
        registerObserver(webServer);
    }

    motorDriver = std::make_shared<MX1616H>();
    ret = motorDriver->init(runtimeConfig);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize MotorDriver");
        return ret;
    }

    mpu6050Manager = std::make_shared<MPU6050Manager>();
    ret = mpu6050Manager->init(runtimeConfig);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize MPU6050Manager");
        return ret;
    }

    pidController = std::make_shared<PIDController>();
    ret = pidController->init(runtimeConfig);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize PIDController");
        return ret;
    }

    registerObserver(motorDriver);
    registerObserver(pidController);
    registerObserver(mpu6050Manager);

    ESP_LOGI(TAG, "ComponentHandler initialization complete");
    return ESP_OK;
}

void ComponentHandler::registerObserver(std::shared_ptr<IConfigObserver> observer) {
    observers.push_back(observer);
    ESP_LOGD(TAG, "Registered observer");
}

void ComponentHandler::unregisterObserver(std::shared_ptr<IConfigObserver> observer) {
    observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
    ESP_LOGD(TAG, "Unregistered observer");
}

void ComponentHandler::notifyConfigUpdate() {
    ESP_LOGI(TAG, "Notifying all observers of configuration update");
    for (auto observer : observers) {
        esp_err_t ret = observer->onConfigUpdate(runtimeConfig);
        if (ret != ESP_OK) {
            ESP_LOGW(TAG, "Observer failed to update configuration");
        }
    }
}