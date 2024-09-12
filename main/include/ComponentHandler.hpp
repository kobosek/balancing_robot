#pragma once

#include "esp_err.h"
#include "esp_log.h"
#include <vector>
#include <memory>

class IConfigObserver;
class IMotorDriver;
class IWebServer;
class IPIDController;
class IMPU6050Manager;
class IWiFiManager;
class IRuntimeConfig;

class ComponentHandler {
public:
    ComponentHandler(IRuntimeConfig& config) : runtimeConfig(config) {}
    esp_err_t init();
    void notifyConfigUpdate();

    const IWiFiManager& getWifiManager() { return *wifiManager; }
    IWebServer& getWebServer() { return *webServer; }
    const IMotorDriver& getMotorDriver() {return *motorDriver; }
    const IPIDController& getPIDController() { return *pidController; }
    const IMPU6050Manager& getMPU6050Manager() { return *mpu6050Manager; }

private:
    static constexpr const char* TAG = "Component Handler";

    void registerObserver(std::shared_ptr<IConfigObserver>);
    void unregisterObserver(std::shared_ptr<IConfigObserver>);

    IRuntimeConfig& runtimeConfig;

    std::shared_ptr<IWiFiManager> wifiManager;
    std::shared_ptr<IWebServer> webServer;
    std::shared_ptr<IMotorDriver> motorDriver;
    std::shared_ptr<IPIDController> pidController;
    std::shared_ptr<IMPU6050Manager> mpu6050Manager;

    std::vector<std::shared_ptr<IConfigObserver>> observers;
};
