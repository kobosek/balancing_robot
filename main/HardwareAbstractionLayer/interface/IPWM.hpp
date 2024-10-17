#pragma once
#include "include/ConfigTypes.hpp"

typedef int esp_err_t;

class IPWM { 
    public:
        virtual ~IPWM() = default;
        virtual esp_err_t init(const LEDCChannelConfig&, uint32_t) = 0;
        virtual esp_err_t setDuty(float) const = 0;
};