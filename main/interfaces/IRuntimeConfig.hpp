#pragma once

#include "interfaces/ITask.hpp"
#include "esp_err.h"
#include <string>

class IRuntimeConfig {
    public:
        virtual esp_err_t init(const std::string& p_filename = "/spiffs/config.json") = 0;
        virtual esp_err_t save(const std::string& p_filename = "/spiffs/config.json") const = 0;
        virtual ~IRuntimeConfig() = default;


        virtual PIDConfig getPidConfig() const = 0;
        virtual void setPidConfig(PIDConfig) = 0;

        // MPU6050 parameters
        virtual int getMpu6050CalibrationSamples() const = 0;
        virtual void setMpu6050CalibrationSamples(int) = 0;

        // Main loop parameters
        virtual int getMainLoopIntervalMs() const = 0;
        virtual void setMainLoopIntervalMs(int) = 0;

        // WiFi parameters
        virtual std::string getWifiSsid() const = 0;
        virtual std::string getWifiPassword() const = 0;
        virtual void setWifiSsid(const std::string&) = 0;
        virtual void setWifiPassword(const std::string&) = 0;

        // JSON serialization/deserialization
        virtual std::string toJson() const = 0;
        virtual esp_err_t fromJson(const std::string&) = 0;
};