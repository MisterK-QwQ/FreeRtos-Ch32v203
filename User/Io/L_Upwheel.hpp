#pragma once
#include "Gpio.hpp"
#include "Expands/Expand.hpp"
class LUpWheel :public Gpio{
    TIMExpand TimData;
    LUpWheel():Gpio(GPIOA,{}){


    }

};