#pragma once

typedef int esp_err_t;

class IPWM { 
    public:
        virtual ~IPWM() = default;
        virtual esp_err_t init() = 0;
        virtual esp_err_t setDuty(float) const = 0;
};