#pragma once
#include "IHalComponent.hpp"

class IWIFIController : public IHalComponent {
public:
    virtual ~IWIFIController() = default;

    // Connection management
    virtual esp_err_t connect() = 0;
};