#pragma once
#include "driver/ledc.h"
#include "driver/mcpwm.h"
#include "driver/i2c_master.h"
#include "esp_wifi_types.h"
#include "freertos/event_groups.h"
#include <vector>
#include <string>

typedef int esp_err_t;

enum class MPU6050AccelConfig : uint8_t {
    RANGE_2G = 0x00,  // ± 2g
    RANGE_4G = 0x08,  // ± 4g
    RANGE_8G = 0x10,  // ± 8g
    RANGE_16G = 0x18  // ± 16g
};

enum class MPU6050GyroConfig : uint8_t {
    RANGE_250_DEG = 0x00,  // ± 250 °/s
    RANGE_500_DEG = 0x08,  // ± 500 °/s
    RANGE_1000_DEG = 0x10, // ± 1000 °/s
    RANGE_2000_DEG = 0x18  // ± 2000 °/s
};

enum class MPU6050DLPFConfig : uint8_t {
    DLPF_OFF = 0x00,                     // Disables DLPF, 8kHz sampling rate
    DLPF_BW_184HZ_ACC_188HZ_GYRO = 0x01, // 184 Hz Acc, 188 Hz Gyro
    DLPF_BW_94HZ_ACC_98HZ_GYRO = 0x02,   // 94 Hz Acc, 98 Hz Gyro
    DLPF_BW_44HZ_ACC_42HZ_GYRO = 0x03,   // 44 Hz Acc, 42 Hz Gyro (Recommended for balancing)
    DLPF_BW_21HZ_ACC_20HZ_GYRO = 0x04,   // 21 Hz Acc, 20 Hz Gyro
    DLPF_BW_10HZ_ACC_10HZ_GYRO = 0x05,   // 10 Hz Acc, 10 Hz Gyro
    DLPF_BW_5HZ_ACC_5HZ_GYRO = 0x06,     // 5 Hz Acc, 5 Hz Gyro
};

enum class MPU6050SampleRateConfig : uint8_t {
    RATE_1KHZ = 0x00,  // 1 kHz sampling rate
    RATE_500HZ = 0x01, // 500 Hz sampling rate
    RATE_250HZ = 0x03, // 250 Hz sampling rate
    RATE_125HZ = 0x07, // 125 Hz sampling rate
};

struct MPU6050Config {
    MPU6050DLPFConfig dlpfConfig = MPU6050DLPFConfig::DLPF_BW_44HZ_ACC_42HZ_GYRO;
    MPU6050SampleRateConfig sampleRate = MPU6050SampleRateConfig::RATE_1KHZ;
    MPU6050AccelConfig accelRange = MPU6050AccelConfig::RANGE_2G;
    MPU6050GyroConfig gyroRange = MPU6050GyroConfig::RANGE_250_DEG;
};
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
    uint32_t maxDuty = 0;
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

struct WIFIConfig {
    std::string staSSID;
    std::string staPassword;
    wifi_auth_mode_t authMode;
};