#include "debug.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "Io/GpioManager.hpp"

namespace {
void PrintBootBanner() {
    printf ("SystemClk:%d\r\n", SystemCoreClock);
    printf ("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
    printf ("FreeRTOS Kernel Version:%s\r\n", tskKERNEL_VERSION_NUMBER);
}
}

int main (void) {
    NVIC_PriorityGroupConfig (NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init (115200);
    PrintBootBanner();

    GpioManager::Init();  // ��ʼ��

    vTaskStartScheduler();
    printf ("[ERROR] scheduler start failed\r\n");
    while (1) {}
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    taskDISABLE_INTERRUPTS(); // �ر��жϣ���ֹ��־�����?
    printf("[FATAL] ջ�����Task: %s\r\n", pcTaskName);
    while(1);
}

