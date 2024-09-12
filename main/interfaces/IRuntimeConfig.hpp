#pragma once
#include "esp_err.h"
#include <string>

class IRuntimeConfig {
    public:
        virtual esp_err_t init(const std::string& p_filename = "/spiffs/config.json") = 0;
        virtual esp_err_t save(const std::string& p_filename = "/spiffs/config.json") const = 0;
        virtual ~IRuntimeConfig() = default;


        virtual float getPidKp() const = 0;
        virtual float getPidKi() const = 0;
        virtual float getPidKd() const = 0;
        virtual float getPidTargetAngle() const = 0;
        virtual float getPidOutputMin() const = 0;
        virtual float getPidOutputMax() const = 0;
        virtual float getPidItermMin() const = 0;
        virtual float getPidItermMax() const = 0;

        virtual void setPidKp(float) = 0;
        virtual void setPidKi(float) = 0;
        virtual void setPidKd(float) = 0;
        virtual void setPidTargetAngle(float) = 0;
        virtual void setPidOutputMin(float) = 0;
        virtual void setPidOutputMax(float) = 0;
        virtual void setPidItermMin(float) = 0;
        virtual void setPidItermMax(float) = 0;

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