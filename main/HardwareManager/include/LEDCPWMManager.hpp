#pragma once

#include "include/ConfigTypes.hpp"
#include <memory>
#include <map>

class IPWM;
class ILEDCTimer;

class LEDCPWMManager {
public:
    static LEDCPWMManager& instance() {
        static LEDCPWMManager s_manager;
        return s_manager;
    };

    esp_err_t configureTimersAndChannels(const LEDCConfig&);

private:
    LEDCPWMManager() = default;

    std::map<ledc_timer_t, std::shared_ptr<ILEDCTimer>> m_highSpeedTimers;
    std::map<ledc_timer_t, std::shared_ptr<ILEDCTimer>> m_lowSpeedTimers;

    std::map<ledc_channel_t, std::shared_ptr<IPWM>> m_highSpeedChannels;
    std::map<ledc_channel_t, std::shared_ptr<IPWM>> m_lowSpeedChannels;

    esp_err_t configureTimersAndChannels(const LEDCConfig&);
    esp_err_t configureAndInitializeTimers(const LEDCConfig&);
    esp_err_t configureAndInitializeChannels(const LEDCConfig&);

    esp_err_t validateConfig(const LEDCConfig&) const;
    esp_err_t validateNumberOfTimers(const LEDCConfig&, ledc_mode_t) const;
    esp_err_t validateTimerIds(const LEDCConfig&) const;
    esp_err_t validateUniqueTimerIds(const LEDCConfig&) const;
    esp_err_t validateNumberOfChannels(const LEDCConfig&, ledc_mode_t) const;
    esp_err_t validateChannelIds(const LEDCConfig&) const;
    esp_err_t validateUniqueChannelIds(const LEDCConfig&) const;
};
