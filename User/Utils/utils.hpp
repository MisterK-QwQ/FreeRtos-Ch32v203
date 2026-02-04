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
 * @brief 移动平均滤波（滑动平均）
 * @details 基于固定长度缓存的滑动窗口，计算平均值，平滑随机噪声，适合缓慢变化的ADC数据
 * @tparam T 数据类型，默认uint16_t（适配12位ADC）
 * @tparam cache_size 缓存长度，默认8（建议设为2的幂次，除法可优化为移位）
 * @param new_val 本次ADC采样的原始值
 * @return T 滤波后的平均值
 * @note 缓存长度越大，平滑效果越好，但动态响应越慢；缓存为静态变量，首次调用初始化为0
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
 * @details 基于奇数长度缓存，排序后取中间值，有效滤除尖峰脉冲干扰（如电磁干扰导致的ADC跳变）
 * @tparam T 数据类型，默认uint16_t（适配12位ADC）
 * @tparam cache_size 缓存长度，默认3（必须为奇数，否则编译报错）
 * @param new_val 本次ADC采样的原始值
 * @return T 滤波后的中值
 * @note 缓存长度越大，抗尖峰能力越强，但计算量略增；采用冒泡排序，适合小缓存场景
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
 * @brief 中值+移动平均复合滤波
 * @details 先通过中值滤波滤除尖峰干扰，再通过移动平均滤波平滑随机噪声，兼顾抗干扰和平滑性
 * @tparam T 数据类型，默认uint16_t（适配12位ADC）
 * @tparam median_size 中值滤波缓存长度，默认3（必须为奇数）
 * @tparam avg_size 移动平均滤波缓存长度，默认8（建议设为2的幂次）
 * @param new_val 本次ADC采样的原始值
 * @return T 复合滤波后的结果
 * @note 适合既有尖峰干扰又有随机噪声的ADC采样场景（如触摸按键、模拟传感器）
 */
template <typename T = uint16_t, size_t median_size = 3, size_t avg_size = 8>
FORCE_INLINE T adc_filter_median_average (T new_val) noexcept {
    // 先中值滤波去尖峰，再移动平均滤波去随机噪声
    T med_val = adc_filter_median<T, median_size> (new_val);
    T filt_val = adc_filter_moving_average<T, avg_size> (med_val);
    return filt_val;
}

/**
 * @brief 指数平滑滤波
 * @details 基于历史值和当前值的加权平均，动态响应快于移动平均，适合快速变化的ADC数据
 * @tparam T 数据类型，默认uint16_t（适配12位ADC）
 * @tparam alpha_frac 平滑系数分子，默认1（alpha = alpha_frac / alpha_den）
 * @tparam alpha_den 平滑系数分母，默认10（alpha建议范围0.1~0.3）
 * @param new_val 本次ADC采样的原始值
 * @return T 滤波后的结果
 * @note 采用定点数运算，避免浮点开销；alpha越大，跟踪速度越快，平滑效果越弱；
 *       首次调用时历史值初始化为0，前几次输出会有过渡过程
 */
template <typename T = uint16_t, size_t alpha_frac = 1, size_t alpha_den = 10>
FORCE_INLINE T adc_filter_exponential (T new_val) noexcept {
    static_assert (alpha_frac < alpha_den, "Alpha must be 0<alpha<1");
    static T last_filt_val = 0;
    T filt_val = (alpha_frac * new_val + (alpha_den - alpha_frac) * last_filt_val) / alpha_den;
    last_filt_val = filt_val;
    return filt_val;
}

}  // namespace Utils
