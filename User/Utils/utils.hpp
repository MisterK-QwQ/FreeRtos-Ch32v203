#pragma once
#include "ch32v20x_conf.h"
#include "Math.hpp"
#include "TimerUtil.hpp"

namespace Utils {
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

// 移动平均+中值复合滤波
// 模板参数：median_size(中值缓存，奇数)、avg_size(平均缓存，2^n)
template <typename T = uint16_t, size_t median_size = 3, size_t avg_size = 8>
FORCE_INLINE T adc_filter_median_average (T new_val) noexcept {
    // 先中值滤波去尖峰，再移动平均滤波去随机噪声
    T med_val = adc_filter_median<T, median_size> (new_val);
    T filt_val = adc_filter_moving_average<T, avg_size> (med_val);
    return filt_val;
}

// 指数平滑滤波（动态响应快，适合变化的模拟量，alpha为平滑系数(0<alpha<1)，建议0.1~0.3）
// alpha用定点数表示：alpha=0.1 → alpha_frac=1, alpha_den=10，避免浮点运算
template <typename T = uint16_t, size_t alpha_frac = 1, size_t alpha_den = 10>
FORCE_INLINE T adc_filter_exponential (T new_val) noexcept {
    static_assert (alpha_frac < alpha_den, "Alpha must be 0<alpha<1");
    static T last_filt_val = 0;
    T filt_val = (alpha_frac * new_val + (alpha_den - alpha_frac) * last_filt_val) / alpha_den;
    last_filt_val = filt_val;
    return filt_val;
}
}  // namespace Utils
