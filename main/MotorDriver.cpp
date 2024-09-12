#include "include/MotorDriver.hpp"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include <math.h>

#define L298_IN1_PIN GPIO_NUM_27
#define L298_IN2_PIN GPIO_NUM_26
#define L298_IN3_PIN GPIO_NUM_25
#define L298_IN4_PIN GPIO_NUM_33
#define PWM_L_PIN GPIO_NUM_32
#define PWM_R_PIN GPIO_NUM_14

static const char* TAG = "MotorDriver";  // Add this line for logging

esp_err_t L298N::init(const IRuntimeConfig& config)
{
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL<<L298_IN1_PIN) | (1ULL<<L298_IN2_PIN) | (1ULL<<L298_IN3_PIN) | (1ULL<<L298_IN4_PIN);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_HIGH_SPEED_MODE,
        .duty_resolution  = LEDC_TIMER_10_BIT,
        .timer_num        = LEDC_TIMER_0,
        .freq_hz          = 5000,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    ledc_channel_config_t ledc_channel[2] = {
        {
            .gpio_num       = PWM_L_PIN,
            .speed_mode     = LEDC_HIGH_SPEED_MODE,
            .channel        = LEDC_CHANNEL_0,
            .timer_sel      = LEDC_TIMER_0,
            .duty           = 0,
            .hpoint         = 0
        },
        {
            .gpio_num       = PWM_R_PIN,
            .speed_mode     = LEDC_HIGH_SPEED_MODE,
            .channel        = LEDC_CHANNEL_1,
            .timer_sel      = LEDC_TIMER_0,
            .duty           = 0,
            .hpoint         = 0
        }
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel[0]));
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel[1]));
    return ESP_OK;
}

esp_err_t L298N::setSpeed(float speed) const {
    if (speed < 0) {
        gpio_set_level(L298_IN1_PIN, 1);
        gpio_set_level(L298_IN2_PIN, 0);
        gpio_set_level(L298_IN3_PIN, 0);
        gpio_set_level(L298_IN4_PIN, 1);
    } else {
        gpio_set_level(L298_IN1_PIN, 0);
        gpio_set_level(L298_IN2_PIN, 1);
        gpio_set_level(L298_IN3_PIN, 1);
        gpio_set_level(L298_IN4_PIN, 0);
    }

    uint32_t duty = (uint32_t)(fabs(speed));
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, duty);
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, duty);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1);
    return ESP_OK;
}

esp_err_t L298N::onConfigUpdate(const IRuntimeConfig& config) {
    return ESP_OK;
}


#define IN1_PIN GPIO_NUM_14
#define IN2_PIN GPIO_NUM_27
#define IN3_PIN GPIO_NUM_26
#define IN4_PIN GPIO_NUM_25

esp_err_t MX1616H::init(const IRuntimeConfig& config)
{
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_HIGH_SPEED_MODE,
        .duty_resolution  = LEDC_TIMER_10_BIT,
        .timer_num        = LEDC_TIMER_0,
        .freq_hz          = 5000,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    ledc_channel_config_t ledc_channel[4] = {
        {
            .gpio_num       = IN1_PIN,
            .speed_mode     = LEDC_HIGH_SPEED_MODE,
            .channel        = LEDC_CHANNEL_0,
            .timer_sel      = LEDC_TIMER_0,
            .duty           = 0,
            .hpoint         = 0
        },
        {
            .gpio_num       = IN2_PIN,
            .speed_mode     = LEDC_HIGH_SPEED_MODE,
            .channel        = LEDC_CHANNEL_1,
            .timer_sel      = LEDC_TIMER_0,
            .duty           = 0,
            .hpoint         = 0
        },
        {
            .gpio_num       = IN3_PIN,
            .speed_mode     = LEDC_HIGH_SPEED_MODE,
            .channel        = LEDC_CHANNEL_2,
            .timer_sel      = LEDC_TIMER_0,
            .duty           = 0,
            .hpoint         = 0
        },
        {
            .gpio_num       = IN4_PIN,
            .speed_mode     = LEDC_HIGH_SPEED_MODE,
            .channel        = LEDC_CHANNEL_3,
            .timer_sel      = LEDC_TIMER_0,
            .duty           = 0,
            .hpoint         = 0
        }
    };

    for (int i = 0; i < 4; i++) {
        ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel[i]));
    }
    return ESP_OK;
}

esp_err_t MX1616H::setSpeed(float speed) const {
    uint32_t duty = (uint32_t)(fabs(speed));

    if (speed > 0) {
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, duty);
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, 0);
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, duty);
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3, 0);
    } else if (speed < 0) {
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 0);
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, duty);
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, 0);
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3, duty);
    } else {
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 0);
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, 0);
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, 0);
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3, 0);
    }

    for (int i = 0; i < 4; i++) {
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, (ledc_channel_t)i);
    }
    return ESP_OK;
}

esp_err_t MX1616H::onConfigUpdate(const IRuntimeConfig& config) {
    return ESP_OK;
}