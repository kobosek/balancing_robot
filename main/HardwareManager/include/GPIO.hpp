#pragma once 
#include "interface/IGPIO.hpp"

class GPIO : public IGPIO {
    public:
        GPIO(const GPIOConfig&);
        ~GPIO() = default;
        GPIO(const GPIO&) = delete;
        GPIO& operator=(const GPIO&) = delete;
        GPIO(GPIO&&) = delete;
        GPIO& operator=(GPIO&&) = delete;

        //IHalComponent
        esp_err_t init() override;

        //IGPIO
        esp_err_t setHigh() override;
        esp_err_t setLow() override;
        int getLevel() const override;
        gpio_num_t getPinNum() const override;
    private:
        static constexpr const char* TAG = "GPIO";

        esp_err_t notInitialized() const override;
        
        GPIOConfig m_config;
};
