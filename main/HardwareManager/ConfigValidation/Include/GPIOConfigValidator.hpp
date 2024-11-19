#pragma once
#include "IConfigValidator.hpp"

class GPIOConfigValidator : public IConfigValidator {
public:
    esp_err_t validateConfig(const HardwareConfig&) override;

private:
    static constexpr const char* TAG = "GPIOConfigValidator";

    esp_err_t validateSingleGPIOConfig(const GPIOConfig& p_config);
    esp_err_t validatePinNumber(const GPIOConfig& p_config);
    esp_err_t validateGPIOMode(const GPIOConfig& p_config);
    esp_err_t validateInterruptType(const GPIOConfig& p_config);
    
    esp_err_t validateUniquePinNumbers(const GPIOSConfig& p_config);
};
