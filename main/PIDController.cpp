#include "include/PIDController.hpp"
#include "include/RuntimeConfig.hpp"
#include <algorithm>

PIDController::PIDController() {
    m_mutex = xSemaphoreCreateMutex();
}

PIDController::~PIDController() {
    if (m_mutex) {
        vSemaphoreDelete(m_mutex);
    }
}

esp_err_t PIDController::setConfig(const PIDConfig& p_config) {
    ESP_LOGI(TAG, "Setting PID parameters");
    m_config = p_config;
    ESP_LOGD(TAG, "PID parameters set - Kp: %.2f, Ki: %.2f, Kd: %.2f, Setpoint: %.2f, ITermMin: %.2f, ITermMax: %.2f, OutputMin: %.2f, OutputMax: %.2f", 
                  m_config.kp, m_config.ki, m_config.kd, m_config.targetAngle, m_config.itermMin, m_config.itermMax, m_config.outputMin, m_config.outputMax);
    return ESP_OK;
}

esp_err_t PIDController::init(const IRuntimeConfig& p_config) {
    ESP_LOGI(TAG, "Initializing PID Controller");
    if (xSemaphoreTake(m_mutex, portMAX_DELAY) == pdTRUE) {
        setConfig(p_config.getPidConfig());
        xSemaphoreGive(m_mutex);
        ESP_LOGI(TAG, "PID Controller initialized successfully");
        return ESP_OK;
    }
    ESP_LOGE(TAG, "Failed to acquire mutex in init");
    return ESP_FAIL;
}

float PIDController::compute(float& p_integral, float& p_lastError, float p_currentValue, float p_dt) const {
    if (xSemaphoreTake(m_mutex, portMAX_DELAY) == pdTRUE) {
        float l_currentError = m_config.targetAngle - p_currentValue;
        
        // Proportional term
        float l_pTerm = m_config.kp * l_currentError;
        
        // Integral term 
        p_integral += l_currentError * p_dt;
        p_integral = applyLimits(p_integral, m_config.itermMin, m_config.itermMax);
        float l_iTerm = m_config.ki * p_integral;
        
        // Derivative term
        float dTerm = m_config.kd * (l_currentError - p_lastError) / p_dt;
        p_lastError = l_currentError;
        
        // Calculate total output
        float l_output = l_pTerm + l_iTerm + dTerm;
        
        // Limit output value
        l_output = std::max(m_config.outputMin, std::min(l_output, m_config.outputMax));
        
        ESP_LOGV(TAG, "PID Computation - Error: %.2f, P: %.2f, I: %.2f, D: %.2f, Output: %.2f", 
                        l_currentError, l_pTerm, l_iTerm, dTerm, l_output);
        xSemaphoreGive(m_mutex);
        return l_output;
    }
    ESP_LOGW(TAG, "Failed to acquire mutex in compute");
    return 0.0f;  // Return 0 if mutex couldn't be obtained    
}

float PIDController::applyLimits(float p_value, float p_min, float p_max) const {
    return std::max(p_min, std::min(p_value, p_max));
}