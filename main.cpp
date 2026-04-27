#include "debug.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "Io/GpioManager.hpp"



int main (void) {
    NVIC_PriorityGroupConfig (NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init (115200);
    
    printf ("SystemClk:%d\r\n", SystemCoreClock);
    printf ("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
    printf ("FreeRTOS Kernel Version:%s\r\n", tskKERNEL_VERSION_NUMBER);

    GpioManager::Init(); //初始化功能

    vTaskStartScheduler();
    printf ("[ERROR] scheduler start failed\r\n");
    while (1) {}
}

extern "C" void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    printf("Fatal Error: Stack Overflow in task: %s\r\n", pcTaskName);
    __disable_irq();
    while(1) {
    }
}

extern "C" void vApplicationMallocFailedHook(void) {
    printf("Fatal Error: FreeRTOS Malloc Failed!\r\n");
    __disable_irq(); 
    while(1) {
    }
}

