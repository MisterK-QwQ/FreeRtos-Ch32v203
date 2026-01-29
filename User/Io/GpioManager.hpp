#pragma once
#include "ch32v20x.h"

#include "Gpio.hpp"
#define MAX_GPIO_PINS 4

namespace GpioManager {
static int m_count = 0u;
static Gpio *m_gpio[MAX_GPIO_PINS];

void Init();
void IoClock (void *m_Periph, bool v);

void AddGpio (Gpio *m_Gpio);

//  void AddGpio (void *m_Periph, GPIO_InitTypeDef m_def, bool defv = false);

/**
 * @brief 获取类实列
 *
 * @tparam T 必须为Gpio的子类否则会出现空地址
 * @param idx Gpio序号
 */
template <typename T>
T *Get (uint16_t idx) {
    return (idx < m_count && m_gpio[idx] != nullptr) ? static_cast<T *> (m_gpio[idx]) : nullptr;
}
};  // namespace GpioManager

// 注册一个任务
#define RegTask(Fun, Name, size, pragma, level) \
    TaskHandle_t Task_Handler##Name = nullptr;  \
    xTaskCreate ((TaskFunction_t)Fun,           \
                 (const char *)#Name,           \
                 (uint16_t)size,                \
                 (void *)pragma,                \
                 (UBaseType_t)level,            \
                 (TaskHandle_t *)&Task_Handler##Name);
// 注册一个Gpio
#define RegGpio(Type)     \
    static Type g_##Type; \
    GpioManager::AddGpio (&g_##Type);

// 注册一个有任务的Gpio
#define TRegGpio(Type, size, level) \
    RegGpio (Type);                 \
    RegTask (g_##Type.Task, Type, size, &g_##Type, level);

// 注册一个有多个任务的Gpio
#define M_TRegGpio(Type, size, level, Num)                        \
    RegGpio (Type);                                               \
    for (uint8_t i = 1u; i < Num; i++) {                          \
        RegTask (g_##Type.Task##i, Type, size, &g_##Type, level); \
    }

//初始化一个Gpio
#define InitGpio(Periph, GPIO_Pin, GPIO_Speed, GPIO_Mode, v)                   \
    GpioManager::IoClock (Periph, true);                                       \
    static GPIO_InitTypeDef def##__LINE__ = {GPIO_Pin, GPIO_Speed, GPIO_Mode}; \
    GPIO_Init ((GPIO_TypeDef *)Periph, &def##__LINE__);                        \
    GPIO_WriteBit ((GPIO_TypeDef *)Periph, GPIO_Pin, v ? Bit_SET : Bit_RESET);
