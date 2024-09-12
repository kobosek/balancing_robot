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

extern "C" void app_main(void)
{
    RuntimeConfig config;
    config.init("/spiffs/config.json");

    ComponentHandler handler(config);
    handler.init();

    int64_t last_run = esp_timer_get_time();
    
    float integral, lastError = 0;
    float pitch = 0;

    while (1) {
        int64_t now = esp_timer_get_time();
        float dt = (now - last_run) / 1000000.0f;  // Convert to seconds

        pitch = handler.getMPU6050Manager().calculatePitch(pitch);
        float output = handler.getPIDController().compute(integral, lastError, pitch, dt);

        handler.getMotorDriver().setSpeed(output);

        // Update global variables for web interface
        handler.getWebServer().update_telemetry(pitch, output);

        // Delay to maintain consistent loop time
        int64_t time_since_last_run = now - last_run;
        if (time_since_last_run < LOOP_INTERVAL_MS * 1000) {
            vTaskDelay((LOOP_INTERVAL_MS * 1000 - time_since_last_run) / 1000 / portTICK_PERIOD_MS);
        }
        last_run = esp_timer_get_time();
    }
}
