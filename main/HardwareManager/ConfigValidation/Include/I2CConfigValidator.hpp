#pragma once
#include "IConfigValidator.hpp"

class I2CConfigValidator : public IConfigValidator {
public:
    esp_err_t validateConfig(const HardwareConfig&) override;

private:
    static constexpr const char* TAG = "I2CConfigValidator";

    // Bus validations
    esp_err_t validateBusConfigs(const I2CConfig& p_config);
    esp_err_t validateNumberOfBuses(const I2CConfig& p_config);
    esp_err_t validateBusPins(const I2CBusConfig& p_config);
    esp_err_t validateBusPort(const I2CBusConfig& p_config);
    esp_err_t validateUniqueBusPorts(const I2CConfig& p_config);
    esp_err_t validateUniqueBusPins(const I2CConfig& p_config);

    // Device validations
    esp_err_t validateDeviceConfigs(const I2CConfig& p_config);
    esp_err_t validateDeviceAddress(const I2CDeviceConfig& p_config);
    esp_err_t validateDeviceAddressLength(const I2CDeviceConfig& p_config);
    esp_err_t validateUniqueDeviceAddresses(const I2CConfig& p_config);
    esp_err_t validateDeviceSCLFrequency(const I2CDeviceConfig& p_config);
};
