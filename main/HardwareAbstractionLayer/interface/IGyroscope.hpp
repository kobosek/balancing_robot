#pragma once

struct AngularVelocityXYZ {
    float angularVelocityX;
    float angularVelocityY;
    float angularVelocityZ;
};

typedef int esp_err_t;

class IGyroscope {
    public:
        virtual ~IGyroscope() = default;
        virtual esp_err_t getAngularVelocityX(float&) const = 0;
        virtual esp_err_t getAngularVelocityY(float&) const = 0;
        virtual esp_err_t getAngularVelocityZ(float&) const = 0;
        virtual esp_err_t getAngularVelocityXYZ(AngularVelocityXYZ&) const = 0;    
};