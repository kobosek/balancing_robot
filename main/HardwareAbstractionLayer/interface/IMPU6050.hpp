#pragma once

#include "interface/IAccelerometer.hpp"
#include "interface/IGyroscope.hpp"
#include "interface/ITempSensor.hpp"

enum class MPU6050Register : uint8_t {
    PWR_MGMT_1 = 0x6B,
    SMPLRT_DIV = 0x19,
    DLPF_CONFIG = 0x1A,
    GYRO_CONFIG = 0x1B,
    ACCEL_CONFIG = 0x1C,
    ACCEL_XOUT_H = 0x3B,
    ACCEL_YOUT_H = 0x3D,
    ACCEL_ZOUT_H = 0x3F,
    TEMP_OUT_H = 0x41,
    GYRO_XOUT_H = 0x43,
    GYRO_YOUT_H = 0x45,
    GYRO_ZOUT_H = 0x47
};

enum class MPU6050PowerManagement : uint8_t {
    CLOCK_INTERNAL = 0x00 // Internal 8MHz oscillator
};

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

enum class MPU6050SampleRateDiv : uint8_t {
    RATE_1KHZ = 0x00,  // 1 kHz sampling rate
    RATE_500HZ = 0x01, // 500 Hz sampling rate
    RATE_250HZ = 0x03, // 250 Hz sampling rate
    RATE_125HZ = 0x07, // 125 Hz sampling rate
};

struct MPU6050Config {
    MPU6050DLPFConfig dlpfConfig = MPU6050DLPFConfig::DLPF_BW_44HZ_ACC_42HZ_GYRO;
    MPU6050SampleRateDiv sampleRate = MPU6050SampleRateDiv::RATE_1KHZ;
    MPU6050AccelConfig accelRange = MPU6050AccelConfig::RANGE_2G;
    MPU6050GyroConfig gyroRange = MPU6050GyroConfig::RANGE_250_DEG;
};

class IMPU6050 : public IAccelerometer, public IGyroscope, public ITempSensor {
    public:
        virtual ~IMPU6050() = default;
        virtual esp_err_t init(const MPU6050Config&) = 0;
        virtual esp_err_t updateConfig(const MPU6050Config&) = 0;
};