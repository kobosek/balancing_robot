#pragma once

#include "interface/II2CDevice.hpp"
#include "driver/i2c_master.h"
#include <memory>

class II2CBus;

struct I2CDeviceConfig {
    uint16_t deviceAddress = 0x68;
    i2c_addr_bit_len_t addressLenght = I2C_ADDR_BIT_LEN_7;
    uint32_t sclFreq = 400000;
    uint32_t sclWait = 0;
};

class I2CDevice : public II2CDevice {
    public:
        I2CDevice(const I2CDeviceConfig&, const std::shared_ptr<II2CBus>);
        esp_err_t init() override;
        esp_err_t writeRegister(uint8_t, uint8_t) const override;
        esp_err_t readRegisters(uint8_t, uint8_t*, size_t) const override;
    private:
        static constexpr const char* TAG = "I2Device";
        I2CDeviceConfig m_config;
        std::shared_ptr<II2CBus> m_i2cbus;
        i2c_master_dev_handle_t m_deviceHandle;
};