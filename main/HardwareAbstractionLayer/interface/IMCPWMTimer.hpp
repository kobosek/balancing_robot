#pragma once
#include <driver/mcpwm.h>

class IMCPWMTimer {
    public:
        virtual ~IMCPWMTimer() = default;
        virtual esp_err_t init() = 0;
        virtual mcpwm_unit_t getUnit() const = 0;
        virtual mcpwm_timer_t  getTimer() const = 0;
};