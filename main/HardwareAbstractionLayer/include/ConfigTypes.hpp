#pragma once
#include "driver/ledc.h"
#include "driver/mcpwm.h"
#include "driver/i2c_master.h"

#include <vector>

struct I2CDeviceConfig {
    uint16_t deviceAddress = 0x68;
    i2c_addr_bit_len_t addressLenght = I2C_ADDR_BIT_LEN_7;
    uint32_t sclFreq = 400000;
    uint32_t sclWait = 0;
};

struct I2CBusConfig {
    gpio_num_t sdaPin;
    gpio_num_t sclPin;
    i2c_port_num_t port = I2C_NUM_0;
    i2c_clock_source_t clockSource = I2C_CLK_SRC_DEFAULT;
    uint8_t glitchIgnoreCount = 7;
    int interruptPriority = 0;
    uint32_t internalPullUp = 1;
};

struct GPIOConfig {
    gpio_num_t pinNum;
    gpio_mode_t gpioMode = GPIO_MODE_OUTPUT;
    gpio_pullup_t internalPullUp = GPIO_PULLUP_DISABLE;
    gpio_pulldown_t internalPullDown = GPIO_PULLDOWN_DISABLE;
    gpio_int_type_t interruptType = GPIO_INTR_DISABLE;
};
struct LEDCTimerConfig {
    ledc_timer_t timerNum;
    ledc_mode_t speedMode = LEDC_HIGH_SPEED_MODE;
    ledc_timer_bit_t dutyResolution = LEDC_TIMER_10_BIT;
    uint32_t frequency = 5000;
    ledc_clk_cfg_t clock = LEDC_AUTO_CLK;
};

struct LEDCChannelConfig {
    int pinNum;
    ledc_channel_t channelNum;
    ledc_timer_t timerNum;
    ledc_mode_t speedMode = LEDC_HIGH_SPEED_MODE;
    ledc_intr_type_t interruptType = LEDC_INTR_DISABLE;
    uint32_t duty = 0;
    int hpoint = 0;
};

struct LEDCConfig {
    std::vector<LEDCTimerConfig> timerConfigs;
    std::vector<LEDCChannelConfig> channelConfigs;
};

struct MCPWMTimerConfig {
};

struct MCPWMConfig {
    std::vector<MCPWMTimerConfig> timerConfigs;
};