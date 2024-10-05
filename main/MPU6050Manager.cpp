#include "include/MPU6050Manager.hpp"
#include <cmath>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

constexpr gpio_num_t I2C_MASTER_SDA_IO = GPIO_NUM_21;
constexpr gpio_num_t I2C_MASTER_SCL_IO = GPIO_NUM_22;

MPU6050Manager::MPU6050Manager()  {
    II2CBus I2CBus(SDA, SCL);
    II2CDevice I2CDevice;
    I2CDevice.init(I2CBus);
    _sensor = std::make_unique<MPU6050>(I2CDevice);
}

esp_err_t MPU6050Manager::init(const IRuntimeConfig& config) {
    ESP_LOGI(TAG, "Initializing MPU6050Manager");

    esp_err_t ret = _sensor.init(MPU6050Config());
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize MPU6050: %s", esp_err_to_name(ret));
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

float MPU6050Manager::calculateRoll(float& pitch) const {
    return 0.0f; //not implemented yet
}

float MPU6050Manager::calculateYaw(float& pitch) const {
    return 0.0f; //not implemented yet
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