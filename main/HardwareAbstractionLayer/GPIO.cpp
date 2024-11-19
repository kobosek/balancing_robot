#include "include/GPIO.hpp"
#include "esp_err.h"
#include "esp_log.h"

constexpr int HIGH = 1;
constexpr int LOW = 0;

GPIO::GPIO(const GPIOConfig& p_config) : m_config(p_config) {}

esp_err_t GPIO::init() {
    ESP_LOGD(TAG, "Initializing GPIO pin: %d, Mode: %d, Pull Up: %d, Pull Down: %d, Interrupt Type: %d", 
             m_config.pinNum, m_config.gpioMode, m_config.internalPullUp, m_config.internalPullDown, m_config.interruptType);

    gpio_config_t l_config = {
        .pin_bit_mask = 1ULL << m_config.pinNum,
        .mode = m_config.gpioMode,
        .pull_up_en = m_config.internalPullUp,
        .pull_down_en = m_config.internalPullDown,
        .intr_type = m_config.interruptType 
    };

    esp_err_t l_ret = gpio_config(&l_config);
    if (l_ret != ESP_OK) {
        setStateError();
        ESP_LOGE(TAG, "Failed to configure GPIO pin %d: %s", m_config.pinNum, esp_err_to_name(l_ret));
        return l_ret;
    }
    setStateInitialized();
    ESP_LOGI(TAG, "GPIO pin %d initialized successfully", m_config.pinNum);
    return ESP_OK;  
}

esp_err_t GPIO::setHigh() {
    ESP_LOGD(TAG, "Setting GPIO num %d to HIGH", m_config.pinNum);

    if(!isInitialized()) {
        return notInitialized();
    }
    esp_err_t l_ret = gpio_set_level(m_config.pinNum, HIGH);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set GPIO num %d to HIGH: %s", m_config.pinNum, esp_err_to_name(l_ret));
    }
    ESP_LOGV(TAG, "GPIO num %d set to HIGH", m_config.pinNum);
    return l_ret;  
}

esp_err_t GPIO::setLow() {
    ESP_LOGD(TAG, "Setting GPIO num %d to LOW", m_config.pinNum);

    if(!isInitialized()) {
        return notInitialized();
    }   
    esp_err_t l_ret = gpio_set_level(m_config.pinNum, LOW);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set GPIO num %d to LOW: %s", m_config.pinNum, esp_err_to_name(l_ret));
    }
    ESP_LOGV(TAG, "GPIO num %d set to LOW", m_config.pinNum);
    return l_ret;  
}

int GPIO::getLevel() const { 
    if(!isInitialized()) {
        notInitialized();
    }
    return gpio_get_level(m_config.pinNum);
}

gpio_num_t GPIO::getPinNum() const {
    return m_config.pinNum;
}

esp_err_t GPIO::notInitialized() const {
    ESP_LOGE(TAG, "GPIO num %d is not initialized: %s", m_config.pinNum, esp_err_to_name(ESP_ERR_INVALID_STATE));
    return ESP_ERR_INVALID_STATE;
}