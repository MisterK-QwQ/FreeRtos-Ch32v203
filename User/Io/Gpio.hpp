#pragma once
#include "Expands/Expand.hpp"

typedef void (*InitCallback)(void*);

#define MAX_GPIO_PINS 37

class Gpio {
  public:
    union {
        Expand expand = {nullptr, ExpandType::None};
        InitCallback initCallback;
    };

    bool Defaultv = false;
    bool initialized = false;
    void *Periph = nullptr;
    GPIO_InitTypeDef def = {0};
    uint32_t rccPeriph=0;
    Gpio() = default;

    Gpio (void *m_Periph, GPIO_InitTypeDef m_def, bool v = false)
        : Periph (m_Periph), def (m_def), Defaultv (v){};

    void AddExpand (Expand m_Expand) {
        expand = m_Expand;
    }

    /**
     * @brief
     * @param Func 注册初始化函数地址
     */
    void RegisterFunc (void *Func) {
        if (Func == nullptr) {
            printf ("注册的函数地址为空！\n");
            return;
        }
        this->initCallback = (InitCallback)Func;
    }

    void init() {
        GPIO_WriteBit((GPIO_TypeDef*)Periph,def.GPIO_Pin,Defaultv?Bit_SET:Bit_RESET);
        if (initCallback)initCallback((void*)this);
    }
};