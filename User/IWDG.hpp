#pragma once

void IWDG_Init(uint8_t prer, uint16_t rlr){
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(prer);
    IWDG_SetReload(rlr);
    IWDG_ReloadCounter();
    IWDG_Enable();
}
void IWDG_Feed(void){
    IWDG_ReloadCounter();
}
void IWDG_ResetNow(void){
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetReload(0x0000);
    IWDG_ReloadCounter();
    while(1);
}