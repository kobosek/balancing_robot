#pragma once
#include "interface/II2CDevice.hpp"
#include <memory>

class II2CBus;

class I2CDevice : public II2CDevice {
    public:
        I2CDevice(const I2CDeviceConfig&, const std::shared_ptr<II2CBus>);
        ~I2CDevice() = default;
        I2CDevice(const I2CDevice&) = delete;
        I2CDevice& operator=(const I2CDevice&) = delete;
        I2CDevice(I2CDevice&&) = delete;
        I2CDevice& operator=(I2CDevice&&) = delete;

        //IHalComponent
        esp_err_t init() override;

        //II2CDevice
        esp_err_t writeRegister(uint8_t, uint8_t) const override;
        esp_err_t readRegisters(uint8_t, uint8_t*, size_t) const override;
    private:
        static constexpr const char* TAG = "I2Device";

        esp_err_t notInitialized() const override;

        I2CDeviceConfig m_config;
        std::shared_ptr<II2CBus> m_i2cbus;
        i2c_master_dev_handle_t m_deviceHandle;
};