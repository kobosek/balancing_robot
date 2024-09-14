#pragma once
#include "interfaces/IComponent.hpp"

class IMPU6050Manager : public IComponent {
    public:
        virtual float calculatePitch(float&) const = 0; 
        virtual float calculateRoll(float&) const = 0; 
        virtual float calculateYaw(float&) const = 0; 
        virtual ~IMPU6050Manager() = default;   
};
