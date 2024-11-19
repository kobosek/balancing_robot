#pragma once
#include "interface/IMPU6050.hpp"
#include <memory>

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

class II2CDevice;

class MPU6050 : public IMPU6050 {
    public:
        MPU6050(std::unique_ptr<II2CDevice>, const MPU6050Config&);
        ~MPU6050() = default;
        MPU6050(const MPU6050&) = delete;
        MPU6050& operator=(const MPU6050&) = delete;
        MPU6050(MPU6050&&) = delete;
        MPU6050& operator=(MPU6050&&) = delete;

        //IAccelerometer
        esp_err_t getAccelerationX(float&) const override;
        esp_err_t getAccelerationY(float&) const override;
        esp_err_t getAccelerationZ(float&) const override;
        esp_err_t getAccelerationXYZ(AccelerationXYZ&) const override;    

        //IGyroscope
        esp_err_t getAngularVelocityX(float&) const override;
        esp_err_t getAngularVelocityY(float&) const override;
        esp_err_t getAngularVelocityZ(float&) const override;
        esp_err_t getAngularVelocityXYZ(AngularVelocityXYZ&) const override;

        //ITemperature
        esp_err_t getTemperature(float&) const override;    

        //IHalComponent
        esp_err_t init() override;

    private:
        static constexpr const char* TAG = "MPU6050";

        esp_err_t notInitialized() const override;  

        MPU6050Config m_config;
        std::unique_ptr<II2CDevice> m_i2cDevice;
        float m_accelerationScale;
        float m_gyroscopeScale;

        esp_err_t configure(const MPU6050Config&);
        esp_err_t setDLPFConfig(MPU6050DLPFConfig);
        esp_err_t setSampleRate(MPU6050SampleRateConfig);
        esp_err_t setAccelRange(MPU6050AccelConfig);
        esp_err_t setGyroRange(MPU6050GyroConfig);

        template<typename RegisterValueType>
        esp_err_t writeRegister(MPU6050Register, RegisterValueType) const;

        esp_err_t readRegisters(MPU6050Register, uint8_t*, size_t) const;
        
        template<typename T>
        esp_err_t readSensorData(MPU6050Register, T&) const;

        template<typename T>
        esp_err_t readSensorDataXYZ(MPU6050Register, T&) const;

        int16_t combineBytes(uint8_t msb, uint8_t lsb) const;
        float scaleAcceleration(int16_t rawValue) const;
        float scaleGyroscope(int16_t rawValue) const;
        float scaleTemperature(int16_t rawValue) const;
};
