#pragma once
#include "Expands/Expand.hpp"
typedef void (*GpioInitCallback)(void*);

class Gpio {
public:
    GpioInitCallback initCallback = nullptr;
   // Expand expand = {nullptr};
    bool initialized = false;
    bool Defaultv = false;
    void *Periph = nullptr;
    GPIO_InitTypeDef def = {0};

    Gpio() = default;
    Gpio(void *m_Periph, GPIO_InitTypeDef m_def, bool v = false)
        : Periph(m_Periph), def(m_def), Defaultv(v){};

    // void AddExpand(Expand m_Expand) {
    //     expand = m_Expand;
    // }
    
    /**
     * @brief 初始化 改变Gpio默认电平 并且调用已注册的函数传入类实列
     */
    void init() {
        if (def.GPIO_Mode != GPIO_Mode_AIN) {
            GPIO_WriteBit((GPIO_TypeDef *)Periph, def.GPIO_Pin, Defaultv ? Bit_SET : Bit_RESET);
        }
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
