#include "Include/I2CConfigValidator.hpp"
#include "esp_log.h"
#include "driver/gpio.h"
#include <unordered_set>
#include <algorithm>
#include <unordered_map>

esp_err_t I2CConfigValidator::validateConfig(const HardwareConfig& p_config) {
    ESP_LOGD(TAG, "Validating I2C configuration");

    const I2CConfig& l_config = p_config.i2cConfigs;

    esp_err_t l_ret = validateNumberOfBuses(l_config);
    if (l_ret != ESP_OK) { return l_ret; }

    l_ret = validateBusConfigs(l_config);
    if (l_ret != ESP_OK) { return l_ret; }

    l_ret = validateUniqueBusPorts(l_config);
    if (l_ret != ESP_OK) { return l_ret; }

    l_ret = validateUniqueBusPins(l_config);
    if (l_ret != ESP_OK) { return l_ret; }

    l_ret = validateDeviceConfigs(l_config);
    if (l_ret != ESP_OK) { return l_ret; }

    l_ret = validateUniqueDeviceAddresses(l_config);
    if (l_ret != ESP_OK) { return l_ret; }

    ESP_LOGI(TAG, "I2C configuration validated successfully");
    return ESP_OK;
}


esp_err_t I2CConfigValidator::validateNumberOfBuses(const I2CConfig& p_config) {
    ESP_LOGD(TAG, "Validating number of I2C buses: %zu", p_config.busConfigs.size());

    if (p_config.busConfigs.size() > I2C_NUM_MAX) {
        ESP_LOGE(TAG, "Too many I2C buses configured: %zu (maximum is %zu)", 
                p_config.busConfigs.size(), I2C_NUM_MAX);
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

esp_err_t I2CConfigValidator::validateBusConfigs(const I2CConfig& p_config) {
    ESP_LOGD(TAG, "Validating I2C bus configurations");
    
    for (const auto& l_busConfig : p_config.busConfigs) {
        esp_err_t l_ret = validateBusPins(l_busConfig);
        if (l_ret != ESP_OK) { return l_ret; }

        l_ret = validateBusPort(l_busConfig);
        if (l_ret != ESP_OK) { return l_ret; }
    }
    return ESP_OK;
}

esp_err_t I2CConfigValidator::validateBusPins(const I2CBusConfig& p_config) {
    ESP_LOGD(TAG, "Validating I2C bus pins SDA: %d, SCL: %d", p_config.sdaPin, p_config.sclPin);

    // Non-existent pins
    const std::unordered_set<int> l_nonExistentPins = {20, 24, 28, 29, 30, 31};
    
    // Flash interface pins (6-11) - generally unavailable
    const std::unordered_set<int> l_flashPins = {6, 7, 8, 9, 10, 11};
    
    // Input-only GPIOs (34-39)
    const std::unordered_set<int> l_inputOnlyPins = {34, 35, 36, 37, 38, 39};

    // Check SDA pin
    if (p_config.sdaPin < GPIO_NUM_0 || p_config.sdaPin >= GPIO_NUM_MAX) {
        ESP_LOGE(TAG, "SDA pin number out of range: %d", p_config.sdaPin);
        return ESP_ERR_INVALID_ARG;
    }

    if (l_nonExistentPins.find(p_config.sdaPin) != l_nonExistentPins.end()) {
        ESP_LOGE(TAG, "Non-existent SDA pin number: %d", p_config.sdaPin);
        return ESP_ERR_INVALID_ARG;
    }

    if (l_flashPins.find(p_config.sdaPin) != l_flashPins.end()) {
        ESP_LOGW(TAG, "SDA pin %d is connected to the flash interface, use with caution", p_config.sdaPin);
    }

    if (l_inputOnlyPins.find(p_config.sdaPin) != l_inputOnlyPins.end()) {
        ESP_LOGE(TAG, "SDA pin %d is input-only", p_config.sdaPin);
        return ESP_ERR_INVALID_ARG;
    }

    // Check SCL pin
    if (p_config.sclPin < GPIO_NUM_0 || p_config.sclPin >= GPIO_NUM_MAX) {
        ESP_LOGE(TAG, "SCL pin number out of range: %d", p_config.sclPin);
        return ESP_ERR_INVALID_ARG;
    }

    if (l_nonExistentPins.find(p_config.sclPin) != l_nonExistentPins.end()) {
        ESP_LOGE(TAG, "Non-existent SCL pin number: %d", p_config.sclPin);
        return ESP_ERR_INVALID_ARG;
    }

    if (l_flashPins.find(p_config.sclPin) != l_flashPins.end()) {
        ESP_LOGW(TAG, "SCL pin %d is connected to the flash interface, use with caution", p_config.sclPin);
    }

    if (l_inputOnlyPins.find(p_config.sclPin) != l_inputOnlyPins.end()) {
        ESP_LOGE(TAG, "SCL pin %d is input-only", p_config.sclPin);
        return ESP_ERR_INVALID_ARG;
    }

    if (p_config.sdaPin == p_config.sclPin) {
        ESP_LOGE(TAG, "SDA and SCL pins cannot be the same");
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

esp_err_t I2CConfigValidator::validateBusPort(const I2CBusConfig& p_config) {
    ESP_LOGD(TAG, "Validating I2C bus port: %d", p_config.port);

    if (p_config.port != I2C_NUM_0 && p_config.port != I2C_NUM_1) {
        ESP_LOGE(TAG, "Invalid I2C port number: %d", p_config.port);
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

esp_err_t I2CConfigValidator::validateUniqueBusPorts(const I2CConfig& p_config) {
    std::unordered_set<i2c_port_num_t> l_usedPorts;
    ESP_LOGD(TAG, "Validating unique I2C bus ports");
    
    for (const auto& l_busConfig : p_config.busConfigs) {
        if (!l_usedPorts.insert(l_busConfig.port).second) {
            ESP_LOGE(TAG, "Duplicate I2C port number: %d", l_busConfig.port);
            return ESP_ERR_INVALID_ARG;
        }
    }
    return ESP_OK;
}

esp_err_t I2CConfigValidator::validateUniqueBusPins(const I2CConfig& p_config) {
    std::unordered_set<gpio_num_t> l_usedPins;
    ESP_LOGD(TAG, "Validating unique I2C bus pins");
    
    for (const auto& l_busConfig : p_config.busConfigs) {
        if (!l_usedPins.insert(l_busConfig.sdaPin).second) {
            ESP_LOGE(TAG, "Duplicate SDA pin number: %d", l_busConfig.sdaPin);
            return ESP_ERR_INVALID_ARG;
        }
        if (!l_usedPins.insert(l_busConfig.sclPin).second) {
            ESP_LOGE(TAG, "Duplicate SCL pin number: %d", l_busConfig.sclPin);
            return ESP_ERR_INVALID_ARG;
        }
    }
    return ESP_OK;
}

esp_err_t I2CConfigValidator::validateDeviceConfigs(const I2CConfig& p_config) {
    ESP_LOGD(TAG, "Validating I2C device configurations");
    
    // Create a set of available bus ports
    std::unordered_set<i2c_port_num_t> l_availableBuses;
    for (const auto& l_busConfig : p_config.busConfigs) {
        l_availableBuses.insert(l_busConfig.port);
    }
    
    for (const auto& l_deviceConfig : p_config.deviceConfigs) {
        // Check if device's bus exists
        if (l_availableBuses.find(l_deviceConfig.busPort) == l_availableBuses.end()) {
            ESP_LOGE(TAG, "Device 0x%02X references non-existent I2C bus %d", 
                    l_deviceConfig.deviceAddress, l_deviceConfig.busPort);
            return ESP_ERR_INVALID_ARG;
        }

        esp_err_t l_ret = validateDeviceAddress(l_deviceConfig);
        if (l_ret != ESP_OK) { return l_ret; }

        l_ret = validateDeviceAddressLength(l_deviceConfig);
        if (l_ret != ESP_OK) { return l_ret; }

        l_ret = validateDeviceSCLFrequency(l_deviceConfig);
        if (l_ret != ESP_OK) { return l_ret; }
    }
    return ESP_OK;
}

esp_err_t I2CConfigValidator::validateDeviceAddress(const I2CDeviceConfig& p_config) {
    ESP_LOGD(TAG, "Validating I2C device address: 0x%02X", p_config.deviceAddress);

    // For 7-bit addressing
    if (p_config.addressLenght == I2C_ADDR_BIT_LEN_7) {
        if (p_config.deviceAddress > 0x7F) {
            ESP_LOGE(TAG, "Invalid 7-bit device address: 0x%02X", p_config.deviceAddress);
            return ESP_ERR_INVALID_ARG;
        }
    }
    // For 10-bit addressing
    else if (p_config.addressLenght == I2C_ADDR_BIT_LEN_10) {
        if (p_config.deviceAddress > 0x3FF) {
            ESP_LOGE(TAG, "Invalid 10-bit device address: 0x%02X", p_config.deviceAddress);
            return ESP_ERR_INVALID_ARG;
        }
    }

    // Reserved addresses (0000XXX and 1111XXX)
    if ((p_config.deviceAddress & 0x78) == 0x00 || (p_config.deviceAddress & 0x78) == 0x78) {
        ESP_LOGW(TAG, "Device address 0x%02X is in reserved range", p_config.deviceAddress);
    }

    return ESP_OK;
}

esp_err_t I2CConfigValidator::validateDeviceAddressLength(const I2CDeviceConfig& p_config) {
    ESP_LOGD(TAG, "Validating I2C device address length: %d", p_config.addressLenght);

    if (p_config.addressLenght != I2C_ADDR_BIT_LEN_7 && p_config.addressLenght != I2C_ADDR_BIT_LEN_10) {
        ESP_LOGE(TAG, "Invalid device address length: %d", p_config.addressLenght);
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

esp_err_t I2CConfigValidator::validateUniqueDeviceAddresses(const I2CConfig& p_config) {
    ESP_LOGD(TAG, "Validating unique I2C device addresses within each bus");
    
    // Create a map of bus port to device addresses
    std::unordered_map<i2c_port_num_t, std::unordered_set<uint16_t>> l_busDevices;
    
    // First, initialize the map with bus ports
    for (const auto& l_busConfig : p_config.busConfigs) {
        l_busDevices[l_busConfig.port] = std::unordered_set<uint16_t>();
    }
    
    // Then check each device
    for (const auto& l_deviceConfig : p_config.deviceConfigs) {
        auto& l_deviceSet = l_busDevices[l_deviceConfig.busPort];
        if (!l_deviceSet.insert(l_deviceConfig.deviceAddress).second) {
            ESP_LOGE(TAG, "Duplicate device address 0x%02X on I2C bus %d", 
                    l_deviceConfig.deviceAddress, l_deviceConfig.busPort);
            return ESP_ERR_INVALID_ARG;
        }
    }
    
    return ESP_OK;
}

esp_err_t I2CConfigValidator::validateDeviceSCLFrequency(const I2CDeviceConfig& p_config) {
    ESP_LOGD(TAG, "Validating I2C device SCL frequency: %d Hz", p_config.sclFreq);

    // ESP32 supports standard mode (100 kHz) and fast mode (400 kHz)
    // Some ESP32s also support high-speed mode (1 MHz or 3.4 MHz), but it's not recommended
    constexpr uint32_t MIN_SCL_FREQ = 100000;  // 100 kHz
    constexpr uint32_t MAX_SCL_FREQ = 400000;  // 400 kHz

    if (p_config.sclFreq < MIN_SCL_FREQ || p_config.sclFreq > MAX_SCL_FREQ) {
        ESP_LOGE(TAG, "Invalid SCL frequency: %d Hz (should be between %d and %d Hz)",
                p_config.sclFreq, MIN_SCL_FREQ, MAX_SCL_FREQ);
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

