#pragma once
#include "interfaces/IConfigObserver.hpp"

class IMotorDriver : public IConfigObserver {
    public:
        virtual esp_err_t setSpeed(float speed) const = 0;
        virtual ~IMotorDriver() = default;
};