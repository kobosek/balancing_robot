#include "include/mpu6050.hpp"

#include "esp_check.h"
#include "esp_log.h"

static const char* TAG = "MPU6050";

MPU6050::MPU6050() : _dev_handle(NULL) {}

esp_err_t MPU6050::init(const i2c_port_t i2c_port, 
                        const gpio_num_t sda_io, 
                        const gpio_num_t scl_io,
                        const uint16_t i2c_addr,
                        const uint32_t i2c_freq) {

    i2c_master_bus_config_t bus_config = {
        .i2c_port = i2c_port,
        .sda_io_num = sda_io,
        .scl_io_num = scl_io,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags = {
            .enable_internal_pullup = true
        }
    };

    i2c_master_bus_handle_t bus_handle;
    ESP_RETURN_ON_ERROR(i2c_new_master_bus(&bus_config, &bus_handle), TAG, "Failed to create I2C master bus");

    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = i2c_addr,
        .scl_speed_hz = i2c_freq,
    };

    ESP_RETURN_ON_ERROR(i2c_master_bus_add_device(bus_handle, &dev_config, &_dev_handle), TAG, "Failed to add I2C device");

    return writeRegister(MPU6050Register::PWR_MGMT_1, MPU6050PowerManagement::CLOCK_INTERNAL);
}

esp_err_t MPU6050::setDLPFConfig(MPU6050DLPFConfig config) {
    return writeRegister(MPU6050Register::DLPF_CONFIG, config);
}

esp_err_t MPU6050::setSampleRate(MPU6050SampleRateDiv rate) {
    return writeRegister(MPU6050Register::SMPLRT_DIV, rate);
}

esp_err_t MPU6050::setAccelRange(MPU6050AccelConfig range) {
    esp_err_t ret = writeRegister(MPU6050Register::ACCEL_CONFIG, range);
    if (ret == ESP_OK) {
        switch(range) {
            case MPU6050AccelConfig::RANGE_2G: _accel_scale = 16384.0f; break;
            case MPU6050AccelConfig::RANGE_4G: _accel_scale = 8192.0f; break;
            case MPU6050AccelConfig::RANGE_8G: _accel_scale = 4096.0f; break;
            case MPU6050AccelConfig::RANGE_16G: _accel_scale = 2048.0f; break;
        }
    }
    return ret;
}

esp_err_t MPU6050::setGyroRange(MPU6050GyroConfig range) {
    esp_err_t ret = writeRegister(MPU6050Register::GYRO_CONFIG, range);
    if (ret == ESP_OK) {
        switch(range) {
            case MPU6050GyroConfig::RANGE_250_DEG: _gyro_scale = 131.0f; break;
            case MPU6050GyroConfig::RANGE_500_DEG: _gyro_scale = 65.5f; break;
            case MPU6050GyroConfig::RANGE_1000_DEG: _gyro_scale = 32.8f; break;
            case MPU6050GyroConfig::RANGE_2000_DEG: _gyro_scale = 16.4f; break;
        }
    }
    return ret;
}

esp_err_t MPU6050::getAcceleration(float& ax, float& ay, float& az) const {
    uint8_t data[6];
    esp_err_t ret = readRegisters(MPU6050Register::ACCEL_XOUT_H, data, 6);
    if (ret != ESP_OK) return ret;

    int16_t raw_ax = (data[0] << 8) | data[1];
    int16_t raw_ay = (data[2] << 8) | data[3];
    int16_t raw_az = (data[4] << 8) | data[5];

    ax = raw_ax / _accel_scale; 
    ay = raw_ay / _accel_scale;
    az = raw_az / _accel_scale;

    return ESP_OK;
}

esp_err_t MPU6050::getRotation(float& gx, float& gy, float& gz) const {
    uint8_t data[6];
    esp_err_t ret = readRegisters(MPU6050Register::GYRO_XOUT_H, data, 6);
    if (ret != ESP_OK) return ret;

    int16_t raw_gx = (data[0] << 8) | data[1];
    int16_t raw_gy = (data[2] << 8) | data[3];
    int16_t raw_gz = (data[4] << 8) | data[5];

    gx = raw_gx / _gyro_scale; 
    gy = raw_gy / _gyro_scale;
    gz = raw_gz / _gyro_scale;

    return ESP_OK;
}

esp_err_t MPU6050::writeRegister(MPU6050Register reg, auto data) {
    uint8_t write_buf[2] = {static_cast<uint8_t>(reg), static_cast<uint8_t>(data)};
    return i2c_master_transmit(_dev_handle, write_buf, sizeof(write_buf), -1);
}

esp_err_t MPU6050::readRegisters(MPU6050Register reg, uint8_t* data, size_t len) const {
    uint8_t reg_addr = static_cast<uint8_t>(reg);
    return i2c_master_transmit_receive(_dev_handle, &reg_addr, 1, data, len, -1);
}

