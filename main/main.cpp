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

#include "driver/gpio.h"

#define TAG "PID_DEBUG"

void pidControlTask(void* pvParameters);

extern "C" void app_main(void)
{
    esp_log_level_set("*", ESP_LOG_INFO);
    ESP_LOGI(TAG, "Starting Simplified PID Control Debug");
    // Create and start the PID control task
    xTaskCreate(pidControlTask, "PID_Control_Task", 4096, NULL, 5, NULL);
}

volatile int position = 0;
volatile int lastPosition = 0;
volatile uint8_t last_state = 0;    // Last state of encoder (A and B)

// More accurate quadrature decoder
void IRAM_ATTR encoder_isr_handler(void *arg) {
    // Read the current state of A and B
    uint8_t current_state = (gpio_get_level(GPIO_NUM_8) << 1) | gpio_get_level(GPIO_NUM_5);

    // Determine direction based on state transitions
    if ((last_state == 0b00 && current_state == 0b01) ||
        (last_state == 0b01 && current_state == 0b11) ||
        (last_state == 0b11 && current_state == 0b10) ||
        (last_state == 0b10 && current_state == 0b00)) {
        position++; // Forward
    } else if ((last_state == 0b00 && current_state == 0b10) ||
               (last_state == 0b10 && current_state == 0b11) ||
               (last_state == 0b11 && current_state == 0b01) ||
               (last_state == 0b01 && current_state == 0b00)) {
        position--; // Reverse
    }

    last_state = current_state; // Update last state
}

float calculate_speed() {
    float speed = (float)(position - lastPosition);
    lastPosition = position;
    return speed;
};

void init_encoders() {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << GPIO_NUM_8) | (1ULL << GPIO_NUM_5),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .intr_type = GPIO_INTR_ANYEDGE
    };
    gpio_config(&io_conf);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(GPIO_NUM_8, encoder_isr_handler, NULL);
    gpio_isr_handler_add(GPIO_NUM_5, encoder_isr_handler, NULL);
}

void pidControlTask(void* pvParameters)
{
    RuntimeConfig config;
    config.init("/spiffs/config.json");
    ComponentHandler handler(config);
    handler.init();
   
    init_encoders();

    float angleSetPoint = 0.0f, speedSetPoint = 0.0f;

    float angleIntegral = 0.0f, angleLastError = 0.0f, speedIntegral = 0.0f, speedLastError = 0.0f;
    
    float pitch = 0.0f;
    float duty = 0.0f;

    const TickType_t xFrequency = pdMS_TO_TICKS(config.getMainLoopIntervalMs());  // 10ms loop time
    TickType_t xLastWakeTime = xTaskGetTickCount();


    while (true) {
        float current_speed = calculate_speed(); // Convert to microseconds
        pitch = handler.getMPU6050Manager().calculatePitch(pitch);
        speedSetPoint = handler.getAnglePIDController().compute(angleSetPoint, angleIntegral, angleLastError, pitch, config.getMainLoopIntervalMs());
        duty = handler.getSpeedPIDController().compute(speedSetPoint, speedIntegral, speedLastError, current_speed, config.getMainLoopIntervalMs());
        handler.getMotorDriver().setSpeed(duty);
        
        // Update global variables for web interface
        handler.getWebServer().update_telemetry(pitch, duty);

        // Log data periodically (e.g., every 100 iterations)
        static int logCounter = 0;
        if (++logCounter >= 100) {
            ESP_LOGI(TAG, "Pitch: %.2f, Speed Set Point: %.2f, Encoder Position: %d, Encoder Speed: %.2f, Duty: %.2f",
             pitch, speedSetPoint, position, current_speed, duty);
            logCounter = 0;
        }
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}
