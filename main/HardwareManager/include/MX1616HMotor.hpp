#pragma once 
#include "interface/IMotor.hpp"
#include <memory>

class IPWM;

class MX1616HMotor : public IMotor {
public:
    MX1616HMotor(const std::shared_ptr<IPWM>, const std::shared_ptr<IPWM>);
    ~MX1616HMotor() = default;
    MX1616HMotor(const MX1616HMotor&) = delete;
    MX1616HMotor& operator=(const MX1616HMotor&) = delete;
    MX1616HMotor(MX1616HMotor&&) = delete;
    MX1616HMotor& operator=(MX1616HMotor&&) = delete;

    //IHalComponent
    esp_err_t init() override;

    //IMotor
    esp_err_t setSpeed(float) const override;

private:
    static constexpr const char* TAG = "MX1616H";

    esp_err_t notInitialized() const override;

    std::shared_ptr<IPWM> m_in1;
    std::shared_ptr<IPWM> m_in2;

    esp_err_t setClockwise(float) const;
    esp_err_t setCounterClockwise(float) const; 
};