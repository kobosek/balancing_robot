#pragma once
#include "interfaces/IComponent.hpp"


class ITask : public IComponent{
public:
    virtual ~ITask() = default;
};

class ISensorTask : public ITask {
public:
    virtual QueueHandle_t getSensorDataQueue() const = 0;
};

class IPIDTask : public ITask {
public:
    virtual QueueHandle_t getPIDOutputQueue() const = 0;
};

class IMotorControlTask : public ITask {};

class ITelemetryTask : public ITask {};

class IConfigurationTask : public ITask {};