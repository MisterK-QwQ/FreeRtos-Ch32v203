#include "debug.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "Io/GpioManager.hpp"

auto main (void) -> int {
    NVIC_PriorityGroupConfig (NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init (115200);
    printf ("SystemClk:%d\r\n", SystemCoreClock);
    printf ("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
    printf ("FreeRTOS Kernel Version:%s\r\n", tskKERNEL_VERSION_NUMBER);

    GpioManager::Init();  // ≥ı ºªØ

    vTaskStartScheduler();
    while (1) { printf ("[ERROR] shouldn't run at here!!\r\n"); }
}
