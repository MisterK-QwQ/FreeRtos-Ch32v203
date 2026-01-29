#pragma once
#include "ch32v20x.h"
#include "ch32v20x_gpio.h"
#include "Expands/Expand.hpp"
typedef void (*GpioInitCallback)(void*);

class Gpio {
public:
    GpioInitCallback initCallback = nullptr;
   // Expand expand = {nullptr};
    bool initialized = false;
    void *Periph = nullptr;
    GPIO_InitTypeDef def = {0};

    Gpio() = default;
    Gpio(void *m_Periph, GPIO_InitTypeDef m_def)
        : Periph(m_Periph), def(m_def){};

    // void AddExpand(Expand m_Expand) {
    //     expand = m_Expand;
    // }
    
    /**
     * @brief 初始化 改变Gpio默认电平 并且调用已注册的函数传入类实列
     */
    void init() {
        if (initCallback) initCallback(this);
    }
    /**
     * @brief 注册函数到initCallback
     * 
     * @tparam T
     * @param Func 
     */
    template<typename T>
    void RegisterFunc(void (*Func)(T*)) { 
        initCallback = (GpioInitCallback)Func;
    }
};
