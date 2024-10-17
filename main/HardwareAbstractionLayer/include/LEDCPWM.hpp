#pragma once
#include "interface/IPWM.hpp"
#include "driver/ledc.h"
#include <functional>

class LEDCPWM : public IPWM {
    public:
        esp_err_t init(const LEDCChannelConfig&, uint32_t) override;
        esp_err_t setDuty(float) const override;
    private:
        static constexpr const char* TAG = "LEDCPWM";
        std::function<esp_err_t(uint32_t)> m_setAndUpdateDuty;
};
