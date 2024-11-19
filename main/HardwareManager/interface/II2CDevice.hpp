#pragma once
#include "IHalComponent.hpp"
class II2CDevice : public IHalComponent {
    public:
        virtual ~II2CDevice() = default;
        virtual esp_err_t writeRegister(uint8_t, uint8_t) const = 0;
        virtual esp_err_t readRegisters(uint8_t, uint8_t*, size_t) const = 0;
};
