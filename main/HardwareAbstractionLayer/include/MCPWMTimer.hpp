#pragma once
#include "interface/IMCPWMTimer.hpp"

struct MCPWMTImerConfig {

};

class MCPWMTimer : public IMCPWMTimer {
    public:
        MCPWMTimer(const MCPWMTImerConfig&);
        esp_err_t init() override;
        mcpwm_unit_t getUnit() const override;
        mcpwm_timer_t getTimer() const override;
};