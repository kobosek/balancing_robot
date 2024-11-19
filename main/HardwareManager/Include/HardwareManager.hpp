#pragma once

#include "HardwareConfigTypes.hpp"
#include <memory>
#include <map>

class ILEDCTimer;
class ILEDCPWM;
class IGPIO;

class HardwareManager {
public:
    static HardwareManager& instance() {
        static HardwareManager s_manager;
        return s_manager;
    };

    esp_err_t configure(const HardwareConfig&);
    esp_err_t configureLEDCPWM(const LEDCConfig&);
    esp_err_t configureGPIO(const GPIOSConfig&);

private:
    static constexpr const char* TAG = "HardwareManager";

    HardwareManager() = default;

    esp_err_t configureAndInitializeTimers(const LEDCConfig&);
    esp_err_t configureAndInitializeChannels(const LEDCConfig&);

    std::map<ledc_timer_t, std::shared_ptr<ILEDCTimer>> m_highSpeedTimers;
    std::map<ledc_timer_t, std::shared_ptr<ILEDCTimer>> m_lowSpeedTimers;
    
    std::map<ledc_channel_t, std::shared_ptr<ILEDCPWM>> m_highSpeedChannels;
    std::map<ledc_channel_t, std::shared_ptr<ILEDCPWM>> m_lowSpeedChannels;

    std::map<gpio_num_t, std::shared_ptr<IGPIO>> m_gpios;

    std::vector<std::unique_ptr<IConfigValidator>> m_configValidators = {
        std::make_unique<LEDCConfigValidator>(),
        std::make_unique<GPIOSConfigValidator>()
    };
};
