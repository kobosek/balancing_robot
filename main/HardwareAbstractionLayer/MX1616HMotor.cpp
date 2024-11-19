#include "include/MX1616HMotor.hpp"
#include "esp_err.h"
#include "esp_log.h"

MX1616HMotor::MX1616HMotor(std::shared_ptr<IPWM> p_in1, std::shared_ptr<IPWM> p_in2) : m_in1(p_in1), m_in2(p_in2) {}

esp_err_t MX1616HMotor::init() {
    ESP_LOGI(TAG, "Initializing MX1616H motor driver");

    if(!m_in1){
        setStateError();
        ESP_LOGE(TAG, "IN1 PWM is not initialized: %s", esp_err_to_name(ESP_ERR_INVALID_STATE));
        return ESP_ERR_INVALID_STATE;
    }

    if(!m_in2){
        setStateError();
        ESP_LOGE(TAG, "IN2 PWM is not initialized: %s", esp_err_to_name(ESP_ERR_INVALID_STATE));
        return ESP_ERR_INVALID_STATE;
    }
    setStateInitialized();
    ESP_LOGI(TAG, "MX1616H motor driver initialized successfully on IN1: %d, IN2: %d",
            m_in1->getPinNum(), m_in2->getPinNum());
    return ESP_OK;
}

esp_err_t MX1616HMotor::setClockwise(float p_duty) const {
    ESP_LOGD(TAG, "Setting clockwise direction with duty %.2f", p_duty);

    esp_err_t l_ret = m_in2->setDuty(0.0f);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set IN2 duty to 0: %s", esp_err_to_name(l_ret));
        return l_ret;
    }

    l_ret = m_in1->setDuty(p_duty);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set IN1 duty to %.2f: %s", p_duty, esp_err_to_name(l_ret));
        return l_ret;
    }    
    ESP_LOGV(TAG, "Direction set to clockwise");
    return ESP_OK;
}

esp_err_t MX1616HMotor::setCounterClockwise(float p_duty) const {
    ESP_LOGD(TAG, "Setting counter-clockwise direction with duty %.2f", p_duty);

    esp_err_t l_ret = m_in1->setDuty(0.0f);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set IN1 duty to 0: %s", esp_err_to_name(l_ret));
        return l_ret;
    }

    l_ret = m_in2->setDuty(p_duty);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set IN2 duty to %.2f: %s", p_duty, esp_err_to_name(l_ret));
        return l_ret;
    }
    ESP_LOGV(TAG, "Direction set to counter clockwise");
    return ESP_OK;
}

esp_err_t MX1616HMotor::setSpeed(float p_speed) const {
    ESP_LOGD(TAG, "Setting MX1616H speed to %.2f", p_speed);

    if (!isInitialized()) {
        return notInitialized();
    }

    float l_clampedSpeed = std::max(-1.0f, std::min(1.0f, p_speed));
    float l_duty = std::abs(l_clampedSpeed);

    esp_err_t l_ret;
    if (l_clampedSpeed >= 0) {
        ESP_LOGD(TAG, "Setting clockwise direction with duty %.2f", l_duty);
        l_ret = setClockwise(l_duty);
        if (l_ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to set clockwise direction");
            return l_ret;
        }
    } else {
        ESP_LOGD(TAG, "Setting counter-clockwise direction with duty %.2f", l_duty);
        l_ret = setCounterClockwise(l_duty);
        if (l_ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to set counter-clockwise direction");
            return l_ret;
        }
    }
    return ESP_OK;
}

esp_err_t MX1616HMotor::notInitialized() const {
    ESP_LOGE(TAG, "MX1616H motor driver is not initialized: %s", esp_err_to_name(ESP_ERR_INVALID_STATE));
    return ESP_ERR_INVALID_STATE;
}
