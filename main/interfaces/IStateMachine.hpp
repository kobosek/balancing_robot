#pragma once
#include "interfaces/ITask.hpp"

class IStateMachine : public ITask{
public:
    enum class State {
        INIT,
        IDLE,
        BALANCING,
        FALLING,
        ERROR
    };

    virtual esp_err_t setState(State newState) = 0;
    virtual State getState() const = 0;
    virtual ~IStateMachine() = default;
};