#pragma once
#include "Gpio.hpp"
#include "TouchKey.hpp"
#include "R_Upwheel.hpp"
#include "LED.hpp"

#include <task.h>
#include <semphr.h>
#include <FreeRTOS.h>


#define RegTask(Fun, Name, size, pragma, level) \
    TaskHandle_t Task_Handler##Name = nullptr; \
    xTaskCreate((TaskFunction_t)Fun, \
                (const char *)#Name, \
                (uint16_t)size, \
                (void *)pragma, \
                (UBaseType_t)level, \
                (TaskHandle_t *)&Task_Handler##Name);

#define TRegGpio(Type, size, level)\
static Type g_##Type;\
GpioManager::AddGpio(g_##Type);\
RegTask(g_##Type.Task, Type, size, &g_##Type, level);\


namespace GpioManager {

void IoClock(void *m_Periph, bool v) {
    uint32_t RCC_APBPeriph = 0;
    if (m_Periph == GPIOA) RCC_APBPeriph = RCC_APB2Periph_GPIOA;
    else if (m_Periph == GPIOB) RCC_APBPeriph = RCC_APB2Periph_GPIOB;
    else if (m_Periph == GPIOC) RCC_APBPeriph = RCC_APB2Periph_GPIOC;
    else if (m_Periph == GPIOD) RCC_APBPeriph = RCC_APB2Periph_GPIOD;
    else if (m_Periph == AFIO) RCC_APBPeriph = RCC_APB2Periph_AFIO;

    if (RCC_APBPeriph) RCC_APB2PeriphClockCmd(RCC_APBPeriph, v ? ENABLE : DISABLE);
}


#define MAX_GPIO_PINS 4
static int m_count = 0u;
static Gpio m_gpio[MAX_GPIO_PINS];
/**
 * @brief 注册Gpio到m_Gpio
 * 
 * @param m_Gpio 
 */
static void AddGpio(const Gpio& m_Gpio) {
    if (m_count < MAX_GPIO_PINS) {
        m_gpio[m_count++] = m_Gpio;
    }
}
static void AddGpio(void *m_Periph, GPIO_InitTypeDef m_def, bool defv = false) {
    if (m_count < MAX_GPIO_PINS) {
        m_gpio[m_count++] = Gpio(m_Periph, m_def, defv);
    }
}

/**
 * @brief 获取类实列
 *
 * @tparam T 必须为Gpio的子类否则会出现空地址
 * @param idx Gpio序号
 */
template <typename T>
static T* Get(uint16_t idx) {
    return (idx < m_count) ? (T*)&m_gpio[idx] : nullptr;
}

static void Init() {
    AddGpio(GPIOB, {GPIO_Pin_1, GPIO_Speed_50MHz, GPIO_Mode_Out_PP}, true);  //临时变量注册进行初始化
    TRegGpio(TouchKey,128,5);   //触摸案列
  //  static TouchKey g_touchKey;
  //  AddGpio(TouchKey());
    //Regask(g_touchKey.TouchKey_task, tack, 128, &g_touchKey, 5);
    // xTaskCreate ((TaskFunction_t)g_touchKey.TouchKey_task,
    //                 (const char *)"tk",
    //                 (uint16_t)128,
    //                 (void *)&g_touchKey,
    //                 (UBaseType_t)5,
    //                 (TaskHandle_t *)&g_touchKey.AdcData.Task_Handler);
    TRegGpio(LED,128,5);  //闪烁案列
    // static LED g_led;
    // AddGpio(g_led);
    // xTaskCreate((TaskFunction_t)g_led.LED_task,
    //                 (const char *)"LEDTask",
    //                 (uint16_t)128,
    //                 (void *)&g_led,
    //                 (UBaseType_t)5,
    //                 (TaskHandle_t *)&g_led.Data.Task_Handler);

    for (int u = 0u; u < m_count; u++) {
        if (m_gpio[u].initialized) continue;
        IoClock(m_gpio[u].Periph, true);
        GPIO_Init((GPIO_TypeDef *)m_gpio[u].Periph, &m_gpio[u].def);
        m_gpio[u].init();
        m_gpio[u].initialized = true;
    }
}
};  // namespace GpioManager

