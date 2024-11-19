#pragma once
#include "interface/INVS.hpp"

class NVS : public INVS {
    public:
        NVS() = default;
        ~NVS() = default;
        NVS(const NVS&) = delete;
        NVS& operator=(const NVS&) = delete;
        NVS(NVS&&) = delete;
        NVS& operator=(NVS&&) = delete;

        //IHalComponent
        esp_err_t init() override;

    private:
        static constexpr const char* TAG = "NVS";
        esp_err_t notInitialized() const override;
};