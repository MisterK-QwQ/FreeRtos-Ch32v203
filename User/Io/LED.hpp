#pragma once
#include "Gpio.hpp"
#include "Expands/Expand.hpp"
class LED :public Gpio{
public:
    Expand Data;
    bool Led;
    LED():Gpio(GPIOB,{GPIO_Pin_0,GPIO_Speed_50MHz,GPIO_Mode_Out_PP}){
    }
    inline static auto Task(LED* _this)->void {
      while (true) {
           _this->Led = !_this->Led;
          GPIO_WriteBit(GPIOB, GPIO_Pin_0, _this->Led ? Bit_SET : Bit_RESET);
          vTaskDelay(200);
      }
    }
};