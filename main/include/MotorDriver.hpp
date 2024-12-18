#pragma once

#include "interfaces/IMotorDriver.hpp"
#include "esp_log.h"
#include "driver/gpio.h"

class L298N : public IMotorDriver {
    public:
        esp_err_t init(const IRuntimeConfig&) override;
        esp_err_t onConfigUpdate(const IRuntimeConfig&) override;
        esp_err_t setSpeed(float speed) const override;
    private:
        static constexpr const char* TAG = "L298N";

        static constexpr gpio_num_t IN1_PIN = GPIO_NUM_1;
        static constexpr gpio_num_t IN2_PIN = GPIO_NUM_2;
        static constexpr gpio_num_t IN3_PIN = GPIO_NUM_3;
        static constexpr gpio_num_t IN4_PIN = GPIO_NUM_4;
        static constexpr gpio_num_t PWM_L_PIN = GPIO_NUM_5;
        static constexpr gpio_num_t PWM_R_PIN = GPIO_NUM_6;
};

class MX1616H : public IMotorDriver {
    public:
        esp_err_t init(const IRuntimeConfig&) override;
        esp_err_t onConfigUpdate(const IRuntimeConfig&) override;
        esp_err_t setSpeed(float speed) const override; 
    private:
        static constexpr const char* TAG = "MX1616H";
        
        static constexpr gpio_num_t IN1_PIN = GPIO_NUM_1;
        static constexpr gpio_num_t IN2_PIN = GPIO_NUM_2;
        static constexpr gpio_num_t IN3_PIN = GPIO_NUM_3;
        static constexpr gpio_num_t IN4_PIN = GPIO_NUM_4;
    };