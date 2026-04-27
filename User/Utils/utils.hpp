#pragma once
#include "ch32v20x_conf.h"
#include "Math.hpp"
#include "TimerUtil.hpp"
#ifndef FORCE_INLINE
    #if defined(__ARMCC_VERSION)
        #define FORCE_INLINE __forceinline
    #elif defined(__GNUC__)
        #define FORCE_INLINE inline __attribute__((always_inline))
    #else
        #define FORCE_INLINE inline
    #endif
#endif


namespace Utils {

/**
 * @brief 移动平均滤波，降低抖动
 */
template <typename T = uint16_t, size_t cache_size = 8>
FORCE_INLINE T adc_filter_moving_average (T new_val) noexcept {
    static T cache[cache_size] = {0};
    static size_t idx = 0;
    static T sum = 0;

    sum -= cache[idx];
    sum += new_val;
    cache[idx] = new_val;
    idx = (idx + 1) % cache_size;
    return sum / cache_size;
}

/**
 * @brief 中值滤波
 */
template <typename T = uint16_t, size_t cache_size = 3>
FORCE_INLINE T adc_filter_median (T new_val) noexcept {
    static_assert (cache_size % 2 == 1, "Median filter cache size must be odd");
    static T cache[cache_size] = {0};
    static size_t idx = 0;

    cache[idx] = new_val;
    idx = (idx + 1) % cache_size;
    T temp[cache_size];
    for (size_t i = 0; i < cache_size; ++i) temp[i] = cache[i];
    for (size_t i = 0; i < cache_size - 1; ++i) {
        for (size_t j = 0; j < cache_size - i - 1; ++j) {
            if (temp[j] > temp[j + 1]) {
                T t = temp[j];
                temp[j] = temp[j + 1];
                temp[j + 1] = t;
            }
        }
    }
    return temp[cache_size / 2];
}

/**
 * @brief 中值 + 均值滤波
 */
template <typename T = uint16_t, size_t median_size = 3, size_t avg_size = 8>
FORCE_INLINE T adc_filter_median_average (T new_val) noexcept {
    T med_val = adc_filter_median<T, median_size> (new_val);
    T filt_val = adc_filter_moving_average<T, avg_size> (med_val);
    return filt_val;
}

/**
 * @brief 指数平滑滤波
 */
template <typename T = uint16_t, size_t alpha_frac = 1, size_t alpha_den = 10>
FORCE_INLINE T adc_filter_exponential (T new_val) noexcept {
    static_assert (alpha_frac < alpha_den, "Alpha must be 0<alpha<1");
    static T last_filt_val = 0;
    T filt_val = (alpha_frac * new_val + (alpha_den - alpha_frac) * last_filt_val) / alpha_den;
    last_filt_val = filt_val;
    return filt_val;
}

template <typename T>
struct LerpValue {
    T start = {};
    T current = {};
    T target = {};
    uint32_t duration_ms = 0;
    uint32_t start_ms = 0;
    bool active = false;
    bool smooth = true;

    LerpValue() = default;
    explicit LerpValue(T initial) : start(initial), current(initial), target(initial) {}

    void reset(T value) {
        start = value;
        current = value;
        target = value;
        duration_ms = 0;
        start_ms = TimerUtil::getCurrentMs();
        active = false;
    }

    void setTarget(T new_target, uint32_t ms, bool use_smoothstep = true) {
        start = current;
        target = new_target;
        duration_ms = ms;
        start_ms = TimerUtil::getCurrentMs();
        smooth = use_smoothstep;
        active = true;

        if (duration_ms == 0 || start == target) {
            current = target;
            active = false;
        }
    }

    T update() {
        if (!active) return current;

        const uint32_t now = TimerUtil::getCurrentMs();
        const uint32_t elapsed = now - start_ms;

        if (elapsed >= duration_ms) {
            current = target;
            active = false;
            return current;
        }

        const float t = static_cast<float>(elapsed) / static_cast<float>(duration_ms);
        current = smooth ? Math::smoothstep<T>(start, target, t) : Math::lerp<T>(start, target, t);
        return current;
    }

    bool finished() const {
        return !active;
    }
};

}  // namespace Utils
