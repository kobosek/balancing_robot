// main.cpp

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h" 
#include "esp_log.h"

#include "include/ComponentHandler.hpp"
#include "include/RuntimeConfig.hpp"

#include "interfaces/IWebServer.hpp"
#include "interfaces/IPIDController.hpp"
#include "interfaces/IMotorDriver.hpp"
#include "interfaces/IMPU6050Manager.hpp"

#define LOOP_INTERVAL_MS 10

#define TAG "PID_DEBUG"

void pidControlTask(void* pvParameters);

extern "C" void app_main(void)
{
    esp_log_level_set("*", ESP_LOG_INFO);
    ESP_LOGI(TAG, "Starting Simplified PID Control Debug");
    // Create and start the PID control task
    xTaskCreatePinnedToCore(pidControlTask, "PID_Control_Task", 4096, NULL, 5, NULL, 1);
}

void pidControlTask(void* pvParameters)
{
    RuntimeConfig config;
    config.init("/spiffs/config.json");
    ComponentHandler handler(config);
    handler.init();
   
    float integral = 0.0f, lastError = 0.0f, pitch = 0.0f;
    const TickType_t xFrequency = pdMS_TO_TICKS(10);  // 10ms loop time
    TickType_t xLastWakeTime = xTaskGetTickCount();

    while (true) {
        pitch = handler.getMPU6050Manager().calculatePitch(pitch);
        float output = handler.getPIDController().compute(integral, lastError, pitch, 0.01f);
        handler.getMotorDriver().setSpeed(output);
        
        // Update global variables for web interface
        handler.getWebServer().update_telemetry(pitch, output);

        // Log data periodically (e.g., every 100 iterations)
        static int logCounter = 0;
        if (++logCounter >= 100) {
            ESP_LOGI(TAG, "Pitch: %.2f, Output: %.2f", pitch, output);
            logCounter = 0;
        }

        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}
