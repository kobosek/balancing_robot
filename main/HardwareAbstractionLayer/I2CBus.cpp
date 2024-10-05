#include "include/I2CBus.hpp"
#include "interface/IGPIO.hpp"
#include "esp_err.h"
#include "esp_log.h"

I2CBus::I2CBus(const I2CBusConfig& p_config) : m_config(p_config), m_busHandle(NULL) {}

esp_err_t I2CBus::init() {
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
        ESP_LOGE(TAG, "Failed to register I2C master bus: %s", esp_err_to_name(l_ret));
    }
    return l_ret;
}

esp_err_t I2CBus::registerDevice(const i2c_device_config_t& p_deviceConfig, i2c_master_dev_handle_t& p_deviceHandle) const {
    esp_err_t l_ret = i2c_master_bus_add_device(m_busHandle, &p_deviceConfig, &p_deviceHandle);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register I2C device: %s", esp_err_to_name(l_ret));
    }
    return l_ret;
};