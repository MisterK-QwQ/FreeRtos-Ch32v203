#pragma once
#include <array>
#include "ch32v20x.h"
#include "Gpio.hpp"

constexpr int MAX_GPIO_PINS = 8;

namespace GpioManager {
    static size_t m_count = 0u;  //实际gpio大小
    static std::array<Gpio *, MAX_GPIO_PINS> m_gpio = {nullptr};
    // static Gpio *m_gpio[MAX_GPIO_PINS];
    auto Init() -> void;
    auto IoClock (GPIO_TypeDef *m_Periph, bool v) -> void;
    auto AddGpio (Gpio *m_Gpio) -> void;

    //  void AddGpio (void *m_Periph, GPIO_InitTypeDef m_def, bool defv = false);

    /**
    * @brief 锟斤拷取锟斤拷实锟斤拷
    * @tparam T 锟斤拷锟斤拷为Gpio锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷挚盏锟街?
    * @param idx Gpio锟斤拷锟?
    */
    template <typename T>
    [[nodiscard]] auto Get (uint16_t idx) -> T * {
        if (idx < m_count && m_gpio[idx] != nullptr) {
            return static_cast<T*>(m_gpio[idx]);
        }
        return nullptr;
     //   return (idx < m_count && m_gpio[idx] != nullptr) ? static_cast<T *> (m_gpio[idx]) : nullptr;
    }

    template <typename T>
    [[nodiscard]] auto Get (GPIO_TypeDef* GpioX,uint16_t pin) -> T * {
        for(size_t i =0;i<m_count;i++){
            if(m_gpio[i]->Periph==GpioX &&m_gpio[i]->def.GPIO_Pin==pin){
                return (T*)m_gpio[i];
            }
        }
        return nullptr;
    }

};  // namespace GpioManager

#define RegTask(Fun, Name, size, pragma, level) \
    TaskHandle_t Task_Handler##Name = nullptr;  \
    ASSERT (xTaskCreate ((TaskFunction_t)Fun,   \
                         (const char *)#Name,   \
                         (uint16_t)size,        \
                         (void *)pragma,        \
                         (UBaseType_t)level,    \
                         (TaskHandle_t *)&Task_Handler##Name) == pdPASS, \
            "task create failed: %s",           \
            #Name);

// 注锟斤拷一锟斤拷Gpio
#define RegGpio(Type)     \
    static Type g_##Type; \
    GpioManager::AddGpio (&g_##Type);

// 注锟斤拷一锟斤拷锟斤拷锟斤拷锟斤拷锟紾pio
#define TRegGpio(Type, size, level, Fun) \
    RegGpio (Type);                      \
    RegTask (Fun, Type, size, &g_##Type, level);

// 注锟斤拷一锟斤拷锟叫讹拷锟斤拷锟斤拷锟斤拷Gpio   锟窖撅拷锟斤拷锟斤拷  锟斤拷使锟斤拷RegTask锟斤拷锟斤拷注锟斤拷
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


// 锟斤拷始锟斤拷一锟斤拷Gpio   
//锟斤拷锟斤拷锟斤拷同一锟叫斤拷锟叫筹拷始锟斤拷
#define InitGpio(Periph, GPIO_Pin, GPIO_Speed, GPIO_Mode, v)                       \
    do {                                                                           \
        GpioManager::IoClock (Periph, true);                                       \
        static GPIO_InitTypeDef def##__LINE__ = {GPIO_Pin, GPIO_Speed, GPIO_Mode}; \
        GPIO_Init ((GPIO_TypeDef *)Periph, &def##__LINE__);                        \
        GPIO_WriteBit ((GPIO_TypeDef *)Periph, GPIO_Pin, v ? Bit_SET : Bit_RESET); \
    } while (0);

