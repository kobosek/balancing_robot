#include "include/ComponentHandler.hpp"
#include "include/WebServer.hpp"
#include "include/WifiManager.hpp"
#include "include/PIDController.hpp"
#include "include/MotorDriver.hpp"
#include "include/MPU6050Manager.hpp"

#include <algorithm>
#include "esp_log.h"

static const char* TAG = "ComponentHanlder";

void ComponentHandler::init() {
    wifiManager = std::make_shared<WiFiManager>();
    wifiManager->init(runtimeConfig);
    
    webServer = std::make_shared<WebServer>(*this, runtimeConfig);
    webServer->init(runtimeConfig);

    motorDriver = std::make_shared<MX1616H>();
    motorDriver->init(runtimeConfig);

    mpu6050Manager = std::make_shared<MPU6050Manager>();
    mpu6050Manager->init(runtimeConfig);

    //mpu6050_manager->setCalibrationSamples(runtimeConfig.getMpu6050CalibrationSamples());
    pidController = std::make_shared<PIDController>();
    pidController->init(runtimeConfig);

    registerObserver(wifiManager);
    registerObserver(webServer);
    registerObserver(motorDriver);
    registerObserver(pidController);
    registerObserver(mpu6050Manager);
}

void ComponentHandler::registerObserver(std::shared_ptr<IConfigObserver> observer) {
    observers.push_back(observer);
}

void ComponentHandler::unregisterObserver(std::shared_ptr<IConfigObserver> observer) {
    observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
}

void ComponentHandler::notifyConfigUpdate() {
    for (auto observer : observers) {
        observer->onConfigUpdate(runtimeConfig);
    }
}