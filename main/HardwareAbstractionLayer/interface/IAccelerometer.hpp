#pragma once

struct AccelerationXYZ {
    float accelerationX;
    float accelerationY;
    float accelerationZ;
};

typedef int esp_err_t;

class IAccelerometer {
    public:
        virtual ~IAccelerometer() = default;
        virtual esp_err_t getAccelerationX(float&) const = 0;
        virtual esp_err_t getAccelerationY(float&) const = 0;
        virtual esp_err_t getAccelerationZ(float&) const = 0;
        virtual esp_err_t getAccelerationXYZ(AccelerationXYZ&) const = 0;    
};