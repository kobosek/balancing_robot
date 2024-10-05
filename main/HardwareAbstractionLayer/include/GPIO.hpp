#pragma once 
#include "interface/IGPIO.hpp"

struct GPIOConfig {
    gpio_num_t pinNum;
    gpio_mode_t gpioMode = GPIO_MODE_OUTPUT;
    gpio_pullup_t internalPullUp = GPIO_PULLUP_DISABLE;
    gpio_pulldown_t internalPullDown = GPIO_PULLDOWN_DISABLE;
    gpio_int_type_t interruptType = GPIO_INTR_DISABLE;
};

class GPIO : public IGPIO {
    public:
        GPIO(const GPIOConfig&);
        esp_err_t init() override;
        esp_err_t setHigh() override;
        esp_err_t setLow() override;
        int getLevel() const override;
        gpio_num_t getPinNum() const override;
    private:
        static constexpr const char* TAG = "GPIO";

        GPIOConfig m_config;
};
