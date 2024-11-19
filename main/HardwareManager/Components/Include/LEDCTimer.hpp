#pragma once
#include "interface/ILEDCTimer.hpp"

class LEDCTimer : public ILEDCTimer {
    public:
        LEDCTimer(const LEDCTimerConfig&);
        ~LEDCTimer() = default;
        LEDCTimer(const LEDCTimer&) = delete;
        LEDCTimer& operator=(const LEDCTimer&) = delete;
        LEDCTimer(LEDCTimer&&) = delete;
        LEDCTimer& operator=(LEDCTimer&&) = delete;

        //IHalComponent
        esp_err_t init() override;
    private:
        static constexpr const char* TAG = "LEDCTimer";

        esp_err_t notInitialized() const override;
        
        const LEDCTimerConfig m_config;
};