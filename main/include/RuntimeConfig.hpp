#pragma once

#include "interfaces/IRuntimeConfig.hpp"

class RuntimeConfig : public IRuntimeConfig{
public:
    esp_err_t init(const std::string& p_filename = "/spiffs/config.json") override;
    esp_err_t save(const std::string& p_filename = "/spiffs/config.json") const override;

    // PID Controller parameters
    float getPidKp() const override { return pid_kp; }
    float getPidKi() const override { return pid_ki; }
    float getPidKd() const override { return pid_kd; }
    float getPidTargetAngle() const override { return pid_target_angle; }
    float getPidOutputMin() const override { return pid_output_min; }
    float getPidOutputMax() const override { return pid_output_max; }
    float getPidItermMin() const override { return pid_iterm_min; }
    float getPidItermMax() const override { return pid_iterm_max; }

    void setPidKp(float value) override { pid_kp = value; }
    void setPidKi(float value) override { pid_ki = value; }
    void setPidKd(float value) override { pid_kd = value; }
    void setPidTargetAngle(float value) override { pid_target_angle = value; }
    void setPidOutputMin(float value) override { pid_output_min = value; }
    void setPidOutputMax(float value) override { pid_output_max = value; }
    void setPidItermMin(float value) override { pid_iterm_min = value; }
    void setPidItermMax(float value) override { pid_iterm_max = value; }

    // MPU6050 parameters
    int getMpu6050CalibrationSamples() const override { return mpu6050_calibration_samples; }
    void setMpu6050CalibrationSamples(int value) override { mpu6050_calibration_samples = value; }

    // Main loop parameters
    int getMainLoopIntervalMs() const override { return main_loop_interval_ms; }
    void setMainLoopIntervalMs(int value) override { main_loop_interval_ms = value; }

    // WiFi parameters
    std::string getWifiSsid() const override { return wifi_ssid; }
    std::string getWifiPassword() const override { return wifi_password; }
    void setWifiSsid(const std::string& value) override { wifi_ssid = value; }
    void setWifiPassword(const std::string& value) override { wifi_password = value; }

    // JSON serialization/deserialization
    std::string toJson() const override;
    esp_err_t fromJson(const std::string& json) override;

private:
    esp_err_t init_spiffs();
    esp_err_t init_nvs_Flash();

    // PID Controller parameters
    float pid_kp;
    float pid_ki;
    float pid_kd;
    float pid_target_angle;
    float pid_output_min;
    float pid_output_max;
    float pid_iterm_min;
    float pid_iterm_max;

    // MPU6050 parameters
    int mpu6050_calibration_samples;

    // Main loop parameters
    int main_loop_interval_ms;

    // WiFi parameters
    std::string wifi_ssid;
    std::string wifi_password;
};