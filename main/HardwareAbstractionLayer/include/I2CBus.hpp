#pragma once

#include "interface/II2CBus.hpp"

class IGPIO;

struct I2CBusConfig {
    gpio_num_t sdaPin;
    gpio_num_t sclPin;
    i2c_port_num_t port = I2C_NUM_0;
    i2c_clock_source_t clockSource = I2C_CLK_SRC_DEFAULT;
    uint8_t glitchIgnoreCount = 7;
    int interruptPriority = 0;
    uint32_t internalPullUp = 1;
};

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