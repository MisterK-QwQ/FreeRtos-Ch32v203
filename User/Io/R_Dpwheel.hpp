#pragma once
#include "Gpio.hpp"
#include "Expands/Expand.hpp"

class RDpWheel : public Gpio {
  public:
    TIMExpand RDTimeData;

    RDpWheel() : Gpio (GPIOB, {GPIO_Pin_1, GPIO_Speed_50MHz, GPIO_Mode_AF_PP}) {
        RDTimeData.TIM_Base.TIM_Period = 100 - 1;   
        RDTimeData.TIM_Base.TIM_Prescaler = 10000 - 1; 
        RDTimeData.TIM_Base.TIM_ClockDivision = TIM_CKD_DIV1;
        RDTimeData.TIM_Base.TIM_CounterMode = TIM_CounterMode_Up;

        RDTimeData.TIM_OC.TIM_OCMode = TIM_OCMode_PWM1;
        RDTimeData.TIM_OC.TIM_OutputState = TIM_OutputState_Enable;
        RDTimeData.TIM_OC.TIM_Pulse = 0;
        RDTimeData.TIM_OC.TIM_OCPolarity = TIM_OCPolarity_Low;

        RegisterFunc (init);
    }

    static auto init (RDpWheel *_this) -> void {
        RCC_APB2PeriphClockCmd (RCC_APB2Periph_AFIO, ENABLE);
        RCC_APB1PeriphClockCmd (RCC_APB1Periph_TIM3, ENABLE);

        TIM_TimeBaseInit (TIM3, &_this->RDTimeData.TIM_Base);
        TIM_OC4Init (TIM3, &_this->RDTimeData.TIM_OC);
        TIM_OC4PreloadConfig (TIM3, TIM_OCPreload_Enable);
        TIM_ARRPreloadConfig (TIM3, ENABLE);
        TIM_Cmd (TIM3, ENABLE);
    }

    static auto Task (RDpWheel *_this) -> void {

        while (true) {
            // TIM_SetCompare4 (TIM3, _this->i);
            vTaskDelay (20);
        }
    }
};