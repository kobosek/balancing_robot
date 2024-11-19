#pragma once
#include "IConfigValidator.hpp"

class WIFIConfigValidator : public IConfigValidator {
public:
    esp_err_t validateConfig(const HardwareConfig&) override;

private:
    static constexpr const char* TAG = "WIFIConfigValidator";

    // WIFI configuration validations
    esp_err_t validateSSID(const WIFIConfig& p_config);
    esp_err_t validatePassword(const WIFIConfig& p_config);
    esp_err_t validateAuthMode(const WIFIConfig& p_config);
};
