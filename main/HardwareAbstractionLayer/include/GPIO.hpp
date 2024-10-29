#pragma once 
#include "interface/IGPIO.hpp"
#include "include/ConfigTypes.hpp"

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
