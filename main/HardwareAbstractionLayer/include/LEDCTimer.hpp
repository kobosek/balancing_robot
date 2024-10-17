#pragma once
#include "interface/ILEDCTimer.hpp"

class LEDCTimer : public ILEDCTimer {
    public:
        esp_err_t init(const LEDCTimerConfig&) override;
        const LEDCTimerConfig& getConfig() const;
    private:
        static constexpr const char* TAG = "LEDCTimer";
};