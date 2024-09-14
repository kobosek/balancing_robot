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

    virtual const IWiFiManager& getWifiManager() = 0;
    virtual IWebServer& getWebServer() = 0;
    virtual const IMotorDriver& getMotorDriver() = 0;
    virtual const IPIDController& getPIDController() = 0;
    virtual const IMPU6050Manager& getMPU6050Manager() = 0;

    virtual const IStateMachine& getStateMachine() = 0;
    virtual const ISensorTask& getSensorTask() = 0;
    virtual const IPIDTask& getPIDTask() = 0;
    virtual const IMotorControlTask& getMotorControlTask() = 0;
    virtual const ITelemetryTask& getTelemetryTask() = 0;
    virtual const IConfigurationTask& getConfigurationTask() = 0;
};