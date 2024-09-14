#pragma once
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "nvs_flash.h"
#include "esp_spiffs.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_http_server.h"
#include "nvs_flash.h"
#include "esp_err.h"

class IRuntimeConfig;

struct SensorData {
    float pitch;
    float roll;
    float yaw;
    int64_t timestamp;
};

struct PIDOutput {
    float output;
};

struct TelemetryData {
    SensorData sensorData;
    PIDOutput pidOutput;
    float motorSpeed;
};

struct PIDConfig {
    float kp;
    float ki;
    float kd;
    float targetAngle;
    float itermMin;
    float itermMax;
    float outputMin;
    float outputMax;
};

class IComponent {
public:
    virtual esp_err_t init(const IRuntimeConfig&) = 0;
    virtual ~IComponent() = default;
};