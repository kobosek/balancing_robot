#pragma once
#include "interfaces/IComponent.hpp"
#include "interfaces/ITask.hpp"

class IWebServer : public IComponent{
    public:
    virtual void update_telemetry(const TelemetryData&) = 0;
    virtual bool hasConfigurationRequest() = 0;
    virtual PIDConfig getConfigurationRequest() = 0;
    virtual void notifyConfigurationUpdated() = 0;
    virtual ~IWebServer() = default;
};