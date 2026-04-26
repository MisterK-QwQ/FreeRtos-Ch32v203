#pragma once
#include "../GpioManager.hpp"
#include "LED.hpp"
#include "Utils/utils.hpp"

namespace {
constexpr uint16_t kTouchKeyThreshold = 4050;
}

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
        RegisterFunc (oninit);
    };

    static void oninit (TouchKey *_this) {
        RCC_APB2PeriphClockCmd (RCC_APB2Periph_ADC1, ENABLE);
        RCC_ADCCLKConfig (RCC_PCLK2_Div8);
        ADC_Init (ADC1, &_this->AdcData.ADC_InitStructure);
        ADC_Cmd (ADC1, ENABLE);
        TKey1->CTLR1 |= (1<<26)|(1<<24);        
        ADC1->IDATAR1 = 0x20;  //���
    };

    static void Task (TouchKey *_this)  {
        while (true) {
            if (GpioManager::Get<LED>(2)->Led) {   //����
                ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_7Cycles5);
                ADC1->RDATAR = 0x10;    //�ŵ�
                while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
                uint16_t currentAdcValue =ADC1->RDATAR;//Utils::adc_filter_exponential(ADC1->RDATAR);
                GPIO_WriteBit (GPIOB, GPIO_Pin_1, currentAdcValue >= kTouchKeyThreshold ? Bit_SET : Bit_RESET);
            } else
                GPIO_WriteBit (GPIOB, GPIO_Pin_1, Bit_SET);
            vTaskDelay(pdMS_TO_TICKS(1));
        }
    }
};

