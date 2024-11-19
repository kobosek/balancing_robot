#pragma once

typedef int esp_err_t;

enum class HalState {
    UNINITIALIZED,
    INITALIZED,
    ERROR
};

class IHalComponent {
    public:
        virtual ~IHalComponent() = default;
        virtual esp_err_t init() = 0;
        virtual HalState getState() const { return m_state; };

    protected:
        virtual bool isInitialized() const { return m_state == HalState::INITALIZED; };
        virtual esp_err_t notInitialized() const = 0;
        void setStateUninitialized() { m_state = HalState::UNINITIALIZED; };
        void setStateInitialized() { m_state = HalState::INITALIZED; };
        void setStateError() { m_state = HalState::ERROR; };

    private:
        HalState m_state = HalState::UNINITIALIZED;
};