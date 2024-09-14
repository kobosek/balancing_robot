#pragma once
#include "interfaces/IComponent.hpp"

class IWiFiManager;
class IWebServer;
class IMotorDriver;
class IPIDController;
class IMPU6050Manager;
class IStateMachine;
class ISensorTask;
class IPIDTask;
class IMotorControlTask;
class ITelemetryTask;
class IConfigurationTask;

class IComponentHandler : public IComponent {
public:
    virtual ~IComponentHandler() = default;

    virtual IWiFiManager& getWifiManager() = 0;
    virtual IWebServer& getWebServer() = 0;
    virtual IMotorDriver& getMotorDriver() = 0;
    virtual IPIDController& getPIDController() = 0;
    virtual IMPU6050Manager& getMPU6050Manager() = 0;

    virtual IStateMachine& getStateMachine() = 0;
    virtual ISensorTask& getSensorTask() = 0;
    virtual IPIDTask& getPIDTask() = 0;
    virtual IMotorControlTask& getMotorControlTask() = 0;
    virtual ITelemetryTask& getTelemetryTask() = 0;
    virtual IConfigurationTask& getConfigurationTask() = 0;
};