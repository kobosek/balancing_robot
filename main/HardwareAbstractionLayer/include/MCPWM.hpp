#pragma once

#include "interface/IPWM.hpp"

class MCPWM : public IPWM { 
    public:
        esp_err_t setDuty(float) const override;
};

