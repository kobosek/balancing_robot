#pragma once
#include "driver/ledc.h"
#include <vector>

struct LEDCTimerConfig {
    ledc_timer_t timerNum;
    ledc_mode_t speedMode = LEDC_HIGH_SPEED_MODE;
    ledc_timer_bit_t dutyResolution = LEDC_TIMER_10_BIT;
    uint32_t frequency = 5000;
    ledc_clk_cfg_t clock = LEDC_AUTO_CLK;
};

struct LEDCCHannelConfig {
    int pinNum;
    ledc_channel_t channelNum;
    ledc_intr_type_t interruptType = LEDC_INTR_DISABLE;
    uint32_t duty = 0;
    int hpoint = 0;
};

struct LEDCConfig {
    std::vector<LEDCTimerConfig> timerConfigs;
    std::vector<LEDCCHannelConfig> channelConfigs;
};