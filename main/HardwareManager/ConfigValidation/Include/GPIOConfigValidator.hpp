#pragma once

#include "HardwareConfigTypes.hpp"

class GPIOConfigValidator {
public:
    static esp_err_t validateConfig(const GPIOConfig& p_config);

private:
    static constexpr const char* TAG = "GPIOConfigValidator";

    static esp_err_t validatePinNumber(const GPIOConfig& p_config);
    static esp_err_t validateGPIOMode(const GPIOConfig& p_config);
    static esp_err_t validateInterruptType(const GPIOConfig& p_config);
};
