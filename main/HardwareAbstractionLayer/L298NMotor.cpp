#include "include/L298NMotor.hpp"
#include "interface/IGPIO.hpp"
#include "interface/IPWM.hpp"
#include "esp_err.h"
#include "esp_log.h"

L298NMotor::L298NMotor(const std::shared_ptr<IGPIO> p_in1,
                       const std::shared_ptr<IGPIO> p_in2,
                       const std::shared_ptr<IPWM> p_pwm) : m_in1(p_in1), m_in2(p_in2), m_pwm(p_pwm) {}

esp_err_t L298NMotor::init() {
    ESP_LOGD(TAG, "Initializing L298N motor driver");
    if (!m_in1) {
        setStateError();
        ESP_LOGE(TAG, "IN1 GPIO not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    if (!m_in2) {
        setStateError();
        ESP_LOGE(TAG, "IN2 GPIO not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    if (!m_pwm) {    
        setStateError();
        ESP_LOGE(TAG, "PWM not initialized");
        return ESP_ERR_INVALID_STATE;
    }
    setStateInitialized();
    ESP_LOGI(TAG, "L298N motor driver initialized successfully on IN1: %d, IN2: %d, PWM: %d",
             m_in1->getPinNum(), m_in2->getPinNum(), m_pwm->getPinNum());
    return ESP_OK;
}

esp_err_t L298NMotor::setClockwise() const {
    ESP_LOGD(TAG, "Setting clockwise direction");

    esp_err_t l_ret = m_in2->setLow();
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set IN2 state to low: %s", esp_err_to_name(l_ret));
        return l_ret;
    }
    
    l_ret = m_in1->setHigh();
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set IN1 state to high: %s", esp_err_to_name(l_ret));
        return l_ret;
    }
    ESP_LOGV(TAG, "Direction set to clockwise");
    return ESP_OK;
}

esp_err_t L298NMotor::setCounterClockwise() const {
    ESP_LOGD(TAG, "Setting counter clockwise direction");

    esp_err_t l_ret = m_in1->setLow();
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set IN1 state to low: %s", esp_err_to_name(l_ret));
        return l_ret;
    }
    
    l_ret = m_in2->setHigh();
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set IN2 state to high: %s", esp_err_to_name(l_ret));
        return l_ret;
    }
    ESP_LOGV(TAG, "Direction set to counter clockwise");
    return ESP_OK;
}

esp_err_t L298NMotor::setDirection(float p_speed) const {
    ESP_LOGD(TAG, "Setting direction for speed %.2f", p_speed);

    if (p_speed >= 0) {
        ESP_LOGD(TAG, "Setting direction to clockwise");
        return setClockwise();
    } else {
        ESP_LOGD(TAG, "Setting direction to counter clockwise");
        return setCounterClockwise();
    }
}

esp_err_t L298NMotor::setSpeed(float p_speed) const {
    ESP_LOGD(TAG, "Setting motor speed to %.2f", p_speed);

    if (!isInitialized()) {
        return notInitialized();
    }

    float l_clampedSpeed = std::max(-1.0f, std::min(1.0f, p_speed));
    if (l_clampedSpeed != p_speed) {
        ESP_LOGW(TAG, "Speed value clamped from %.2f to %.2f", p_speed, l_clampedSpeed);
    }
    
    esp_err_t l_ret = setDirection(l_clampedSpeed);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set direction for speed %.2f, error: %s", l_clampedSpeed, esp_err_to_name(l_ret));
        return l_ret;
    }

    float l_duty = std::abs(l_clampedSpeed);
    
    ESP_LOGD(TAG, "Setting PWM duty cycle to %.2f", l_duty);

    l_ret = m_pwm->setDuty(l_duty);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set PWM duty cycle to %.2f, error: %s", l_duty, esp_err_to_name(l_ret));
        return l_ret;
    }
    return ESP_OK;
}

esp_err_t L298NMotor::notInitialized() const {
    ESP_LOGE(TAG, "L298N motor driver not initialized: %s", esp_err_to_name(ESP_ERR_INVALID_STATE));
    return ESP_ERR_INVALID_STATE;
}