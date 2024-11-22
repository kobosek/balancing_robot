#pragma once

#include "HardwareConfigTypes.hpp"
#include "ConfigValidation/Include/GPIOConfigValidator.hpp"
#include "ConfigValidation/Include/LEDCConfigValidator.hpp"
#include "ConfigValidation/Include/I2CConfigValidator.hpp"
#include "ConfigValidation/Include/WIFIConfigValidator.hpp"

#include <memory>
#include <map>

class ILEDCTimer;
class ILEDCPWM;
class IGPIO;
class II2CBus;
class II2CDevice;
class INVS;
class IWIFIController;

class HardwareManager {
public:
    static HardwareManager& instance() {
        static HardwareManager s_manager;
        return s_manager;
    };

    esp_err_t configure(const HardwareConfig&);
    esp_err_t configureLEDCPWM(const LEDCConfig&);
    esp_err_t configureGPIO(const GPIOSConfig&);
    esp_err_t configureI2C(const I2CConfig&);
    esp_err_t configureWIFI(const WIFIConfig&);

private:
    static constexpr const char* TAG = "HardwareManager";

    HardwareManager() = default;

    esp_err_t configureAndInitializeTimers(const LEDCConfig&);
    esp_err_t configureAndInitializeChannels(const LEDCConfig&);
    esp_err_t configureAndInitializeBuses(const I2CConfig&);
    esp_err_t configureAndInitializeDevices(const I2CConfig&);
    esp_err_t initializeNVS();

    std::map<std::pair<ledc_mode_t, ledc_timer_t>, std::shared_ptr<ILEDCTimer>> m_ledcTimers;
    std::map<std::pair<ledc_mode_t, ledc_channel_t>, std::shared_ptr<ILEDCPWM>> m_ledcChannels;
    std::map<gpio_num_t, std::shared_ptr<IGPIO>> m_gpios;
    
    std::map<i2c_port_num_t, std::shared_ptr<II2CBus>> m_i2cBuses;
    std::map<std::pair<i2c_port_num_t, uint16_t>, std::shared_ptr<II2CDevice>> m_i2cDevices;

    std::shared_ptr<INVS> m_nvs;
    std::shared_ptr<IWIFIController> m_wifiController;

    std::vector<std::unique_ptr<IConfigValidator>> m_configValidators = {
        std::make_unique<LEDCConfigValidator>(),
        std::make_unique<GPIOConfigValidator>(),
        std::make_unique<I2CConfigValidator>(),
        std::make_unique<WIFIConfigValidator>()
    };
};
