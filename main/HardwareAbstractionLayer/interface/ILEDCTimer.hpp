#pragma once
#include "include/ConfigTypes.hpp"

class ILEDCTimer {
    public:
        virtual ~ILEDCTimer() = default;
        virtual esp_err_t init(const LEDCTimerConfig&) = 0;
};