#include "include/MPU6050.hpp"
#include "interface/II2CDevice.hpp"
#include "esp_err.h"
#include "esp_log.h"

MPU6050::MPU6050(std::unique_ptr<II2CDevice> p_i2cDevice) : m_i2cDevice(std::move(p_i2cDevice)) {}

template<typename RegisterValueType>
esp_err_t MPU6050::writeRegister(MPU6050Register l_register, RegisterValueType l_data) const {
    if (!m_i2cDevice) {
        ESP_LOGE(TAG, "I2C interface is not initialized");
        return ESP_ERR_INVALID_STATE;
    }
    return m_i2cDevice->writeRegister(static_cast<uint8_t>(l_register), static_cast<uint8_t>(l_data));
}

esp_err_t MPU6050::readRegisters(MPU6050Register l_register, uint8_t* l_data, size_t l_len) const {
    if (!m_i2cDevice) {
        ESP_LOGE(TAG, "I2C interface is not initialized");
        return ESP_ERR_INVALID_STATE;
    }
    return m_i2cDevice->readRegisters(static_cast<uint8_t>(l_register), l_data, l_len);
}
esp_err_t MPU6050::init(const MPU6050Config& p_config) {
    ESP_LOGI(TAG, "Initializing MPU6050");

    esp_err_t l_ret = writeRegister(MPU6050Register::PWR_MGMT_1, MPU6050PowerManagement::CLOCK_INTERNAL);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set power management: %s", esp_err_to_name(l_ret));
        return l_ret;
    }

    l_ret = updateConfig(p_config);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure sensor: %s", esp_err_to_name(l_ret));
        return l_ret;
    }

    ESP_LOGI(TAG, "MPU6050 initialized successfully");
    return ESP_OK;
}

esp_err_t MPU6050::updateConfig(const MPU6050Config& p_config) {
    esp_err_t l_ret = setDLPFConfig(p_config.dlpfConfig);
    if (l_ret != ESP_OK) return l_ret;

    l_ret = setSampleRate(p_config.sampleRate);
    if (l_ret != ESP_OK) return l_ret;

    l_ret = setAccelRange(p_config.accelRange);
    if (l_ret != ESP_OK) return l_ret;

    l_ret = setGyroRange(p_config.gyroRange);
    if (l_ret != ESP_OK) return l_ret;
}

esp_err_t MPU6050::setDLPFConfig(MPU6050DLPFConfig p_config) {
    ESP_LOGD(TAG, "Setting DLPF config");
    esp_err_t l_ret = writeRegister(MPU6050Register::DLPF_CONFIG, p_config);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set DLPF config: %s", esp_err_to_name(l_ret));
        return l_ret;
    }
    return ESP_OK
}

esp_err_t MPU6050::setSampleRate(MPU6050SampleRateDiv p_rate) {
    ESP_LOGD(TAG, "Setting sample rate");
    esp_err_t l_ret = writeRegister(MPU6050Register::SMPLRT_DIV, p_rate);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set sample rate: %s", esp_err_to_name(l_ret));
        return l_ret;
    }
    return ESP_OK
}

esp_err_t MPU6050::setAccelRange(MPU6050AccelConfig p_range) {
    ESP_LOGD(TAG, "Setting accelerometer range");
    esp_err_t l_ret = writeRegister(MPU6050Register::ACCEL_CONFIG, p_range);
    if (l_ret == ESP_OK) {
        switch(p_range) {
            case MPU6050AccelConfig::RANGE_2G: m_accelerationScale = 16384.0f; break;
            case MPU6050AccelConfig::RANGE_4G: m_accelerationScale = 8192.0f; break;
            case MPU6050AccelConfig::RANGE_8G: m_accelerationScale = 4096.0f; break;
            case MPU6050AccelConfig::RANGE_16G: m_accelerationScale = 2048.0f; break;
        }
    }
    return l_ret;
}

esp_err_t MPU6050::setGyroRange(MPU6050GyroConfig p_range) {
    ESP_LOGD(TAG, "Setting gyroscope range");
    esp_err_t l_ret = writeRegister(MPU6050Register::GYRO_CONFIG, p_range);
    if (l_ret == ESP_OK) {
        switch(p_range) {
            case MPU6050GyroConfig::RANGE_250_DEG: m_gyroscopeScale = 131.0f; break;
            case MPU6050GyroConfig::RANGE_500_DEG: m_gyroscopeScale = 65.5f; break;
            case MPU6050GyroConfig::RANGE_1000_DEG: m_gyroscopeScale = 32.8f; break;
            case MPU6050GyroConfig::RANGE_2000_DEG: m_gyroscopeScale = 16.4f; break;
        }
    }
    return l_ret;
}

template<typename T>
esp_err_t MPU6050::readSensorData(MPU6050Register p_startRegister, T& p_data) const {
    std::array<uint8_t, 2> l_rawData;
    esp_err_t l_ret = readRegisters(p_startRegister, l_rawData.data(), l_rawData.size());
    if (l_ret == ESP_OK) {
        int16_t l_rawValue = combineBytes(l_rawData[0], l_rawData[1]);
        if constexpr (std::is_same_v<T, float>) {
            if (p_startRegister == MPU6050Register::TEMP_OUT_H) {
                p_data = scaleTemperature(l_rawValue);
                ESP_LOGV(TAG, "Temperature: %.2f", p_data);
            } else if (p_startRegister >= MPU6050Register::ACCEL_XOUT_H && p_startRegister <= MPU6050Register::ACCEL_ZOUT_H) {
                p_data = scaleAcceleration(l_rawValue);
                ESP_LOGV(TAG, "Acceleration: %.2f", p_data);
            } else if (p_startRegister >= MPU6050Register::GYRO_XOUT_H && p_startRegister <= MPU6050Register::GYRO_ZOUT_H) {
                p_data = scaleGyroscope(l_rawValue);
                ESP_LOGV(TAG, "Angular Velocity: %.2f", p_data);
            }
        }
    } else {
        ESP_LOGE(TAG, "Failed to read sensor data: %s", esp_err_to_name(l_ret));
    }
    return l_ret;
}

template<typename T>
esp_err_t MPU6050::readSensorDataXYZ(MPU6050Register p_startRegister, T& p_dataXYZ) const {
    std::array<uint8_t, 6> l_rawData;
    esp_err_t l_ret = readRegisters(p_startRegister, l_rawData.data(), l_rawData.size());
    if (l_ret == ESP_OK) {
        int16_t l_rawX = combineBytes(l_rawData[0], l_rawData[1]);
        int16_t l_rawY = combineBytes(l_rawData[2], l_rawData[3]);
        int16_t l_rawZ = combineBytes(l_rawData[4], l_rawData[5]);

        if constexpr (std::is_same_v<T, AccelerationXYZ>) {
            p_dataXYZ = {
                scaleAcceleration(l_rawX),
                scaleAcceleration(l_rawY),
                scaleAcceleration(l_rawZ)
            };
            ESP_LOGV(TAG, "Acceleration: X=%.2f, Y=%.2f, Z=%.2f", 
                     p_dataXYZ.accelerationX, p_dataXYZ.accelerationY, p_dataXYZ.accelerationZ);
        } else if constexpr (std::is_same_v<T, AngularVelocityXYZ>) {
            p_dataXYZ = {
                scaleGyroscope(l_rawX),
                scaleGyroscope(l_rawY),
                scaleGyroscope(l_rawZ)
            };
            ESP_LOGV(TAG, "Angular Velocity: X=%.2f, Y=%.2f, Z=%.2f", 
                     p_dataXYZ.angularVelocityX, p_dataXYZ.angularVelocityY, p_dataXYZ.angularVelocityZ);
        }
    } else {
        ESP_LOGE(TAG, "Failed to read XYZ sensor data: %s", esp_err_to_name(l_ret));
    }
    return l_ret;
}
esp_err_t MPU6050::getAccelerationX(float& p_accelerationX) const {
    return readSensorData(MPU6050Register::ACCEL_XOUT_H, p_accelerationX);
}

esp_err_t MPU6050::getAccelerationY(float& p_accelerationY) const {
    return readSensorData(MPU6050Register::ACCEL_YOUT_H, p_accelerationY);
}

esp_err_t MPU6050::getAccelerationZ(float& p_accelerationZ) const {
    return readSensorData(MPU6050Register::ACCEL_ZOUT_H, p_accelerationZ);
}

esp_err_t MPU6050::getAccelerationXYZ(AccelerationXYZ& p_accelerationXYZ) const {
    return readSensorDataXYZ(MPU6050Register::ACCEL_XOUT_H, p_accelerationXYZ);
}

esp_err_t MPU6050::getAngularVelocityX(float& p_omegaX) const {
    return readSensorData(MPU6050Register::GYRO_XOUT_H, p_omegaX);
}

esp_err_t MPU6050::getAngularVelocityY(float& p_omegaY) const {
    return readSensorData(MPU6050Register::GYRO_YOUT_H, p_omegaY);
}

esp_err_t MPU6050::getAngularVelocityZ(float& p_omegaZ) const {
    return readSensorData(MPU6050Register::GYRO_ZOUT_H, p_omegaZ);
}

esp_err_t MPU6050::getAngularVelocityXYZ(AngularVelocityXYZ& p_omegaXYZ) const {
    return readSensorDataXYZ(MPU6050Register::GYRO_XOUT_H, p_omegaXYZ);
}

esp_err_t MPU6050::getTemperature(float& p_temperature) const {
    return readSensorData(MPU6050Register::TEMP_OUT_H, p_temperature);
}

int16_t MPU6050::combineBytes(uint8_t msb, uint8_t lsb) const {
    return (msb << 8) | lsb;
}

float MPU6050::scaleAcceleration(int16_t p_rawValue) const {
    return p_rawValue / m_accelerationScale;
}

float MPU6050::scaleGyroscope(int16_t p_rawValue) const {
    return p_rawValue / m_gyroscopeScale;
}

float MPU6050::scaleTemperature(int16_t p_rawValue) const {
    return p_rawValue / 340.0f + 36.53f;
}