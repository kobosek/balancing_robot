#pragma once
#include "driver/gpio.h"

typedef int esp_err_t;

class IGPIO {
    public:
        virtual ~IGPIO() = default;
        virtual esp_err_t init() = 0;
        virtual esp_err_t setHigh() = 0;
        virtual esp_err_t setLow() = 0;
        virtual int getLevel() const = 0;
        virtual gpio_num_t getPinNum() const = 0;
};