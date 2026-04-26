#include "GpioManager.hpp"
#include "Expands/TouchKey.hpp"
#include "Expands/LED.hpp"
#include "Expands/R_Upwheel.hpp"
#include "Expands/L_Upwheel.hpp"
#include "Expands/Wwdg.hpp"
// #include "R_Dpwheel.hpp"
// #include "L_Dpwheel.hpp"

namespace {
constexpr uint16_t kDefaultTaskStack = 256;
constexpr UBaseType_t kWwdgTaskPriority = 7;
constexpr UBaseType_t kTouchKeyTaskPriority = 5;
constexpr UBaseType_t kLedTaskPriority = 7;
}

auto GpioManager::IoClock (GPIO_TypeDef *m_Periph, bool v) -> void {
    if (m_Periph == nullptr) return;
    FunctionalState cmd = v ? ENABLE : DISABLE;
    if (m_Periph == GPIOA) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, cmd);
    else if (m_Periph == GPIOB) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, cmd);
    else if (m_Periph == GPIOC) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, cmd);
    else if (m_Periph == GPIOD) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, cmd);
    else if ((AFIO_TypeDef*)m_Periph == AFIO) RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, cmd);
}

auto GpioManager::Init() -> void {
    InitGpio (GPIOB, GPIO_Pin_1, GPIO_Speed_50MHz, GPIO_Mode_Out_PP, true);  // ������Ҫ
 
    TRegGpio (Wwdg, kDefaultTaskStack, kWwdgTaskPriority, Wwdg::Task);
    TRegGpio (TouchKey, kDefaultTaskStack, kTouchKeyTaskPriority, TouchKey::Task);
    TRegGpio (LED, kDefaultTaskStack, kLedTaskPriority, LED::Task);
    // TRegGpio (LUpWheel, 256, 5, LUpWheel::Task);  // ǰ����
    // TRegGpio(LDpWheel,128,5�� LDpWheel::Task);  //������
    // TRegGpio(RDpWheel,128,5,RDpWheel::Task);  //������

    for (uint8_t  u = 0u; u < m_count; u++) { 
         if (m_gpio[u] == nullptr || m_gpio[u]->initialized) continue;
        IoClock (m_gpio[u]->Periph, true);
        GPIO_Init (m_gpio[u]->Periph, &m_gpio[u]->def);
        m_gpio[u]->init();  //��ʼ��ע���Gpio
        m_gpio[u]->initialized = true;
    }
    printf ("[INFO] Gpio Size(%d)\r\n", GpioManager::m_count);
}

auto GpioManager::AddGpio (Gpio *m_Gpio) -> void {
    ASSERT (m_count < MAX_GPIO_PINS, "ע���Gpio����m_gpio�����С.");
    m_gpio[m_count] = m_Gpio;
    m_count++;
}

// void GpioManager::AddGpio(void *m_Periph, GPIO_InitTypeDef m_def, bool defv) {
//     AddGpio(Gpio(m_Periph, m_def, defv));
// }

