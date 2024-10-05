// #include "esp_log.h"

// #include "include/RuntimeConfig.hpp"
// #include "include/ComponentHandler.hpp"

// // extern "C" void app_main(void)
// // {
// //     esp_log_level_set("*", ESP_LOG_INFO);
// //     ESP_LOGI("main", "Starting Balancing Robot");

// //     // Initialize RuntimeConfig
// //     RuntimeConfig config;
// //     config.init("/spiffs/config.json");

// //     // Initialize ComponentHandler
// //     ComponentHandler handler;
// //     handler.init(config);

// //     ESP_LOGI("main", "All tasks and components initialized");

// //     // The FreeRTOS scheduler will now take over
// // }

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "include/MPU6050Manager.hpp"
#include "include/PIDController.hpp"
#include "include/MotorDriver.hpp"
#include "include/RuntimeConfig.hpp"

#include "math.h"

#define TAG "PID_DEBUG"

// Function prototypes
void pidControlTask(void* pvParameters);

extern "C" void app_main(void)
{
    esp_log_level_set("*", ESP_LOG_INFO);
    ESP_LOGI(TAG, "Starting Simplified PID Control Debug");

    // Create and start the PID control task
    xTaskCreate(pidControlTask, "PID_Control_Task", 4096, NULL, 5, NULL);
}

void pidControlTask(void* pvParameters)
{
    // Initialize components
    MPU6050Manager mpu;
    PIDController pid;
    MX1616H motor;
    RuntimeConfig config;

    // Initialize MPU6050
    esp_err_t ret = mpu.init(config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize MPU6050");
        vTaskDelete(NULL);
    }

    // Initialize PID controller
    PIDConfig pidConfig = {0.12, 0, 0, 1.0, -1.0, 1.0, 0.40, 0.9};  // Example values
    pid.setConfig(pidConfig);

    // Initialize motor driver
    ret = motor.init(config);  // Passing nullptr as we're not using RuntimeConfig here
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize motor driver");
        vTaskDelete(NULL);
    }

    float integral = 0.0f;
    float lastError = 0.0f;
    TickType_t lastWakeTime = xTaskGetTickCount();
    const TickType_t frequency = pdMS_TO_TICKS(10);  // 10ms loop time
    float pitch = 0.0f;

    while (true) {
        // Read sensor data
        pitch = mpu.calculatePitch(pitch);

        // Compute PID output
        float output = pid.compute(integral, lastError, pitch, 0.01);  // 0.01s as dt

        // Apply motor control
        motor.setSpeed(output);

        // Debug output
        ESP_LOGI(TAG, "Angle: %.2f, PID Output: %.5f", pitch, output);

        // Wait for the next cycle
        vTaskDelayUntil(&lastWakeTime, frequency);
    }
}
