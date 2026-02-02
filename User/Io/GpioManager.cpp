#include "GpioManager.hpp"
#include "TouchKey.hpp"
#include "LED.hpp"
#include "R_Upwheel.hpp"
#include "L_Upwheel.hpp"

// #include "R_Dpwheel.hpp"
// #include "L_Dpwheel.hpp"

auto GpioManager::IoClock (void *m_Periph, bool v) -> void {
    uint32_t RCC_APBPeriph = 0;
    if (m_Periph == GPIOA)
        RCC_APBPeriph = RCC_APB2Periph_GPIOA;
    else if (m_Periph == GPIOB)
        RCC_APBPeriph = RCC_APB2Periph_GPIOB;
    else if (m_Periph == GPIOC)
        RCC_APBPeriph = RCC_APB2Periph_GPIOC;
    else if (m_Periph == GPIOD)
        RCC_APBPeriph = RCC_APB2Periph_GPIOD;
    else if (m_Periph == AFIO)
        RCC_APBPeriph = RCC_APB2Periph_AFIO;

    if (RCC_APBPeriph)
        RCC_APB2PeriphClockCmd (RCC_APBPeriph, v ? ENABLE : DISABLE);
}

auto GpioManager::Init() -> void {
    InitGpio (GPIOB, GPIO_Pin_1, GPIO_Speed_50MHz, GPIO_Mode_Out_PP, true);  // 触摸需要
    TRegGpio (TouchKey, 256,5, TouchKey::Task);                             // 触摸案列
    TRegGpio (LED, 256, 5, LED::Task);                                       // 闪烁案列
    // TRegGpio (LUpWheel, 256, 5, LUpWheel::Task);  // 前左轮
    // TRegGpio(LDpWheel,128,5， LDpWheel::Task);  //后左轮
    // TRegGpio(RDpWheel,128,5,RDpWheel::Task);  //后右轮

    for (int u = 0u; u < m_count; u++) { 
         if (m_gpio[u]->initialized)
                    continue;
        IoClock (m_gpio[u]->Periph, true);
        GPIO_Init ((GPIO_TypeDef *)m_gpio[u]->Periph, &m_gpio[u]->def);
        m_gpio[u]->init();
        m_gpio[u]->initialized = true;
    }
    printf ("[INFO] Gpio Size(%d)\r\n", GpioManager::m_count);
}

auto GpioManager::AddGpio (Gpio *m_Gpio) -> void {
    ASSERT (m_count < MAX_GPIO_PINS, "注册的Gpio大于m_gpio数组大小.");
    m_gpio[m_count] = m_Gpio;
    m_count++;
}

// void GpioManager::AddGpio(void *m_Periph, GPIO_InitTypeDef m_def, bool defv) {
//     AddGpio(Gpio(m_Periph, m_def, defv));
// }
