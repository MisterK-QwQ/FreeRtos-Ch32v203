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
    void* ExpandMode;
    ExpandType Type;
    union{
        TaskHandle_t Task_Handler=nullptr;
    };
};

struct TIMExpand:public Expand{
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
};
struct  SPIExpand:public Expand{
    SPI_InitTypeDef  SPI_InitStructure;
};

struct I2CExpand : public Expand {
    I2C_InitTypeDef I2C_InitStructure;
};
struct ADCExpand:public Expand{
    ADC_InitTypeDef ADC_InitStructure;
    union{
        uint8_t ADC_Channel; 
        uint32_t IDATAR1;    //充电
        uint32_t RDATAR; //放电  
    };
    
uint32_t GetRDATAR(){
    ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_7Cycles5 );
    TKey1->IDATAR1 = this->IDATAR1;  // 充电时间（对齐官方案例0x10）
    TKey1->RDATAR = this->RDATAR;    // 放电时间（对齐官方案例0x8）
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));
    return (uint16_t) TKey1->RDATAR;
}

};
struct DAMExpand:public Expand{
    uint32_t DummyByte         = 0xff;
    DMA_InitTypeDef  DMA_InitStructure;
};