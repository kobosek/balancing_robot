#pragma once
#include "HardwareConfigTypes.hpp"
#include "esp_err.h"

class IConfigValidator {
public:
    virtual ~IConfigValidator() = default;
    virtual esp_err_t validateConfig(const HardwareConfig& p_config) = 0;
};