#pragma once
#include "include/ConfigTypes.hpp"

#include "interface/IPWM.hpp"
#include "driver/ledc.h"
#include <memory>

class LEDCPWM : public IPWM {
    public:
        LEDCPWM(const LEDCTimerConfig&, const LEDCCHannelConfig&);
        esp_err_t init() override;
        esp_err_t setDuty(float) const override;
    private:
        static constexpr const char* TAG = "LEDCPWM";
        
        uint32_t calculateMaxDuty() const;

        LEDCTimerConfig m_timerConfig;
        LEDCCHannelConfig m_config;
};
