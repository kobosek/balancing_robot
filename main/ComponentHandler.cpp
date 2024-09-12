#include "include/ComponentHandler.hpp"
#include "include/WebServer.hpp"
#include "include/WifiManager.hpp"
#include "include/PIDController.hpp"
#include "include/MotorDriver.hpp"
#include "include/MPU6050Manager.hpp"

#include <algorithm>
#include "esp_log.h"

esp_err_t ComponentHandler::init() {
    Logger::info(TAG, "Initializing ComponentHandler");

    wifiManager = std::make_shared<WiFiManager>();
    esp_err_t ret = wifiManager->init(runtimeConfig);
    if (ret != ESP_OK) {
        Logger::error(TAG, "Failed to initialize WiFiManager");
        return ret;
    }

    webServer = std::make_shared<WebServer>(*this, runtimeConfig);
    ret = webServer->init(runtimeConfig);
    if (ret != ESP_OK) {
        Logger::error(TAG, "Failed to initialize WebServer");
        return ret;
    }

    motorDriver = std::make_shared<MX1616H>();
    ret = motorDriver->init(runtimeConfig);
    if (ret != ESP_OK) {
        Logger::error(TAG, "Failed to initialize MotorDriver");
        return ret;
    }

    mpu6050Manager = std::make_shared<MPU6050Manager>();
    ret = mpu6050Manager->init(runtimeConfig);
    if (ret != ESP_OK) {
        Logger::error(TAG, "Failed to initialize MPU6050Manager");
        return ret;
    }

    pidController = std::make_shared<PIDController>();
    ret = pidController->init(runtimeConfig);
    if (ret != ESP_OK) {
        Logger::error(TAG, "Failed to initialize PIDController");
        return ret;
    }

    registerObserver(wifiManager);
    registerObserver(webServer);
    registerObserver(motorDriver);
    registerObserver(pidController);
    registerObserver(mpu6050Manager);

    Logger::info(TAG, "ComponentHandler initialization complete");
    return ESP_OK;
}

void ComponentHandler::registerObserver(std::shared_ptr<IConfigObserver> observer) {
    observers.push_back(observer);
    Logger::debug(TAG, "Registered observer");
}

void ComponentHandler::unregisterObserver(std::shared_ptr<IConfigObserver> observer) {
    observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
    Logger::debug(TAG, "Unregistered observer");
}

void ComponentHandler::notifyConfigUpdate() {
    Logger::info(TAG, "Notifying all observers of configuration update");
    for (auto observer : observers) {
        esp_err_t ret = observer->onConfigUpdate(runtimeConfig);
        if (ret != ESP_OK) {
            Logger::warn(TAG, "Observer failed to update configuration");
        }
    }
}