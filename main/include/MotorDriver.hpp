#pragma once

#include "interfaces/IMotorDriver.hpp"

class L298N : public IMotorDriver {
    public:
        esp_err_t init(const IRuntimeConfig&) override;
        esp_err_t onConfigUpdate(const IRuntimeConfig&) override;
        esp_err_t setSpeed(float speed) const override;   
};

class MX1616H : public IMotorDriver {
    public:
        esp_err_t init(const IRuntimeConfig&) override;
        esp_err_t onConfigUpdate(const IRuntimeConfig&) override;
        esp_err_t setSpeed(float speed) const override; 
};