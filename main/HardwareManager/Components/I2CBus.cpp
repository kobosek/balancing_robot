#include "include/I2CBus.hpp"
#include "interface/IGPIO.hpp"
#include "esp_err.h"
#include "esp_log.h"

I2CBus::I2CBus(const I2CBusConfig& p_config) : m_config(p_config), m_busHandle(NULL) {}

I2CBus::~I2CBus() {
    if (m_busHandle != NULL) {
        i2c_del_master_bus(m_busHandle);
    }
}

esp_err_t I2CBus::init() {
    ESP_LOGD(TAG, "Initializing I2C Bus SDA: %d, SCL: %d, Port: %d, Clock Source: %d, Glitch Ignore Count: %d, Internal Pullup: %d",
             m_config.sdaPin, m_config.sclPin, m_config.port, m_config.clockSource, m_config.glitchIgnoreCount, m_config.internalPullUp);

    i2c_master_bus_config_t l_busConfig = {
        .i2c_port = m_config.port,
        .sda_io_num = m_config.sdaPin,
        .scl_io_num = m_config.sclPin,
        .clk_source = m_config.clockSource,
        .glitch_ignore_cnt = m_config.glitchIgnoreCount,
        .flags = {
            .enable_internal_pullup = m_config.internalPullUp
        }
    };

    esp_err_t l_ret = i2c_new_master_bus(&l_busConfig, &m_busHandle);
    if (l_ret != ESP_OK) {
        setStateError();        
        ESP_LOGE(TAG, "Failed to register I2C master bus: %s", esp_err_to_name(l_ret));
        return l_ret;
    }
    setStateInitialized();
    ESP_LOGI(TAG, "I2C Bus initialized successfully on SDA: %d, SCL: %d", m_config.sdaPin, m_config.sclPin);
    return ESP_OK;
}

esp_err_t I2CBus::registerDevice(const i2c_device_config_t& p_deviceConfig, i2c_master_dev_handle_t& p_deviceHandle) const {
    ESP_LOGD(TAG, "Registering I2C device on address 0x%02X", p_deviceConfig.device_address);
    
    if (!isInitialized()) {        
        return notInitialized();
    }

    esp_err_t l_ret = i2c_master_bus_add_device(m_busHandle, &p_deviceConfig, &p_deviceHandle);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register I2C device on address 0x%02X: %s", p_deviceConfig.device_address, esp_err_to_name(l_ret));
        return l_ret;
    }
    ESP_LOGV(TAG, "I2C Device on address 0x%02X registered successfully", p_deviceConfig.device_address);
    return ESP_OK;
}

esp_err_t I2CBus::notInitialized() const {
    ESP_LOGE(TAG, "I2C Bus is not initialized on SDA: %d, SCL: %d: %s", m_config.sdaPin, m_config.sclPin, esp_err_to_name(ESP_ERR_INVALID_STATE));
    return ESP_ERR_INVALID_STATE;
}