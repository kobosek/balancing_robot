#pragma once
#include "interface/IWIFIController.hpp"

class WIFIController : public IWIFIController {
public:
    WIFIController(const WIFIConfig&);
    ~WIFIController() = default;
    
    WIFIController(const WIFIController&) = delete;
    WIFIController& operator=(const WIFIController&) = delete;
    WIFIController(WIFIController&&) = delete;
    WIFIController& operator=(WIFIController&&) = delete;

    //IHalComponent
    esp_err_t init() override;

    // IWIFIController interface implementation
    esp_err_t connect() override;

private:
    static constexpr const char* TAG = "WIFIController";
    static void eventHandler(void*, esp_event_base_t, int32_t, void*);
    
    esp_err_t notInitialized() const override; 
    
    WIFIConfig m_config;
};