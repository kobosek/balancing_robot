#include "include/GPIO.hpp"
#include "esp_err.h"
#include "esp_log.h"

constexpr int HIGH = 1;
constexpr int LOW = 0;

GPIO::GPIO(const GPIOConfig& p_config) : m_config(p_config) {}

esp_err_t GPIO::init() {
    gpio_config_t l_config = {
        .pin_bit_mask = 1ULL << m_config.pinNum,
        .mode = m_config.gpioMode,
        .pull_up_en = m_config.internalPullUp,
        .pull_down_en = m_config.internalPullDown,
        .intr_type = m_config.interruptType 
    };

    esp_err_t l_ret = gpio_config(&l_config);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure GPIO pin");
    }
    return l_ret;  
}

esp_err_t GPIO::setHigh() {
    esp_err_t l_ret = gpio_set_level(m_config.pinNum, HIGH);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set HIGH");
    }
    return l_ret;  
}

esp_err_t GPIO::setLow() {
    esp_err_t l_ret = gpio_set_level(m_config.pinNum, LOW);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set LOW");
    }
    return l_ret;  
}

int GPIO::getLevel() const { 
    return gpio_get_level(m_config.pinNum);
}

gpio_num_t GPIO::getPinNum() const {
    return m_config.pinNum
}