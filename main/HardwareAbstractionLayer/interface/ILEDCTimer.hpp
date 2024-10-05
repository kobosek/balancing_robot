#pragma once
#include "include/ConfigTypes.hpp"

class ILEDCTimer {
    public:
        virtual ~ILEDCTimer() = default;
        virtual esp_err_t init() = 0;
        virtual const LEDCTimerConfig& getConfig() const = 0;
};