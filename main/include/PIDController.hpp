#pragma once

#include "interfaces/IPIDController.hpp"

class PIDController : public IPIDController {
public:
    PIDController() : m_kp(0), m_ki(0), m_kd(0), m_setpoint(0),
      m_outputMin(0), m_outputMax(0), m_iTermMin(0), m_iTermMax(0) {}

    esp_err_t init(const IRuntimeConfig&) override;
    esp_err_t onConfigUpdate(const IRuntimeConfig&) override;

    float compute(float&, float&, float, float) const override;
private:
    esp_err_t setParams(const IRuntimeConfig&);

    void setSetpoint(float);

    void setKp(float);
    void setKi(float);
    void setKd(float);
    void setOutputLimits(float, float);
    void setItermLimits(float, float);

    float m_kp, m_ki, m_kd;
    float m_setpoint;
    float m_outputMin, m_outputMax;
    float m_iTermMin, m_iTermMax;
};
