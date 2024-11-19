#include "include/I2CDevice.hpp"
#include "interface/II2CBus.hpp"
#include "esp_err.h"
#include "esp_log.h"

I2CDevice::I2CDevice(const I2CDeviceConfig& p_config, const std::shared_ptr<II2CBus> p_i2cbus) : m_config(p_config), m_i2cbus(p_i2cbus), m_deviceHandle(NULL) {}

esp_err_t I2CDevice::init() {
    ESP_LOGD(TAG, "Initializing I2C Device Address: 0x%02X, SCL Freq: %d",
            m_config.deviceAddress, m_config.sclFreq);

    i2c_device_config_t l_deviceConfig = {
        .dev_addr_length = m_config.addressLenght,
        .device_address = m_config.deviceAddress,
        .scl_speed_hz = m_config.sclFreq,
        .scl_wait_us = m_config.sclWait
    };
    
    if(!m_i2cbus) {
        setStateError();
        ESP_LOGE(TAG, "I2C Bus is not initialized: %s", esp_err_to_name(ESP_ERR_INVALID_STATE));
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t l_ret = m_i2cbus->registerDevice(l_deviceConfig, m_deviceHandle);
    if (l_ret != ESP_OK) {
        setStateError();
        ESP_LOGE(TAG, "Failed to initialize I2C Device: %s", esp_err_to_name(l_ret));
        return l_ret;
    }
    setStateInitialized();
    ESP_LOGI(TAG, "I2C Device initialized successfully with address 0x%02X", m_config.deviceAddress);
    return ESP_OK;
}

esp_err_t I2CDevice::writeRegister(uint8_t l_registerAddress, uint8_t l_data) const {
    ESP_LOGD(TAG, "Writing register 0x%02X with data 0x%02X", l_registerAddress, l_data);

    if(!isInitialized()) {
        return notInitialized();
    }

    uint8_t l_buffer[2] = {l_registerAddress, l_data};
    esp_err_t l_ret = i2c_master_transmit(m_deviceHandle, l_buffer, sizeof(l_buffer), -1);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to write register 0x%02X: %s", l_registerAddress, esp_err_to_name(l_ret));
        return l_ret;
    }
    ESP_LOGV(TAG, "Register 0x%02X written with data 0x%02X", l_registerAddress, l_data);
    return ESP_OK;
}

esp_err_t I2CDevice::readRegisters(uint8_t l_registerAddress, uint8_t* l_data, size_t len) const {
    ESP_LOGD(TAG, "Reading register 0x%02X", l_registerAddress);

    if(!isInitialized()) {
        return notInitialized();
    }

    esp_err_t l_ret = i2c_master_transmit_receive(m_deviceHandle, &l_registerAddress, 1, l_data, len, -1);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read register 0x%02X: %s", l_registerAddress, esp_err_to_name(l_ret));
        return l_ret;
    }
    ESP_LOGV(TAG, "Register 0x%02X read successfully", l_registerAddress);
    return ESP_OK;
}

esp_err_t I2CDevice::notInitialized() const {
    ESP_LOGE(TAG, "I2C Device is not initialized: %s", esp_err_to_name(ESP_ERR_INVALID_STATE));
    return ESP_ERR_INVALID_STATE;
}