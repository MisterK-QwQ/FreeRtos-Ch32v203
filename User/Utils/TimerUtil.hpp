#pragma once
#include "ch32v20x_conf.h"

class TimerUtil {
  private:
    uint32_t lastMS;
    uint32_t baseMS;
    uint32_t diffStartMS;
    uint32_t windowMS;

    static uint64_t s_totalMs;
    static uint32_t s_prevSysTick;
    static const uint32_t SYSTICK_MAX = 0xFFFFFFFFU;

    static inline uint32_t getSysTickRaw() {
        return (uint32_t)SysTick->CNT;
    }

    static void sysTickInit() {
        static bool s_inited = false;
        if (s_inited)
            return;

        SysTick->CTLR = 0;
        SysTick->SR = 0;
        SysTick->CMP = SYSTICK_MAX;
        SysTick->CNT = SYSTICK_MAX;
        SysTick->CTLR |= (1 << 0) | (1 << 2) | (1 << 3);

        s_prevSysTick = getSysTickRaw();
        s_inited = true;
    }

  public:
    TimerUtil() noexcept {
        uint32_t currMs = getCurrentMs();
        lastMS = currMs;
        baseMS = currMs;
        diffStartMS = currMs;
        windowMS = currMs;
    }

    static uint32_t getCurrentMs() {
        sysTickInit();
        uint32_t currSysTick = getSysTickRaw();
        uint64_t deltaTick = 0;
        if (currSysTick > s_prevSysTick) {
            deltaTick = (uint64_t)s_prevSysTick + (SYSTICK_MAX - currSysTick) + 1;
        } else {
            deltaTick = (uint64_t)s_prevSysTick - currSysTick;
        }

        s_totalMs += deltaTick / (SystemCoreClock / 1000);
        s_prevSysTick = currSysTick;

        return static_cast<uint32_t> (s_totalMs);
    }

    /**
     * 判断指定毫秒数是否已过（基于windowMS）
     * @return true=已过，false=未过
     */
    bool hasTimePassed (uint32_t ms) {
        return (getCurrentMs() - windowMS) >= ms;
    }

    /**
     * 获取当前时间
     * @return 当前毫秒数
     */
    uint32_t getTime() {
        return getCurrentMs();
    }

    /**
     * 重置差值计算起点（原reset2）
     */
    void resetDiffStart() {
        diffStartMS = getTime();
    }

    /**
     * 获取当前与差值起点的时间差
     * @return 时间差（毫秒）
     */
    uint32_t getDifference() {
        return getTime() - diffStartMS;
    }

    /**
     * 设置差值计算起点
     * @param difference 偏移的毫秒数
     */
    void setDifference (uint32_t difference) {
        diffStartMS = getTime() - difference;
    }

    /**
     * 重置时间窗口和lastMS（原resetTime）
     */
    void resetTimeWindow() {
        lastMS = getCurrentMs();
        windowMS = getCurrentMs();
    }

    /**
     * 获取时间窗口已流逝的时间（原getElapsedTime1）
     * @return 流逝毫秒数
     */
    uint32_t getWindowElapsedTime() {
        return getCurrentMs() - windowMS;
    }

    /**
     * 判断指定时长是否已流逝（基于lastMS）
     * @param time 要判断的时长（毫秒）
     * @param reset 是否自动重置（默认false）
     * @return true=已流逝，false=未流逝
     */
    bool hasTimedElapsed (uint32_t time, bool reset = false) {
        uint32_t currMs = getCurrentMs();
        if ((currMs - lastMS) > time) {
            if (reset) {
                resetTimeWindow();
            }
            return true;
        }
        return false;
    }

    /**
     * 重置基础计时起点（原reset1）
     */
    void resetBaseTime() {
        baseMS = getCurrentMs();
    }

    /**
     * 判断基础计时起点后指定毫秒数是否已过
     * @param milliseconds 要判断的时长（毫秒）
     * @return true=已过，false=未过
     */
    bool isBaseTimeElapsed (uint32_t milliseconds) {
        return (getCurrentMs() - baseMS) > milliseconds;
    }

    /**
     * 获取基础计时起点已流逝的时间（原getElapsedTime）
     * @return 流逝毫秒数
     */
    uint32_t getBaseElapsedTime() {
        return getCurrentMs() - baseMS;
    }
};
