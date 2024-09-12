#pragma once
#include "interfaces/IConfigObserver.hpp"

class IMPU6050Manager : public IConfigObserver {
    public:
        virtual float calculatePitch(float&) const = 0; 
        virtual ~IMPU6050Manager() = default;   
};
