#pragma once
#include "Gpio.hpp"
#include "TouchKey.hpp"

namespace GpioManager {
enum ClockSource {
    _AFIO,
    _GPIOA,
    _GPIOB,
    _GPIOC,
    _GPIOD,
    _ADC1,
    _ADC2,
    _TIM1,
    _TIM2,
    _TIM3,
    _TIM4,
    _TIM5,
    _SPI1,
    _SPI2,
    _USART1,
    _USART2,
    _USART3,
    _USART4,
    _I2C1,
    _I2C2,
    _DMA1,
    _CAN1,
    _USB,
    MAX
};

struct ClockItem {
    bool enabled = false;  // 时钟是否已使能（防止重复使能）
    uint16_t count = 0;    // 时钟使用计数（便于后续释放管理）
};

static ClockItem m_Clock[static_cast<size_t> (ClockSource::MAX)] = {0};

using IOPeriph_Type = enum {
    None,
    AHB_peripheral,
    APB2_peripheral,
    APB1_peripheral
};

static int m_count = 0u;
static Gpio m_gpio[MAX_GPIO_PINS];

bool ExpandClock (void *m_Mode) {
    ClockSource clkSource = ClockSource::MAX;     
    uint32_t RCC_APBPeriph = 0;                   
    IOPeriph_Type RccType = IOPeriph_Type::None;  

    if (m_Mode == nullptr) {
        return false;
    }

    /************************ 统一串联所有外设判断 ************************/
    // APB2 高速总线外设
    if (m_Mode == ADC1) {
        clkSource = ClockSource::_ADC1;
        RCC_APBPeriph = RCC_APB2Periph_ADC1;
        RccType = IOPeriph_Type::APB2_peripheral;
    } else if (m_Mode == ADC2) {
        clkSource = ClockSource::_ADC2;
        RCC_APBPeriph = RCC_APB2Periph_ADC2;
        RccType = IOPeriph_Type::APB2_peripheral;
    } else if (m_Mode == TIM1) {
        clkSource = ClockSource::_TIM1;
        RCC_APBPeriph = RCC_APB2Periph_TIM1;
        RccType = IOPeriph_Type::APB2_peripheral;
    } else if (m_Mode == SPI1) {
        clkSource = ClockSource::_SPI1;
        RCC_APBPeriph = RCC_APB2Periph_SPI1;
        RccType = IOPeriph_Type::APB2_peripheral;
    } else if (m_Mode == USART1) {
        clkSource = ClockSource::_USART1;
        RCC_APBPeriph = RCC_APB2Periph_USART1;
        RccType = IOPeriph_Type::APB2_peripheral;
    }
    // APB1 低速总线外设
    else if (m_Mode == TIM2) {
        clkSource = ClockSource::_TIM2;
        RCC_APBPeriph = RCC_APB1Periph_TIM2;
        RccType = IOPeriph_Type::APB1_peripheral;
    } else if (m_Mode == TIM3) {
        clkSource = ClockSource::_TIM3;
        RCC_APBPeriph = RCC_APB1Periph_TIM3;
        RccType = IOPeriph_Type::APB1_peripheral;
    } else if (m_Mode == TIM4) {
        clkSource = ClockSource::_TIM4;
        RCC_APBPeriph = RCC_APB1Periph_TIM4;
        RccType = IOPeriph_Type::APB1_peripheral;
    } else if (m_Mode == TIM5) {
        clkSource = ClockSource::_TIM5;
        RCC_APBPeriph = RCC_APB1Periph_TIM5;
        RccType = IOPeriph_Type::APB1_peripheral;
    } else if (m_Mode == SPI2) {
        clkSource = ClockSource::_SPI2;
        RCC_APBPeriph = RCC_APB1Periph_SPI2;
        RccType = IOPeriph_Type::APB1_peripheral;
    } else if (m_Mode == USART2) {
        clkSource = ClockSource::_USART2;
        RCC_APBPeriph = RCC_APB1Periph_USART2;
        RccType = IOPeriph_Type::APB1_peripheral;
    } else if (m_Mode == USART3) {
        clkSource = ClockSource::_USART3;
        RCC_APBPeriph = RCC_APB1Periph_USART3;
        RccType = IOPeriph_Type::APB1_peripheral;
    } else if (m_Mode == UART4) {
        clkSource = ClockSource::_USART4;
        RCC_APBPeriph = RCC_APB1Periph_UART4;
        RccType = IOPeriph_Type::APB1_peripheral;
    } else if (m_Mode == I2C1) {
        clkSource = ClockSource::_I2C1;
        RCC_APBPeriph = RCC_APB1Periph_I2C1;
        RccType = IOPeriph_Type::APB1_peripheral;
    } else if (m_Mode == I2C2) {
        clkSource = ClockSource::_I2C2;
        RCC_APBPeriph = RCC_APB1Periph_I2C2;
        RccType = IOPeriph_Type::APB1_peripheral;
    } else if (m_Mode == CAN1) {
        clkSource = ClockSource::_CAN1;
        RCC_APBPeriph = RCC_APB1Periph_CAN1;
        RccType = IOPeriph_Type::APB1_peripheral;
    }
    // AHB 高速总线外设
    else if (m_Mode == DMA1) {
        clkSource = ClockSource::_DMA1;
        RCC_APBPeriph = RCC_AHBPeriph_DMA1;
        RccType = IOPeriph_Type::AHB_peripheral;
    }

    // 后续时钟使能逻辑不变
    if (clkSource == ClockSource::MAX || RCC_APBPeriph == 0 || RccType == IOPeriph_Type::None) {
        return false;
    }
    if (!m_Clock[static_cast<size_t> (clkSource)].enabled) {
        switch (RccType) {
        case IOPeriph_Type::APB2_peripheral:
            RCC_APB2PeriphClockCmd (RCC_APBPeriph, ENABLE);
            break;
        case IOPeriph_Type::APB1_peripheral:
            RCC_APB1PeriphClockCmd (RCC_APBPeriph, ENABLE);
            break;
        case IOPeriph_Type::AHB_peripheral:
            RCC_AHBPeriphClockCmd (RCC_APBPeriph, ENABLE);
            break;
        default:
            return false;
        }
        m_Clock[static_cast<size_t> (clkSource)].enabled = true;
    }
    m_Clock[static_cast<size_t> (clkSource)].count++;
    return true;
}


bool IoClock (void *m_Periph) {
    ClockSource clkSource = ClockSource::MAX;
    uint32_t RCC_APBPeriph = 0;

    if (m_Periph == GPIOA) {
        clkSource = ClockSource::_GPIOA;
        RCC_APBPeriph = RCC_APB2Periph_GPIOA;
    } else if (m_Periph == GPIOB) {
        clkSource = ClockSource::_GPIOB;
        RCC_APBPeriph = RCC_APB2Periph_GPIOB;
    } else if (m_Periph == GPIOC) {
        clkSource = ClockSource::_GPIOC;
        RCC_APBPeriph = RCC_APB2Periph_GPIOC;
    } else if (m_Periph == GPIOD) {
        clkSource = ClockSource::_GPIOD;
        RCC_APBPeriph = RCC_APB2Periph_GPIOD;
    }

    if (!m_Clock[static_cast<size_t> (ClockSource::_AFIO)].enabled) {
        RCC_APB2PeriphClockCmd (RCC_APB2Periph_AFIO, ENABLE);
        m_Clock[static_cast<size_t> (ClockSource::_AFIO)].enabled = true;
        m_Clock[static_cast<size_t> (ClockSource::_AFIO)].count++;
    }

    if (clkSource != ClockSource::MAX && RCC_APBPeriph != 0) {
        if (!m_Clock[static_cast<size_t> (clkSource)].enabled) {
            RCC_APB2PeriphClockCmd (RCC_APBPeriph, ENABLE);
            m_Clock[static_cast<size_t> (clkSource)].enabled = true;
        }
        m_Clock[static_cast<size_t> (clkSource)].count++;
        return true;
    }
    return false;
}

static void AddGpio (void *m_Periph, GPIO_InitTypeDef m_def, bool defv = false) {
    m_gpio[m_count] = Gpio (m_Periph, m_def, defv);
    m_count++;
}

static void AddGpio (Gpio m_Gpio) {
    m_gpio[m_count] = m_Gpio;
    m_count++;
}

static void Init() {
    AddGpio (GPIOA, {GPIO_Pin_0, GPIO_Speed_50MHz, GPIO_Mode_Out_PP}, true);
    AddGpio (TouchKey());

    for (int u = 0u; u < m_count; u++) {
        auto &io = m_gpio[u];
        if (io.initialized)
            continue;
        IoClock(io.Periph);
        if(io.expand.Type!=ExpandType::None){
            if(io.expand.ExpandMode)
                ExpandClock(io.expand.ExpandMode);
            else printf("ExpandMode null");
        }
        GPIO_Init ((GPIO_TypeDef *)io.Periph, &io.def);
        io.init();
        io.initialized = true;
    }
}
};  // namespace GpioManager