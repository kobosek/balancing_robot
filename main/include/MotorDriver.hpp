#pragma once

#include "interfaces/IMotorDriver.hpp"
#include "include/Logger.hpp"
#include "driver/gpio.h"

class L298N : public IMotorDriver {
    public:
        esp_err_t init(const IRuntimeConfig&) override;
        esp_err_t onConfigUpdate(const IRuntimeConfig&) override;
        esp_err_t setSpeed(float speed) const override;
    private:
        static constexpr const char* TAG = "L298N";

        static constexpr gpio_num_t IN1_PIN = GPIO_NUM_27;
        static constexpr gpio_num_t IN2_PIN = GPIO_NUM_26;
        static constexpr gpio_num_t IN3_PIN = GPIO_NUM_25;
        static constexpr gpio_num_t IN4_PIN = GPIO_NUM_33;
        static constexpr gpio_num_t PWM_L_PIN = GPIO_NUM_32;
        static constexpr gpio_num_t PWM_R_PIN = GPIO_NUM_14;
};

class MX1616H : public IMotorDriver {
    public:
        esp_err_t init(const IRuntimeConfig&) override;
        esp_err_t onConfigUpdate(const IRuntimeConfig&) override;
        esp_err_t setSpeed(float speed) const override; 
    private:
        static constexpr const char* TAG = "MX1616H";
        
        static constexpr gpio_num_t IN1_PIN = GPIO_NUM_14;
        static constexpr gpio_num_t IN2_PIN = GPIO_NUM_27;
        static constexpr gpio_num_t IN3_PIN = GPIO_NUM_26;
        static constexpr gpio_num_t IN4_PIN = GPIO_NUM_25;
    };