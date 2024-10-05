#pragma once
#include "driver/i2c_master.h"

typedef int esp_err_t;

class IGPIO;

class II2CBus {
    public:
        virtual ~II2CBus() = default;
        virtual esp_err_t init() = 0;
        virtual esp_err_t registerDevice(const i2c_device_config_t&, i2c_master_dev_handle_t&) const = 0;
};
