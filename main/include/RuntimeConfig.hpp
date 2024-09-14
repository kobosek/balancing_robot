#pragma once

#include "interfaces/IRuntimeConfig.hpp"

#include "esp_log.h"

class RuntimeConfig : public IRuntimeConfig{
public:
    esp_err_t init(const std::string& p_filename = "/spiffs/config.json") override;
    esp_err_t save(const std::string& p_filename = "/spiffs/config.json") const override;

    // PID Controller parameters
    PIDConfig getPidConfig() const override;
    void setPidConfig(PIDConfig);

    // MPU6050 parameters
    int getMpu6050CalibrationSamples() const override;
    void setMpu6050CalibrationSamples(int) override;

    // Main loop parameters
    int getMainLoopIntervalMs() const override;
    void setMainLoopIntervalMs(int) override;

    // WiFi parameters
    std::string getWifiSsid() const override;
    std::string getWifiPassword() const override;
    void setWifiSsid(const std::string& ) override;
    void setWifiPassword(const std::string&) override;

    // JSON serialization/deserialization
    std::string toJson() const override;
    esp_err_t fromJson(const std::string&) override;

private:
    static constexpr const char* TAG = "RuntimeConfig";

    PIDConfig m_pidConfig;

    // MPU6050 parameters
    int m_mpuCalibrationSamples;

    // Main loop parameters
    int m_mainLoopIntervalSamples;

    // WiFi parameters
    std::string m_wifiSSID;
    std::string m_wifiPassword;

    mutable SemaphoreHandle_t m_mutex;

    esp_err_t initNVS();
    esp_err_t initSPIFFS();
};