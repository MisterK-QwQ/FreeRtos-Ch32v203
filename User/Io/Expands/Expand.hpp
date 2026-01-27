#pragma once

enum class ExpandType{
    None,
    TIM,
    SPI,
    ADC,
    I2C,
    DMA,
    USART,
    CAN
};

struct Expand{
    void* Modx=nullptr;
    TaskHandle_t Task_Handler=nullptr;
};

struct TIMExpand:public Expand{
    TIM_OCInitTypeDef TIM_OC;
    TIM_TimeBaseInitTypeDef TIM_Base;
};
struct  SPIExpand:public Expand{
    SPI_InitTypeDef  SPI_InitStructure;
};

struct I2CExpand : public Expand {
    I2C_InitTypeDef I2C_InitStructure;
};
struct ADCExpand:public Expand{
    ADC_InitTypeDef ADC_InitStructure;
    uint8_t ADC_Channel; 
    uint32_t IDATAR1;    //³äµç
    uint32_t RDATAR; //·Åµç  
    
    
    uint32_t GetRDATAR(){
        if(Modx == nullptr) return 0;
        ADC_TypeDef* adc = (ADC_TypeDef*)Modx;
        ADC_RegularChannelConfig(adc, ADC_Channel, 1, ADC_SampleTime_7Cycles5);
        adc->IDATAR1 = this->IDATAR1;
        adc->RDATAR = this->RDATAR;
        while(!ADC_GetFlagStatus(adc,ADC_FLAG_EOC ));
        return adc->RDATAR;
    }

};
struct DMAExpand:public Expand{
    uint32_t DummyByte         = 0xff;
    DMA_InitTypeDef  DMA_InitStructure;
};