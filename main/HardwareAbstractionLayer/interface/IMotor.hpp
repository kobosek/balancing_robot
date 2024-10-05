#pragma once

typedef int esp_err_t;

class IMotor {
    public:
        virtual ~IMotor() = default;
        virtual esp_err_t init() = 0;
        virtual esp_err_t setSpeed(float speed) const = 0;
};