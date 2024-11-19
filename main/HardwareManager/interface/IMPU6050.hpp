#pragma once
#include "interface/IAccelerometer.hpp"
#include "interface/IGyroscope.hpp"
#include "interface/ITempSensor.hpp"
#include "interface/IHalComponent.hpp"


class IMPU6050 : public IAccelerometer, public IGyroscope, public ITempSensor, public IHalComponent {
    public:
        virtual ~IMPU6050() = default;
};