#pragma once

#include "interfaces/IMotorDriver.hpp"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_log.h"

class L298N : public IMotorDriver {
public:
    esp_err_t init(const IRuntimeConfig&) override;
    esp_err_t onConfigUpdate(const IRuntimeConfig&) override;
    esp_err_t setSpeed(float) const override;

private:
    static constexpr const char* TAG = "L298N";
    static constexpr gpio_num_t IN1_PIN = GPIO_NUM_27;
    static constexpr gpio_num_t IN2_PIN = GPIO_NUM_26;
    static constexpr gpio_num_t IN3_PIN = GPIO_NUM_25;
    static constexpr gpio_num_t IN4_PIN = GPIO_NUM_33;
    static constexpr gpio_num_t PWM_L_PIN = GPIO_NUM_32;
    static constexpr gpio_num_t PWM_R_PIN = GPIO_NUM_14;

    static constexpr uint32_t PWM_FREQUENCY = 5000; // 5 kHz
    static constexpr ledc_timer_bit_t PWM_RESOLUTION = LEDC_TIMER_10_BIT; // 10-bit resolution
    static constexpr uint32_t MAX_DUTY = (1 << PWM_RESOLUTION) - 1;

    esp_err_t configureGPIO();
    esp_err_t configurePWM();
};

class MX1616H : public IMotorDriver {
public:
    esp_err_t init(const IRuntimeConfig&) override;
    esp_err_t onConfigUpdate(const IRuntimeConfig&) override;
    esp_err_t setSpeed(float) const override;

private:
    static constexpr const char* TAG = "MX1616H";
    static constexpr gpio_num_t IN1_PIN = GPIO_NUM_14;
    static constexpr gpio_num_t IN2_PIN = GPIO_NUM_27;
    static constexpr gpio_num_t IN3_PIN = GPIO_NUM_26;
    static constexpr gpio_num_t IN4_PIN = GPIO_NUM_25;

    static constexpr uint32_t PWM_FREQUENCY = 5000; // 5 kHz
    static constexpr ledc_timer_bit_t PWM_RESOLUTION = LEDC_TIMER_10_BIT; // 10-bit resolution
    static constexpr uint32_t MAX_DUTY = (1 << PWM_RESOLUTION) - 1;

    esp_err_t configurePWM();
};