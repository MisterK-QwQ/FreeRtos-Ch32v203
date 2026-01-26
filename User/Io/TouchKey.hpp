#pragma once
#include "Gpio.hpp"
#include "Expands/Expand.hpp"
class TouchKey : public Gpio {
  public:
    bool LED=true;
    ADCExpand AdcData;
    TouchKey() : Gpio (GPIOA, {GPIO_Pin_2,GPIO_Speed_50MHz,GPIO_Mode_AIN}) {
        AdcData.Type=ExpandType::ADC;
        AdcData.ExpandMode=ADC1;
        AdcData.ADC_Channel=ADC_Channel_2;
        AdcData.IDATAR1=0x10;
        AdcData.RDATAR=0x8;


        AdcData.ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
        AdcData.ADC_InitStructure.ADC_ScanConvMode = DISABLE;
        AdcData.ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
        AdcData.ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
        AdcData.ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
        AdcData.ADC_InitStructure.ADC_NbrOfChannel = 1;

        xTaskCreate ((TaskFunction_t)task1_task,
                     (const char *)"task1s",
                     (uint16_t)256,
                     (void *)this,
                     (UBaseType_t)5,
                     (TaskHandle_t *)&AdcData.Task_Handler);


        AddExpand(AdcData);
        RegisterFunc((void*)oninit); 
    }
    static void oninit(void* _this){
      auto* m_this=(TouchKey*)_this;
      RCC_ADCCLKConfig(RCC_PCLK2_Div8);
      ADC_Init(ADC1, &m_this->AdcData.ADC_InitStructure);
      ADC_Cmd(ADC1, ENABLE);
      ADC1->CTLR1 |= (1<<26)|(1<<24);
    }
    static void task1_task(TouchKey*_this) {
      while (1) {
        uint16_t currentAdcValue = _this->AdcData.GetRDATAR();
        GPIO_WriteBit(GPIOA, GPIO_Pin_0,currentAdcValue>=4020 ? Bit_SET:Bit_RESET);
      }

       
    }
};