#pragma once

#include "HardwareConfigTypes.hpp"
#include "esp_err.h"
#include "driver/ledc.h"

class LEDCConfigValidator {
public:
    static esp_err_t validateConfig(const LEDCConfig& p_config);

private:
    static constexpr const char* TAG = "LEDCConfigValidator";

    static esp_err_t validateNumberOfTimers(const LEDCConfig& p_config, ledc_mode_t p_speedMode);
    static esp_err_t validateTimerIds(const LEDCConfig& p_config);
    static esp_err_t validateUniqueTimerIds(const LEDCConfig& p_config);
    static esp_err_t validateNumberOfChannels(const LEDCConfig& p_config, ledc_mode_t p_speedMode);
    static esp_err_t validateChannelIds(const LEDCConfig& p_config);
    static esp_err_t validateUniqueChannelIds(const LEDCConfig& p_config);
};
