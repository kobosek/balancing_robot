#include "include/I2CDevice.hpp"
#include "interface/II2CBus.hpp"
#include "esp_err.h"
#include "esp_log.h"

I2CDevice::I2CDevice(const I2CDeviceConfig& p_config, const std::shared_ptr<II2CBus> p_i2cbus) : m_config(p_config), m_i2cbus(p_i2cbus), m_deviceHandle(NULL) {}

esp_err_t I2CDevice::init() {
    i2c_device_config_t l_deviceConfig = {
        .dev_addr_length = m_config.addressLenght,
        .device_address = m_config.deviceAddress,
        .scl_speed_hz = m_config.sclFreq,
        .scl_wait_us = m_config.sclWait
    };

    esp_err_t l_ret = m_i2cbus->registerDevice(l_deviceConfig, m_deviceHandle);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize I2C Device", esp_err_to_name(l_ret));
    }
    return l_ret;
}

esp_err_t I2CDevice::writeRegister(uint8_t l_registerAddress, uint8_t l_data) const {
    uint8_t l_buffer[2] = {l_registerAddress, l_data};
    esp_err_t l_ret = i2c_master_transmit(m_deviceHandle, l_buffer, sizeof(l_buffer), -1);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to write register 0x%02X: %s", l_register, esp_err_to_name(l_ret));
    }
    return l_ret;
}

esp_err_t I2CDevice::readRegisters(uint8_t l_registerAddress, uint8_t* l_data, size_t len) const {
    esp_err_t l_ret = i2c_master_transmit_receive(m_deviceHandle, &l_registerAddress, 1, l_data, len, -1);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read register 0x%02X: %s", l_registerAddress, esp_err_to_name(l_ret));
    }
    return l_ret;
}