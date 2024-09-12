#include "include/PIDController.hpp"
#include "include/RuntimeConfig.hpp"

#include "esp_log.h"
#include <algorithm>

static const char* TAG = "PIDController";  // Add this line for logging

esp_err_t PIDController::setParams(const IRuntimeConfig& p_config) {
    setKp(p_config.getPidKp());
    setKi(p_config.getPidKi());
    setKd(p_config.getPidKd());
    setSetpoint(p_config.getPidTargetAngle());
    setOutputLimits(p_config.getPidOutputMin(), p_config.getPidOutputMax());
    setItermLimits(p_config.getPidItermMin(), p_config.getPidItermMax());   
    return ESP_OK;
}


esp_err_t PIDController::init(const IRuntimeConfig& p_config) {
    return setParams(p_config);
}

float PIDController::compute(float& p_integral, float& p_lastError, float p_currentValue, float p_dt) const {
    float l_currentError = m_setpoint - p_currentValue;
    
    // Proportional term
    float pTerm = m_kp * l_currentError;
    
    // Integral term 
    p_integral += l_currentError * p_dt;
    p_integral = std::max(m_iTermMin, std::min(p_integral, m_iTermMax));
    float iTerm = m_ki * p_integral;
    
    // Derivative term
    float dTerm = m_kd * (l_currentError - p_lastError) / p_dt;
    p_lastError = l_currentError;
    
    // Calculate total output
    float output = pTerm + iTerm + dTerm;
    
    // Limit output value
    output = std::max(m_outputMin, std::min(output, m_outputMax));

    //ESP_LOGI("PID", "Target: %.2f, Input: %.2f, Error: %.2f, Output: %.2f", m_target, input, error, output);
    //ESP_LOGI("PID", "P: %.2f, I: %.2f, D: %.2f", pTerm, iTerm, dTerm);
    
    return output;
}

esp_err_t PIDController::onConfigUpdate(const IRuntimeConfig& config) {
    return setParams(config);
}

void PIDController::setSetpoint(float setpoint) {
    m_setpoint = setpoint;
}

void PIDController::setKp(float kp) {
    m_kp = kp;
}

void PIDController::setKi(float ki) {
    m_ki = ki;
}
void PIDController::setKd(float kd) {
    m_kd = kd;
}

void PIDController::setOutputLimits(float min, float max) {
    m_outputMin = min;
    m_outputMax = max;
}

void PIDController::setItermLimits(float min, float max) {
    m_iTermMin = min;
    m_iTermMax = max;
}