#pragma once
#include "HardwareConfigTypes.hpp"

class IConfigValidator {
public:
    virtual ~IConfigValidator() = default;
    virtual esp_err_t validateConfig(const HardwareConfig&) = 0;
};