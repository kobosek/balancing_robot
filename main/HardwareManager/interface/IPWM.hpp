#pragma once
#include "include/ConfigTypes.hpp"
#include "interface/IHalComponent.hpp"

class IPWM : public IHalComponent { 
    public:
        virtual ~IPWM() = default;
        virtual int getPinNum() const = 0;
        virtual esp_err_t setDuty(float) const = 0;
};