#pragma once
#include "IConfigValidator.hpp"

class LEDCConfigValidator : public IConfigValidator {
public:
    esp_err_t validateConfig(const HardwareConfig&) override;

private:
    static constexpr const char* TAG = "LEDCConfigValidator";

    esp_err_t validateNumberOfTimers(const LEDCConfig& p_config, ledc_mode_t p_speedMode);
    esp_err_t validateTimerIds(const LEDCConfig& p_config);
    esp_err_t validateUniqueTimerIds(const LEDCConfig& p_config);
    esp_err_t validateNumberOfChannels(const LEDCConfig& p_config, ledc_mode_t p_speedMode);
    esp_err_t validateChannelIds(const LEDCConfig& p_config);
    esp_err_t validateUniqueChannelIds(const LEDCConfig& p_config);
    esp_err_t validatePinNumbers(const LEDCConfig& p_config);
    esp_err_t validateUniquePinNumbers(const LEDCConfig& p_config);
};
