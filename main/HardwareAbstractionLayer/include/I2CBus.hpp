#pragma once

#include "interface/II2CBus.hpp"
#include "include/ConfigTypes.hpp"

class IGPIO;

class I2CBus : public II2CBus {
    public:
        I2CBus(const I2CBusConfig&);
        esp_err_t init() override;
        esp_err_t registerDevice(const i2c_device_config_t& p_deviceConfig, i2c_master_dev_handle_t& p_deviceHandle) const override;
    private:
        static constexpr const char* TAG = "I2CBus";

        I2CBusConfig m_config;
        i2c_master_bus_handle_t m_busHandle;
};