#pragma once
#include "include/ConfigTypes.hpp"
#include "IHalComponent.hpp"

class ILEDCTimer : public IHalComponent {
    public:
        virtual ~ILEDCTimer() = default;
};