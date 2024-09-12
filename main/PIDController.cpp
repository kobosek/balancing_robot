#include "include/PIDController.hpp"
#include "include/RuntimeConfig.hpp"
#include <algorithm>

esp_err_t PIDController::setParams(const IRuntimeConfig& config) {
    Logger::info(TAG, "Setting PID parameters");
    setKp(config.getPidKp());
    setKi(config.getPidKi());
    setKd(config.getPidKd());
    setSetpoint(config.getPidTargetAngle());
    setOutputLimits(config.getPidOutputMin(), config.getPidOutputMax());
    setItermLimits(config.getPidItermMin(), config.getPidItermMax());   
    Logger::debug(TAG, "PID parameters set - Kp: %.2f, Ki: %.2f, Kd: %.2f, Setpoint: %.2f", 
                  m_kp, m_ki, m_kd, m_setpoint);
    return ESP_OK;
}

esp_err_t PIDController::init(const IRuntimeConfig& config) {
    Logger::info(TAG, "Initializing PID Controller");
    esp_err_t ret = setParams(config);
    if (ret != ESP_OK) {
        Logger::error(TAG, "Failed to set initial PID parameters");
        return ret;
    }
    Logger::info(TAG, "PID Controller initialized successfully");
    return ESP_OK;
}

float PIDController::compute(float& integral, float& lastError, float currentValue, float dt) const {
    float currentError = m_setpoint - currentValue;
    
    // Proportional term
    float pTerm = m_kp * currentError;
    
    // Integral term 
    integral += currentError * dt;
    integral = std::max(m_iTermMin, std::min(integral, m_iTermMax));
    float iTerm = m_ki * integral;
    
    // Derivative term
    float dTerm = m_kd * (currentError - lastError) / dt;
    lastError = currentError;
    
    // Calculate total output
    float output = pTerm + iTerm + dTerm;
    
    // Limit output value
    output = std::max(m_outputMin, std::min(output, m_outputMax));
    
    Logger::verbose(TAG, "PID Computation - Error: %.2f, P: %.2f, I: %.2f, D: %.2f, Output: %.2f", 
                    currentError, pTerm, iTerm, dTerm, output);
    
    return output;
}

esp_err_t PIDController::onConfigUpdate(const IRuntimeConfig& config) {
    Logger::info(TAG, "Updating PID Controller configuration");
    return setParams(config);
}

void PIDController::setSetpoint(float setpoint) {
    m_setpoint = setpoint;
    Logger::debug(TAG, "PID setpoint updated to %.2f", setpoint);
}

void PIDController::setKp(float kp) {
    m_kp = kp;
    Logger::debug(TAG, "PID Kp updated to %.2f", kp);
}

void PIDController::setKi(float ki) {
    m_ki = ki;
    Logger::debug(TAG, "PID Ki updated to %.2f", ki);
}

void PIDController::setKd(float kd) {
    m_kd = kd;
    Logger::debug(TAG, "PID Kd updated to %.2f", kd);
}

void PIDController::setOutputLimits(float min, float max) {
    m_outputMin = min;
    m_outputMax = max;
    Logger::debug(TAG, "PID output limits updated to [%.2f, %.2f]", min, max);
}

void PIDController::setItermLimits(float min, float max) {
    m_iTermMin = min;
    m_iTermMax = max;
    Logger::debug(TAG, "PID integral term limits updated to [%.2f, %.2f]", min, max);
}