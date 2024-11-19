#pragma once
#include "interface/IPWM.hpp"

class LEDCPWM : public IPWM {
    public:
        LEDCPWM(const LEDCChannelConfig&);
        ~LEDCPWM() = default;
        LEDCPWM(const LEDCPWM&) = delete;
        LEDCPWM& operator=(const LEDCPWM&) = delete;
        LEDCPWM(LEDCPWM&&) = delete;
        LEDCPWM& operator=(LEDCPWM&&) = delete;

        //IHalComponent
        esp_err_t init() override;

        //IPWM
        int getPinNum() const override;
        esp_err_t setDuty(float) const override;
    private:
        static constexpr const char* TAG = "LEDCPWM";

        esp_err_t notInitialized() const override;

        const LEDCChannelConfig m_config;
};
