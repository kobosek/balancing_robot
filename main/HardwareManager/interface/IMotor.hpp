#pragma once
#include "IHalComponent.hpp"

class IMotor : public IHalComponent {
    public:
        virtual ~IMotor() = default;
        virtual esp_err_t setSpeed(float speed) const = 0;
};