#pragma once
#include "HardwareAbstractionLayer/interface/ILEDCTimer.hpp"
#include "HardwareAbstractionLayer/interface/IPWM.hpp"
#include <memory>
#include <map>

class LEDCPWMManager {
public:
    static LEDCPWMManager& instance() {
        static LEDCPWMManager s_manager;
        return s_manager;
    }

    esp_err_t configure(const LEDCConfig&);

private:
    LEDCPWMManager() = default;

    std::map<ledc_timer_t, std::shared_ptr<ILEDCTimer>> m_highSpeedTimers;
    std::map<ledc_timer_t, std::shared_ptr<ILEDCTimer>> m_lowSpeedTimers;
};
