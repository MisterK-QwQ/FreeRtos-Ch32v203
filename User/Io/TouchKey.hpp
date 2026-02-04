#pragma once
#include "GpioManager.hpp"
#include "LED.hpp"
#include "Utils/utils.hpp"
class TouchKey : public Gpio {
  public:
    ADCExpand AdcData;

    TouchKey() : Gpio (GPIOA, {GPIO_Pin_1, (GPIOSpeed_TypeDef)0, GPIO_Mode_AIN}) {
        AdcData.Modx = ADC1;
        AdcData.ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
        AdcData.ADC_InitStructure.ADC_ScanConvMode = DISABLE;
        AdcData.ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
        AdcData.ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
        AdcData.ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
        AdcData.ADC_InitStructure.ADC_NbrOfChannel = 1;
      //  RegisterFunc (oninit);
    };

#if (m_onInit)
    inline static auto oninit (TouchKey *_this) -> void {

    };
#endif

    inline static auto Task (TouchKey *_this) -> void {
        RCC_APB2PeriphClockCmd (RCC_APB2Periph_ADC1, ENABLE);
        RCC_ADCCLKConfig (RCC_PCLK2_Div8);
        ADC_Init (ADC1, &_this->AdcData.ADC_InitStructure);
        ADC_Cmd (ADC1, ENABLE);
        //ADC1->CTLR1 |= (1 << 26) | (1 << 24);
        ADC1->IDATAR1 = 0x20;
        ADC1->RDATAR = 0x10;    

        while (true) {
            if (1) {
                ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_7Cycles5);
                ADC_SoftwareStartConvCmd(ADC1, ENABLE);
                while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
              
                uint16_t currentAdcValue =ADC1->RDATAR; //Utils::adc_filter_exponential<uint16_t,2,10>(Math::clamp<int>(ADC1->RDATAR,1400,2500));
                GPIO_WriteBit (GPIOB, GPIO_Pin_1, currentAdcValue >= 4050 ? Bit_SET : Bit_RESET);
                printf("currentAdcValue%d\n\r",currentAdcValue);
                
                ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
            } else
                GPIO_WriteBit (GPIOB, GPIO_Pin_1, Bit_SET);
        }
    }
};
