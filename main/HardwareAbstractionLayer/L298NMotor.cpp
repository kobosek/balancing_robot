#include "include/L298NMotor.hpp"
#include "interface/IGPIO.hpp"
#include "interface/IPWM.hpp"
#include "esp_err.h"
#include "esp_log.h"

L298NMotor::L298NMotor(const std::shared_ptr<IGPIO> p_in1,
                       const std::shared_ptr<IGPIO> p_in2,
                       const std::shared_ptr<IPWM> p_pwm) : m_in1(p_in1), m_in2(p_in2), m_pwm(p_pwm) {}


esp_err_t L298NMotor::init() {
    ESP_LOGI(TAG, "Initializing L298N motor driver");

    esp_err_t l_ret = m_in1->init();
    if (l_ret != ESP_OK) return l_ret;

    l_ret = m_in2->init();
    if (l_ret != ESP_OK) return l_ret;

    l_ret = m_pwm->init();
    if (l_ret != ESP_OK) return l_ret;

    ESP_LOGI(TAG, "L298N motor driver initialized successfully");
    return ESP_OK;
}

esp_err_t L298NMotor::setClockwise() const {
    esp_err_t l_ret = m_in2->setLow();
    if (l_ret != ESP_OK) return l_ret;
    
    l_ret = m_in1->setHigh();
    if (l_ret != ESP_OK) return l_ret;

    return ESP_OK;
}

esp_err_t L298NMotor::setCounterClockwise() const {
    esp_err_t l_ret = m_in1->setLow();
    if (l_ret != ESP_OK) return l_ret;
    
    l_ret = m_in2->setHigh();
    if (l_ret != ESP_OK) return l_ret;

    return ESP_OK;
}

esp_err_t L298NMotor::setDirection(float p_speed) const {
    if (p_speed < 0) {
        return setClockwise();
    } else {
        return setCounterClockwise();
    }
}

esp_err_t L298NMotor::setSpeed(float p_speed) const {
    ESP_LOGD(TAG, "Setting L298N speed to %.2f", p_speed);

    float l_clampedSpeed = std::max(-1.0f, std::min(1.0f, p_speed));

    esp_err_t l_ret = setDirection(l_clampedSpeed);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set direction");
        return l_ret;
    }

    float l_duty = std::abs(l_clampedSpeed);

    l_ret = m_pwm->setDuty(l_duty);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set duty");
        return l_ret;
    }
    return ESP_OK;
}