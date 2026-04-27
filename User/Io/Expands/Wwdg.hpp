#pragma once
#include "../Gpio.hpp"
#include "Expand.hpp"
#include "ch32v20x_wwdg.h"

#define WWDG_CNT 0X7F

namespace {
constexpr TickType_t kWwdgFeedPeriod = pdMS_TO_TICKS(5);
constexpr uint8_t kWwdgWindowValue = 0x5F;
}

class Wwdg : public Gpio {
  public:
    uint8_t wwdg_wr = 0x5F;
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    Wwdg() : Gpio() {
        NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        RegisterFunc (oninit);
    }

    Wwdg (const Wwdg &) = delete;
    Wwdg &operator= (const Wwdg &) = delete;

    static void oninit (Wwdg *_this) {
        RCC_APB1PeriphClockCmd (RCC_APB1Periph_WWDG, ENABLE);
        WWDG_SetCounter (WWDG_CNT);
        WWDG_SetPrescaler (WWDG_Prescaler_8);
        WWDG_SetWindowValue (kWwdgWindowValue);
        WWDG_Enable (WWDG_CNT);
        WWDG_ClearFlag();
        NVIC_Init (&_this->NVIC_InitStructure);
        WWDG_EnableIT();
        _this->wwdg_wr = WWDG->CFGR & 0x7F;
    }

    static void Task (Wwdg *_this) {
        TickType_t lastWakeTime = xTaskGetTickCount();
        while (true) {
            uint8_t wwdg_tr = WWDG->CTLR & 0x7F;
              if (wwdg_tr < _this->wwdg_wr && wwdg_tr > 0x40) {
                WWDG_SetCounter (WWDG_CNT);
            }
            vTaskDelayUntil (&lastWakeTime, kWwdgFeedPeriod);
        }
    }
};

extern "C" void WWDG_IRQHandler(void) {
    if(WWDG_GetFlagStatus() != RESET) {
        WWDG_SetCounter(WWDG_CNT);
        WWDG_ClearFlag();
    }
}

