#pragma once
#include "../Gpio.hpp"
#include "Expand.hpp"

namespace {
constexpr TickType_t kLedBlinkPeriod = pdMS_TO_TICKS(200);
}

class LED :public Gpio{
public:
    Expand Data;
    bool Led = false;
    LED():Gpio(GPIOB,{GPIO_Pin_0,GPIO_Speed_50MHz,GPIO_Mode_Out_PP}){
    }
    inline static void Task(LED* _this) {
      TickType_t lastWakeTime = xTaskGetTickCount();
      while (true) {
           _this->Led = !_this->Led;
          GPIO_WriteBit(GPIOB, GPIO_Pin_0, _this->Led ? Bit_SET : Bit_RESET);
          vTaskDelayUntil(&lastWakeTime, kLedBlinkPeriod);
      }
    }
};

