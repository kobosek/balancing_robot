#pragma once

#include "esp_err.h"

class IRuntimeConfig;

class IConfigObserver {
public:
    virtual esp_err_t init(const IRuntimeConfig&) = 0;
    virtual esp_err_t onConfigUpdate(const IRuntimeConfig&) = 0;
    virtual ~IConfigObserver() = default;
};