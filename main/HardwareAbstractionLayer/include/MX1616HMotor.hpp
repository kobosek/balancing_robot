#pragma once 

#include "interface/IMotor.hpp"
#include <memory>

class IPWM;

class MX1616HMotor : public IMotor {
public:
    MX1616HMotor(const std::shared_ptr<IPWM>, const std::shared_ptr<IPWM>);
    esp_err_t init() override;
    esp_err_t setSpeed(float) const override;

private:
    static constexpr const char* TAG = "MX1616H";

    std::shared_ptr<IPWM> m_in1;
    std::shared_ptr<IPWM> m_in2;
};