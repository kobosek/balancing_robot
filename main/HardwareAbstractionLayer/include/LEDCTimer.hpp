#pragma once
#include "interface/ILEDCTimer.hpp"

class LEDCTimer : public ILEDCTimer {
    public:
        LEDCTimer(const LEDCTimerConfig&);
        esp_err_t init() override;
        const LEDCTimerConfig& getConfig() const;
    private:
        static constexpr const char* TAG = "LEDCTimer";

        LEDCTimerConfig m_config;
};