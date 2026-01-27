#pragma once
#include "Gpio.hpp"
#include "Expands/Expand.hpp"
class LED :public Gpio{
public:
    Expand Data;
    LED():Gpio(GPIOB,{GPIO_Pin_0,GPIO_Speed_50MHz,GPIO_Mode_Out_PP}){

                     
    }
    inline static void Task(LED* _this) {
      while (true) {
        GPIO_WriteBit(GPIOB, GPIO_Pin_0,Bit_RESET);
        vTaskDelay(200); 
        GPIO_WriteBit(GPIOB, GPIO_Pin_0,Bit_SET);
        vTaskDelay(200); 
      }
    }
};