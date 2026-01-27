#pragma once
#include "Gpio.hpp"
#include "Expands/Expand.hpp"
class RUpWheel :public Gpio{
public:
    int i=0;
    TIMExpand TimData;
    RUpWheel():Gpio(GPIOA,{GPIO_Pin_9,GPIO_Speed_50MHz,GPIO_Mode_AF_PP}){
        TimData.TIM_Base.TIM_Period=100-1;
        TimData.TIM_Base.TIM_Prescaler=4800-1;
        TimData.TIM_Base.TIM_ClockDivision=TIM_CKD_DIV1;
        TimData.TIM_Base.TIM_CounterMode=TIM_CounterMode_Up;

        TimData.TIM_OC.TIM_OCMode = TIM_OCMode_PWM2;
        TimData.TIM_OC.TIM_OutputState = TIM_OutputState_Enable;
        TimData.TIM_OC.TIM_Pulse = 0;  
        TimData.TIM_OC.TIM_OCPolarity = TIM_OCPolarity_High;
        
        xTaskCreate((TaskFunction_t)RUpWheel_task,
                (const char *)"RUpWheeltask",
                (uint16_t)128,
                (void *)this,
                (UBaseType_t)5,
                (TaskHandle_t *)&TimData.Task_Handler);

        //AddExpand(TimData);
        RegisterFunc((GpioInitCallback)init);
    }

    static void init(RUpWheel* _this){
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
        TIM_TimeBaseInit(TIM1, &_this->TimData.TIM_Base);
        TIM_OC2Init(TIM1, &_this->TimData.TIM_OC);
        TIM_CtrlPWMOutputs(TIM1, ENABLE );
        TIM_OC2PreloadConfig( TIM1, TIM_OCPreload_Enable );
        TIM_ARRPreloadConfig( TIM1, ENABLE );
        TIM_Cmd( TIM1, ENABLE);
    }

    static void RUpWheel_task(RUpWheel* _this){
        while (true) {
            TIM_SetCompare2(TIM1, _this->i);
            _this->i++;
            if(_this->i >= 99) {
                _this->i = 0;
            }
            printf("%d\n\r", _this->i);
        vTaskDelay(10); 
        }
    }
};