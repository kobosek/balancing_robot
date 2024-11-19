#pragma once
#include "driver/i2c_master.h"
#include "IHalComponent.hpp"

class II2CBus : public IHalComponent {
    public:
        virtual ~II2CBus() = default;
        virtual esp_err_t registerDevice(const i2c_device_config_t&, i2c_master_dev_handle_t&) const = 0;
};
