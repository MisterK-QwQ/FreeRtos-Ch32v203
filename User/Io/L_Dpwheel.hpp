#pragma once
#include "Gpio.hpp"
#include "Expands/Expand.hpp"

class LDpWheel : public Gpio {
  public:
    TIMExpand LDTimData;

    LDpWheel() : Gpio (GPIOB, {GPIO_Pin_0, GPIO_Speed_50MHz, GPIO_Mode_AF_PP}) {
        LDTimData.TIM_Base.TIM_Period = 100 - 1;       // ARR=99，计数0~99，占空比0~100%
        LDTimData.TIM_Base.TIM_Prescaler = 10000 - 1;  // PSC=4799，96MHz/4800/100=200Hz PWM
        LDTimData.TIM_Base.TIM_ClockDivision = TIM_CKD_DIV1;
        LDTimData.TIM_Base.TIM_CounterMode = TIM_CounterMode_Up;

        LDTimData.TIM_OC.TIM_OCMode = TIM_OCMode_PWM1;  // 仅改这一行
        LDTimData.TIM_OC.TIM_OutputState = TIM_OutputState_Enable;
        LDTimData.TIM_OC.TIM_Pulse = 0;
        LDTimData.TIM_OC.TIM_OCPolarity = TIM_OCPolarity_Low;

        RegisterFunc (init);
    }
    

    static auto init (LDpWheel *_this) -> void {

    }


    static auto Task (LDpWheel *_this) -> void {
        RCC_APB2PeriphClockCmd (RCC_APB2Periph_AFIO, ENABLE);
        RCC_APB1PeriphClockCmd (RCC_APB1Periph_TIM3, ENABLE);

        TIM_TimeBaseInit (TIM3, &_this->LDTimData.TIM_Base);
        TIM_OC3Init (TIM3, &_this->LDTimData.TIM_OC);
        TIM_OC3PreloadConfig (TIM3, TIM_OCPreload_Enable);
        TIM_ARRPreloadConfig (TIM3, ENABLE);
        TIM_Cmd (TIM3, ENABLE);  // 启动TIM3

        while (true) {
            // TIM_SetCompare3 (TIM3, _this->i);
            vTaskDelay (20);
        }
    }
};