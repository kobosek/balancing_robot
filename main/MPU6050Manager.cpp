#include "include/MPU6050Manager.hpp"
#include <cmath>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define I2C_MASTER_NUM I2C_NUM_0
#define MPU6050_ADDR 0x68

const gpio_num_t I2C_MASTER_SDA_IO = GPIO_NUM_6;
const gpio_num_t I2C_MASTER_SCL_IO = GPIO_NUM_7;

esp_err_t MPU6050Manager::init(const IRuntimeConfig& config) {
    ESP_LOGI(TAG, "Initializing MPU6050Manager");

    esp_err_t ret = _sensor.init(I2C_MASTER_NUM, I2C_MASTER_SDA_IO, I2C_MASTER_SCL_IO, MPU6050_ADDR, I2C_MASTER_FREQ_HZ);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize MPU6050: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = _sensor.setAccelRange(MPU6050AccelConfig::RANGE_4G);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set accelerometer range: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = _sensor.setGyroRange(MPU6050GyroConfig::RANGE_500_DEG);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set gyroscope range: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = _sensor.setDLPFConfig(MPU6050DLPFConfig::DLPF_BW_44HZ_ACC_42HZ_GYRO);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set DLPF config: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = _sensor.setSampleRate(MPU6050SampleRateDiv::RATE_1KHZ);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set sample rate: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = calibrateGyro();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to calibrate gyroscope: %s", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(TAG, "MPU6050Manager initialized successfully");
    return ESP_OK;
}

float MPU6050Manager::calculatePitch(float& pitch) const {
    float acceleration_x, acceleration_y, acceleration_z;
    float omega_x, omega_y, omega_z;

    if (_sensor.getAcceleration(acceleration_x, acceleration_y, acceleration_z) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read acceleration");
        return pitch; 
    }

    if (_sensor.getRotation(omega_x, omega_y, omega_z) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read rotation");
        return pitch; 
    }

    float angleY_accel = std::atan2(-acceleration_x, std::sqrt(acceleration_y*acceleration_y + acceleration_z*acceleration_z)) * 180.0f / M_PI;
    omega_y -= _gyro_error;

    pitch = ALPHA * (pitch + omega_y * 0.01f) + (1 - ALPHA) * angleY_accel;

    ESP_LOGV(TAG, "Calculated pitch: %.2f", pitch);
    return pitch;
}

esp_err_t MPU6050Manager::calibrateGyro() {
    ESP_LOGI(TAG, "Calibrating gyroscope...");
    float omega_x, omega_y, omega_z;
    _gyro_error = 0.0f;

    for (int i = 0; i < CALIBRATION_SAMPLES; i++) {
        if (_sensor.getRotation(omega_x, omega_y, omega_z) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to read gyroscope during calibration");
            return ESP_FAIL;
        }
        _gyro_error += omega_y;
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    _gyro_error /= CALIBRATION_SAMPLES;
    ESP_LOGI(TAG, "Calibration complete. Gyro error: %.2f", _gyro_error);
    return ESP_OK;
}

esp_err_t MPU6050Manager::onConfigUpdate(const IRuntimeConfig& config) {
    ESP_LOGI(TAG, "Updating MPU6050Manager configuration");
    
    // Here you can add any configuration updates specific to the MPU6050
    // For example, if you want to change the accelerometer or gyroscope range based on config:
    
    // esp_err_t ret = _sensor.setAccelRange(static_cast<MPU6050AccelConfig>(config.getMpu6050AccelRange()));
    // if (ret != ESP_OK) {
    //     ESP_LOGE(TAG, "Failed to update accelerometer range: %s", esp_err_to_name(ret));
    //     return ret;
    // }
    
    // ret = _sensor.setGyroRange(static_cast<MPU6050GyroConfig>(config.getMpu6050GyroRange()));
    // if (ret != ESP_OK) {
    //     ESP_LOGE(TAG, "Failed to update gyroscope range: %s", esp_err_to_name(ret));
    //     return ret;
    // }

    // You might also want to recalibrate the gyroscope after a configuration change
    // return calibrateGyro();

    return ESP_OK;
}