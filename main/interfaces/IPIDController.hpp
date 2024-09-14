#pragma once

#include "interfaces/IComponent.hpp"

class IPIDController : public IComponent{
  public:
    virtual float compute(float&, float&, float, float) const = 0;
    virtual esp_err_t setConfig(const PIDConfig&) = 0;
    virtual ~IPIDController() = default;
};