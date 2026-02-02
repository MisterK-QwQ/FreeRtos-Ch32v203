#pragma once
#include <array>
#include "ch32v20x.h"

#include "Gpio.hpp"
#define MAX_GPIO_PINS 4

namespace GpioManager {
static int m_count = 0u;
static std::array<Gpio*,MAX_GPIO_PINS> m_gpio={nullptr};
//static Gpio *m_gpio[MAX_GPIO_PINS];

auto Init() -> void;
auto IoClock (void *m_Periph, bool v) -> void;

auto AddGpio (Gpio *m_Gpio) -> void;

//  void AddGpio (void *m_Periph, GPIO_InitTypeDef m_def, bool defv = false);

/**
 * @brief 获取类实列
 *
 * @tparam T 必须为Gpio的子类否则会出现空地址
 * @param idx Gpio序号
 */
template <typename T>
[[nodiscard]] auto Get (uint16_t idx) -> T * {
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
#define TRegGpio(Type, size, level, Fun) \
    RegGpio (Type);                      \
    RegTask (Fun, Type, size, &g_##Type, level);

// 注册一个有多个任务的Gpio   已经废弃  请使用RegTask单独注册
// #define M_TRegGpio(Type, size, level, ...)                                   \
//     do {                                                                     \
//         using TaskFunc = void (*) (Type *);                                  \
//         RegGpio (Type);                                                      \
//         const TaskFunc _task_list[] = {__VA_ARGS__};                         \
//         const uint8_t _task_cnt = sizeof (_task_list) / sizeof (TaskFunc);   \
//         for (uint8_t i = 0u; i < _task_cnt; i++) {                           \
//             RegTask (_task_list[i], Type##_Task##i, size, &g_##Type, level); \
//         }                                                                    \
//     } while (0)

// 初始化一个Gpio
#define InitGpio(Periph, GPIO_Pin, GPIO_Speed, GPIO_Mode, v)                       \
    do {                                                                           \
        GpioManager::IoClock (Periph, true);                                       \
        static GPIO_InitTypeDef def##__LINE__ = {GPIO_Pin, GPIO_Speed, GPIO_Mode}; \
        GPIO_Init ((GPIO_TypeDef *)Periph, &def##__LINE__);                        \
        GPIO_WriteBit ((GPIO_TypeDef *)Periph, GPIO_Pin, v ? Bit_SET : Bit_RESET); \
    } while (0);
