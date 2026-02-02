#pragma once
#include "Gpio.hpp"
#include "Expands/Expand.hpp"

class LUpWheel : public Gpio {
  public:
    TIMExpand LUTimData;

    LUpWheel() : Gpio (GPIOA, {GPIO_Pin_6, GPIO_Speed_50MHz, GPIO_Mode_AF_PP}) {
        LUTimData.TIM_Base.TIM_Period = 100 - 1;       // ARR=99，计数0~99，占空比0~100%
        LUTimData.TIM_Base.TIM_Prescaler = 10000 - 1;  // PSC=4799，96MHz/4800/100=200Hz PWM
        LUTimData.TIM_Base.TIM_ClockDivision = TIM_CKD_DIV1;
        LUTimData.TIM_Base.TIM_CounterMode = TIM_CounterMode_Up;

        LUTimData.TIM_OC.TIM_OCMode = TIM_OCMode_PWM1;  // 仅改这一行
        LUTimData.TIM_OC.TIM_OutputState = TIM_OutputState_Enable;
        LUTimData.TIM_OC.TIM_Pulse = 0;
        LUTimData.TIM_OC.TIM_OCPolarity = TIM_OCPolarity_Low;

        RegisterFunc (init);
    }

    static auto init (LUpWheel *_this) -> void {
        RCC_APB2PeriphClockCmd (RCC_APB2Periph_AFIO, ENABLE);
        RCC_APB1PeriphClockCmd (RCC_APB1Periph_TIM3, ENABLE);

        TIM_TimeBaseInit (TIM3, &_this->LUTimData.TIM_Base);
        TIM_OC1Init (TIM3, &_this->LUTimData.TIM_OC);
        TIM_OC1PreloadConfig (TIM3, TIM_OCPreload_Enable);
        TIM_ARRPreloadConfig (TIM3, ENABLE);
        TIM_Cmd (TIM3, ENABLE);  // 启动TIM3
    }

    static auto Task (LUpWheel *_this) -> void {
        while (true) {
            // TIM_SetCompare1 (TIM3, _this->i);
            vTaskDelay (20);
        }
    }
};