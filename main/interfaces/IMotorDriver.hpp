#pragma once
#include "interfaces/IComponent.hpp"

class IMotorDriver : public IComponent {
    public:
        virtual esp_err_t setSpeed(float speed) const = 0;
        virtual ~IMotorDriver() = default;
};