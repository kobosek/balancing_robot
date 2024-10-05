#pragma once

#include "interface/IMotor.hpp"
#include <memory>

class IGPIO;
class IPWM;

typedef int esp_err_t;

class L298NMotor : public IMotor {

public:
    L298NMotor(const std::shared_ptr<IGPIO>, const std::shared_ptr<IGPIO>, const std::shared_ptr<IPWM>);
    esp_err_t init() override;
    esp_err_t setSpeed(float) const override;

private:
    static constexpr const char* TAG = "L298NMotor";
    
    std::shared_ptr<IGPIO> m_in1;
    std::shared_ptr<IGPIO> m_in2;
    std::shared_ptr<IPWM> m_pwm;

    esp_err_t setDirection(float) const;
    esp_err_t setClockwise() const;
    esp_err_t setCounterClockwise() const;
};

