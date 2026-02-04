#pragma once

auto IWDG_Init (uint8_t prer, uint16_t rlr) -> void {
    IWDG_WriteAccessCmd (IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler (prer);
    IWDG_SetReload (rlr);
    IWDG_ReloadCounter();
    IWDG_Enable();
}

auto IWDG_Feed (void) -> void {
    IWDG_ReloadCounter();
}

auto IWDG_ResetNow (void) -> void {
    IWDG_WriteAccessCmd (IWDG_WriteAccess_Enable);
    IWDG_SetReload (0x0000);
    IWDG_ReloadCounter();
    while (1);
}