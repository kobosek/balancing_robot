#pragma once 

#include "interface/IMotor.hpp"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_log.h"

class MX1616HMotor : public IMotor {
public:
    esp_err_t init() override;
    esp_err_t setSpeed(float) const override;

private:
    static constexpr const char* TAG = "MX1616H";

    std::unique_ptr<IGPIO> m_in1(GPIO_NUM_25);
    std::unique_ptr<IGPIO> m_in2(GPIO_NUM_33);
    std::unique_ptr<IPWM> m_pwm(GPIO_NUM_14);

    static constexpr uint32_t PWM_FREQUENCY = 5000; // 5 kHz
    static constexpr ledc_timer_bit_t PWM_RESOLUTION = LEDC_TIMER_10_BIT; // 10-bit resolution
    static constexpr uint32_t MAX_DUTY = (1 << PWM_RESOLUTION) - 1;

    esp_err_t configurePWM();
};