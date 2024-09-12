#include "include/MPU6050Manager.hpp"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <cmath>

static const char* TAG = "MPU6050 Manager";

#define I2C_MASTER_NUM I2C_NUM_0
#define MPU6050_ADDR 0x68

const gpio_num_t I2C_MASTER_SDA_IO = GPIO_NUM_21;
const gpio_num_t I2C_MASTER_SCL_IO = GPIO_NUM_22;

esp_err_t MPU6050Manager::init(const IRuntimeConfig&) {

    esp_err_t ret = _sensor.init(I2C_MASTER_NUM, GPIO_NUM_21, I2C_MASTER_SCL_IO, MPU6050_ADDR, I2C_MASTER_FREQ_HZ);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize MPU6050: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = _sensor.setAccelRange(MPU6050AccelConfig::RANGE_4G);
    if (ret != ESP_OK) return ret;

    ret = _sensor.setGyroRange(MPU6050GyroConfig::RANGE_500_DEG);
    if (ret != ESP_OK) return ret;

    ret = _sensor.setDLPFConfig(MPU6050DLPFConfig::DLPF_BW_44HZ_ACC_42HZ_GYRO);
    if (ret != ESP_OK) return ret;

    ret = _sensor.setSampleRate(MPU6050SampleRateDiv::RATE_1KHZ);
    if (ret != ESP_OK) return ret;

    return calibrateGyro();
}

float MPU6050Manager::calculatePitch(float& pitch) const {
    float acceleration_x, acceleration_y, acceleration_z;
    float omega_x, omega_y, omega_z;

    if (_sensor.getAcceleration(acceleration_x, acceleration_y, acceleration_z) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read acceleration");
        return 0.0f;
    }

    if (_sensor.getRotation(omega_x, omega_y, omega_z) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read rotation");
        return 0.0f;
    }

    float angleY_accel = std::atan2(-acceleration_x, std::sqrt(acceleration_y*acceleration_y + acceleration_z*acceleration_z)) * 180.0f / M_PI;
    omega_y -= _gyro_error;

    pitch = ALPHA * (pitch + omega_y * 0.01f) + (1 - ALPHA) * angleY_accel;

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

esp_err_t MPU6050Manager::onConfigUpdate(const IRuntimeConfig&) {
    return ESP_OK;
}