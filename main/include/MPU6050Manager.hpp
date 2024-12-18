#pragma once

#include "interfaces/IMPU6050Manager.hpp"
#include "esp_log.h"
#include "include/mpu6050.hpp"

class MPU6050Manager : public IMPU6050Manager {
    public:
        esp_err_t init(const IRuntimeConfig&) override;
        esp_err_t onConfigUpdate(const IRuntimeConfig&) override;
        float calculatePitch(float&) const override; 

    private:
        static constexpr const char* TAG = "MPU6050Manager";

        esp_err_t calibrateGyro();    
        MPU6050 _sensor;
        static constexpr float ALPHA = 0.98f;
        static constexpr int CALIBRATION_SAMPLES = 100;
        static constexpr uint32_t I2C_MASTER_FREQ_HZ = 100000;
        float _gyro_error = 0.0f;
};