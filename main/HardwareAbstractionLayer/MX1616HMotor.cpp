#include "include/MX1616HMotor.hpp"
#include "esp_err.h"
#include "esp_log.h"

MX1616HMotor::MX1616HMotor(std::shared_ptr<IPWM> p_in1, std::shared_ptr<IPWM> p_in2) : m_in1(p_in1), m_in2(p_in2) {}

esp_err_t MX1616HMotor::init() {
    ESP_LOGI(TAG, "Initializing MX1616H motor driver");

    esp_err_t ret = m_in1->init();
    if (ret != ESP_OK) return ret;

    ret = m_in2->init();
    if (ret != ESP_OK) return ret;

    ESP_LOGI(TAG, "MX1616H motor driver initialized successfully");
    return ESP_OK;
}

esp_err_t MX1616HMotor::setSpeed(float p_speed) const {
    ESP_LOGD(TAG, "Setting MX1616H speed to %.2f", p_speed);

    float l_clampedSpeed = std::max(-1.0f, std::min(1.0f, p_speed));
    float l_duty = std::abs(l_clampedSpeed);

    esp_err_t l_ret;
    if (l_clampedSpeed > 0) {
        l_ret = m_in2->setDuty(0.0f);
        if (l_ret != ESP_OK) return l_ret;
        l_ret = m_in1->setDuty(l_duty);
        if (l_ret != ESP_OK) return l_ret;
    } else {
        l_ret = m_in1->setDuty(0.0f);
        if (l_ret != ESP_OK) return l_ret;
        l_ret = m_in2->setDuty(l_duty);
        if (l_ret != ESP_OK) return l_ret;   
    }    
    return ESP_OK;
}
