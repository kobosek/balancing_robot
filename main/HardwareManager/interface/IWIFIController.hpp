#pragma once
#include "IHalComponent.hpp"

class IWIFIController : public IHalComponent {
public:
    virtual ~IWIFIController() = default;

    // Connection management
    virtual esp_err_t connect() = 0;
    virtual esp_err_t disconnect() = 0;
    
    // General operations
    virtual esp_err_t start() = 0;
    virtual esp_err_t stop() = 0;
};