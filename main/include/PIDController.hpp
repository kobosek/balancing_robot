#pragma once

#include "interfaces/IPIDController.hpp"
#include "interfaces/ITask.hpp"


class PIDController : public IPIDController {
public:
    PIDController();
    ~PIDController();
    
    esp_err_t init(const IRuntimeConfig&) override;
    esp_err_t setConfig(const PIDConfig&) override;

    float compute(float&, float&, float, float) const override;
private:
    static constexpr const char* TAG = "PIDController";
  
    PIDConfig m_config;

    SemaphoreHandle_t m_mutex;

    float applyLimits(float value, float min, float max) const;
};
