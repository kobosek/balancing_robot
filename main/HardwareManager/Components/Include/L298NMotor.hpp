#pragma once
#include "interface/IMotor.hpp"
#include <memory>

class IGPIO;
class IPWM;

class L298NMotor : public IMotor {

public:
    L298NMotor(const std::shared_ptr<IGPIO>, const std::shared_ptr<IGPIO>, const std::shared_ptr<IPWM>);
    ~L298NMotor() = default;
    L298NMotor(const L298NMotor&) = delete;
    L298NMotor& operator=(const L298NMotor&) = delete;
    L298NMotor(L298NMotor&&) = delete;
    L298NMotor& operator=(L298NMotor&&) = delete;

    //IHalComponent
    esp_err_t init() override;

    //IMotor
    esp_err_t setSpeed(float) const override;

private:
    static constexpr const char* TAG = "L298NMotor";
    
    esp_err_t notInitialized() const override;

    std::shared_ptr<IGPIO> m_in1;
    std::shared_ptr<IGPIO> m_in2;
    std::shared_ptr<IPWM> m_pwm;

    esp_err_t setDirection(float) const;
    esp_err_t setClockwise() const;
    esp_err_t setCounterClockwise() const;
};

