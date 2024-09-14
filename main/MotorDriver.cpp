#include "include/MotorDriver.hpp"
#include <math.h>


esp_err_t L298N::init(const IRuntimeConfig&) {
    ESP_LOGI(TAG, "Initializing L298N motor driver");

    esp_err_t l_ret = configureGPIO();
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure GPIO pins");
        return l_ret;
    }

    l_ret = configurePWM();
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure PWM");
        return l_ret;
    }

    ESP_LOGI(TAG, "L298N motor driver initialized successfully");
    return ESP_OK;
}

esp_err_t L298N::configureGPIO() {
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL<<IN1_PIN) | (1ULL<<IN2_PIN) | (1ULL<<IN3_PIN) | (1ULL<<IN4_PIN);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    
    return gpio_config(&io_conf);
}

esp_err_t L298N::configurePWM() {
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .duty_resolution = PWM_RESOLUTION,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = PWM_FREQUENCY,
        .clk_cfg = LEDC_AUTO_CLK
    };
    
    esp_err_t l_ret = ledc_timer_config(&ledc_timer);
    if (l_ret != ESP_OK) return l_ret;

    ledc_channel_config_t ledc_channel[2] = {
        {
            .gpio_num = PWM_L_PIN,
            .speed_mode = LEDC_HIGH_SPEED_MODE,
            .channel = LEDC_CHANNEL_0,
            .timer_sel = LEDC_TIMER_0,
            .duty = 0,
            .hpoint = 0
        },
        {
            .gpio_num = PWM_R_PIN,
            .speed_mode = LEDC_HIGH_SPEED_MODE,
            .channel = LEDC_CHANNEL_1,
            .timer_sel = LEDC_TIMER_0,
            .duty = 0,
            .hpoint = 0
        }
    };

    for (int i = 0; i < 2; i++) {
        l_ret = ledc_channel_config(&ledc_channel[i]);
        if (l_ret != ESP_OK) return l_ret;
    }

    return ESP_OK;
}

esp_err_t L298N::setSpeed(float p_speed) const {
    ESP_LOGD(TAG, "Setting L298N speed to %.2f", p_speed);

    p_speed = std::max(-1.0f, std::min(1.0f, p_speed));

    if (p_speed < 0) {
        gpio_set_level(IN1_PIN, 1);
        gpio_set_level(IN2_PIN, 0);
        gpio_set_level(IN3_PIN, 0);
        gpio_set_level(IN4_PIN, 1);
    } else {
        gpio_set_level(IN1_PIN, 0);
        gpio_set_level(IN2_PIN, 1);
        gpio_set_level(IN3_PIN, 1);
        gpio_set_level(IN4_PIN, 0);
    }

    uint32_t l_duty = static_cast<uint32_t>(std::abs(p_speed) * MAX_DUTY);

    esp_err_t l_ret = ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, l_duty);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set duty for LEDC channel 0");
        return l_ret;
    }
    l_ret = ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, l_duty);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set duty for LEDC channel 1");
        return l_ret;
    }
    l_ret = ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to update duty for LEDC channel 0");
        return l_ret;
    }
    l_ret = ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to update duty for LEDC channel 1");
        return l_ret;
    }

    return ESP_OK;
}

esp_err_t MX1616H::init(const IRuntimeConfig&) {
    ESP_LOGI(TAG, "Initializing MX1616H motor driver");

    esp_err_t l_ret = configurePWM();
    if (l_ret != ESP_OK) return l_ret;

    ESP_LOGI(TAG, "MX1616H motor driver initialized successfully");
    return ESP_OK;
}

esp_err_t MX1616H::configurePWM() {
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .duty_resolution = PWM_RESOLUTION,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = PWM_FREQUENCY,
        .clk_cfg = LEDC_AUTO_CLK
    };
    
    esp_err_t l_ret = ledc_timer_config(&ledc_timer);
    if (l_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure LEDC timer");
        return l_ret;
    }

    ledc_channel_config_t ledc_channel[4] = {
        {
            .gpio_num = IN1_PIN,
            .speed_mode = LEDC_HIGH_SPEED_MODE,
            .channel = LEDC_CHANNEL_0,
            .timer_sel = LEDC_TIMER_0,
            .duty = 0,
            .hpoint = 0
        },
        {
            .gpio_num = IN2_PIN,
            .speed_mode = LEDC_HIGH_SPEED_MODE,
            .channel = LEDC_CHANNEL_1,
            .timer_sel = LEDC_TIMER_0,
            .duty = 0,
            .hpoint = 0
        },
        {
            .gpio_num = IN3_PIN,
            .speed_mode = LEDC_HIGH_SPEED_MODE,
            .channel = LEDC_CHANNEL_2,
            .timer_sel = LEDC_TIMER_0,
            .duty = 0,
            .hpoint = 0
        },
        {
            .gpio_num = IN4_PIN,
            .speed_mode = LEDC_HIGH_SPEED_MODE,
            .channel = LEDC_CHANNEL_3,
            .timer_sel = LEDC_TIMER_0,
            .duty = 0,
            .hpoint = 0
        }
    };

    for (int i = 0; i < 4; i++) {
        l_ret = ledc_channel_config(&ledc_channel[i]);
        if (l_ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to configure LEDC channel %d", i);
            return l_ret;
        }
    }

    return ESP_OK;
}

esp_err_t MX1616H::setSpeed(float p_speed) const {
    ESP_LOGD(TAG, "Setting MX1616H speed to %.2f", p_speed);

    p_speed = std::max(-1.0f, std::min(1.0f, p_speed));

    uint32_t l_duty = static_cast<uint32_t>(std::abs(p_speed) * MAX_DUTY);

    if (p_speed > 0) {
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, l_duty);
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, 0);
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, l_duty);
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3, 0);
    } else if (p_speed < 0) {
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 0);
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, l_duty);
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, 0);
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3, l_duty);
    } else {
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 0);
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, 0);
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, 0);
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3, 0);
    }

    for (int i = 0; i < 4; i++) {
        esp_err_t l_ret = ledc_update_duty(LEDC_HIGH_SPEED_MODE, (ledc_channel_t)i);
        if (l_ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to update duty for LEDC channel %d", i);
            return l_ret;
        }
    }
    return ESP_OK;
}
