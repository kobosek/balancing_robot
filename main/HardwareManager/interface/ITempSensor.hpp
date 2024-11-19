#pragma once

typedef int esp_err_t;

class ITempSensor {
    public:
        virtual ~ITempSensor() = default;
        virtual esp_err_t getTemperature(float&) const = 0;    
};