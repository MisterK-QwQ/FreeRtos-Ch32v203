#pragma once
#include "GpioManager.hpp"
#include "LED.hpp"
class TouchKey : public Gpio {
  public:
    ADCExpand AdcData;

    TouchKey() : Gpio (GPIOA, {GPIO_Pin_1,(GPIOSpeed_TypeDef)0, GPIO_Mode_AIN}) {
        AdcData.Modx = ADC1;
        AdcData.ADC_Channel = ADC_Channel_1;
        AdcData.IDATAR1 = 0x10;
        AdcData.RDATAR = 0x8;
        AdcData.ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
        AdcData.ADC_InitStructure.ADC_ScanConvMode = DISABLE;
        AdcData.ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
        AdcData.ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
        AdcData.ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
        AdcData.ADC_InitStructure.ADC_NbrOfChannel = 1;
        RegisterFunc (oninit);
    };

    inline static void oninit (TouchKey *_this) {
        RCC_APB2PeriphClockCmd (RCC_APB2Periph_ADC1, ENABLE);
        RCC_ADCCLKConfig (RCC_PCLK2_Div8);
        ADC_Init (ADC1, &_this->AdcData.ADC_InitStructure);
        ADC_Cmd (ADC1, ENABLE);
        ADC1->CTLR1 |= (1 << 26) | (1 << 24);
    };

    inline static void Task (TouchKey *_this) { //务必将任务函数都名为Task
        while (true) {
            if(GpioManager::Get<LED>(1)->Led){
                uint16_t currentAdcValue = _this->AdcData.GetRDATAR();
                GPIO_WriteBit (GPIOB, GPIO_Pin_1, currentAdcValue >= 4050 ? Bit_SET : Bit_RESET);
            }else  GPIO_WriteBit (GPIOB, GPIO_Pin_1, Bit_SET);

        }
    }
};
