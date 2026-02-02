#pragma once
#include <cmath>
#include <type_traits>
#include "Vector.hpp"

#ifdef __embedded__
#define FORCE_INLINE [[gnu::always_inline]] inline
#else
#define FORCE_INLINE inline
#endif

namespace Math {
template <typename T = float>
FORCE_INLINE constexpr T PI() noexcept { return static_cast<T> (3.14159265358979323846); }

// 半PI、两倍PI
template <typename T = float>
FORCE_INLINE constexpr T HALF_PI() noexcept { return PI<T>() * static_cast<T> (0.5); }

template <typename T = float>
FORCE_INLINE constexpr T TWO_PI() noexcept { return PI<T>() * static_cast<T> (2.0); }

// 弧度/角度转换系数
template <typename T = float>
FORCE_INLINE constexpr T RAD2DEG() noexcept { return static_cast<T> (180.0) / PI<T>(); }

template <typename T = float>
FORCE_INLINE constexpr T DEG2RAD() noexcept { return PI<T>() / static_cast<T> (180.0); }

// 浮点精度阈值（避免浮点比较的精度问题）
template <typename T = float>
FORCE_INLINE constexpr T EPSILON() noexcept { return static_cast<T> (1e-6); }

// 绝对值（适配所有数值类型）
template <typename T>
FORCE_INLINE T abs (const T &val) noexcept {
    return val < static_cast<T> (0) ? -val : val;
}

// 符号函数：返回-1/0/1（适配所有数值类型）
template <typename T>
FORCE_INLINE int sign (const T &val) noexcept {
    if (val > static_cast<T> (0))
        return 1;
    if (val < static_cast<T> (0))
        return -1;
    return 0;
}

// 钳位函数：将值限制在[min, max]范围内
template <typename T>
FORCE_INLINE T clamp (const T &val, const T &min_val, const T &max_val) noexcept {
    if (val < min_val)
        return min_val;
    if (val > max_val)
        return max_val;
    return val;
}

// 数值映射：将[in_min, in_max]的val映射到[out_min, out_max]
template <typename T>
FORCE_INLINE T map (const T &val, const T &in_min, const T &in_max, const T &out_min, const T &out_max) noexcept {
    return (val - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// 线性插值：lerp(a, b, t)，t∈[0,1]返回a到b的插值
template <typename T>
FORCE_INLINE T lerp (const T &a, const T &b, const float &t) noexcept {
    const float t_clamped = clamp (t, 0.0f, 1.0f);
    if constexpr (std::is_integral_v<T>) {
        return static_cast<T> (a + (b - a) * t_clamped + 0.5f);
    } else {
        return static_cast<T> (a + (b - a) * t_clamped);
    }
}

// 平滑插值：smoothstep(a, b, t)，比lerp更自然的S型插值（动画/过渡效果）
template <typename T>
FORCE_INLINE T smoothstep (const T &a, const T &b, const float &t) noexcept {
    const float t_clamped = clamp (t, 0.0f, 1.0f);
    const float t_smooth = t_clamped * t_clamped * (3.0f - 2.0f * t_clamped);
    return lerp (a, b, t_smooth);
}

template <typename T>
FORCE_INLINE T round(const T& val) noexcept {
    static_assert(std::is_floating_point_v<T>);
    return val > 0 ? static_cast<T>(static_cast<long long>(val + 0.5)) : static_cast<T>(static_cast<long long>(val - 0.5));
}

template <typename T>
FORCE_INLINE T floor (const T &val) noexcept {
    static_assert (std::is_floating_point_v<T>);
    return std::floor (val);
}

template <typename T>
FORCE_INLINE T ceil (const T &val) noexcept {
    static_assert (std::is_floating_point_v<T>);
    return std::ceil (val);
}

template <typename T>
FORCE_INLINE T trunc(const T& val) noexcept {
    static_assert(std::is_floating_point_v<T>);
    return static_cast<T>(static_cast<long long>(val));
}

// 标准三角函数（封装std::sin/cos/tan，适配弧度，有FPU时用，精度高）
template <typename T = float>
FORCE_INLINE T sin (const T &rad) noexcept {
    static_assert (std::is_floating_point_v<T>);
    return std::sin (rad);
}

template <typename T = float>
FORCE_INLINE T cos (const T &rad) noexcept {
    static_assert (std::is_floating_point_v<T>);
    return std::cos (rad);
}

template <typename T = float>
FORCE_INLINE T tan (const T &rad) noexcept {
    static_assert (std::is_floating_point_v<T>);
    return std::tan (rad);
}

// 嵌入式无FPU快速三角函数（泰勒展开版，速度快/精度足够，适配弧度，建议无FPU时使用）
template <typename T = float>
FORCE_INLINE T fast_sin (const T &rad) noexcept {
    static_assert (std::is_floating_point_v<T>);
    T x = clamp (rad, -PI<T>(), PI<T>());
    T x2 = x * x;
    T x3 = x2 * x;
    T x5 = x3 * x2;
    T x7 = x5 * x2;
    return x - x3 / 6.0f + x5 / 120.0f - x7 / 5040.0f;
}

template <typename T = float>
FORCE_INLINE T fast_cos (const T &rad) noexcept {
    static_assert (std::is_floating_point_v<T>);
    return fast_sin (rad + HALF_PI<T>());
}

// 两点间距离（返回实际距离，开根号）
template <size_t N, typename T>
FORCE_INLINE T distance (const Vec<N, T> &a, const Vec<N, T> &b) noexcept {
    return (a - b).length();
}

// 两点间距离平方
template <size_t N, typename T>
FORCE_INLINE T distance_sq (const Vec<N, T> &a, const Vec<N, T> &b) noexcept {
    return (a - b).length_sq();
}

// 向量投影：将v投影到target向量上（返回投影后的向量）
template <size_t N, typename T>
FORCE_INLINE Vec<N, T> project (const Vec<N, T> &v, const Vec<N, T> &target) noexcept {
    T t_len_sq = target.length_sq();
    if (t_len_sq < EPSILON<T>())
        return Vec<N, T>{};
    return target * (v.dot (target) / t_len_sq);
}

// 向量反射：根据法向量normal反射向量v（法向量需归一化，返回反射向量）
template <size_t N, typename T>
FORCE_INLINE Vec<N, T> reflect (const Vec<N, T> &v, const Vec<N, T> &normal) noexcept {
    return v - normal * static_cast<T> (2) * v.dot (normal);
}

// 向量夹角（返回弧度值，可选转换为角度）
template <size_t N, typename T>
FORCE_INLINE T angle_between (const Vec<N, T> &a, const Vec<N, T> &b) noexcept {
    static_assert (std::is_floating_point_v<T>);
    T dot_val = a.dot (b);
    T a_len = a.length();
    T b_len = b.length();
    if (a_len < EPSILON<T>() || b_len < EPSILON<T>())
        return static_cast<T> (0);
    // 钳位到[-1,1]，避免浮点精度问题导致acos返回NaN
    T cos_theta = clamp (dot_val / (a_len * b_len), static_cast<T> (-1), static_cast<T> (1));
    return std::acos (cos_theta);
}

// 向量线性插值：lerp(a, b, t)，t∈[0,1]
template <size_t N, typename T>
FORCE_INLINE Vec<N, T> lerp (const Vec<N, T> &a, const Vec<N, T> &b, const float &t) noexcept {
    Vec<N, T> res;
    for (size_t i = 0; i < N; ++i)
        res[i] = lerp (a[i], b[i], t);
    return res;
}

// 2x2矩阵（行优先，嵌入式常用，与Vec2配套）
 template <typename T = float>
    struct Mat2
    {
        union
        {
            T data[2][2];
            struct { T m00, m01, m10, m11; };
        };

        FORCE_INLINE Mat2() noexcept : data{} {}
        FORCE_INLINE Mat2(T m00, T m01, T m10, T m11) noexcept : m00(m00), m01(m01), m10(m10), m11(m11) {}
        FORCE_INLINE explicit Mat2(const T* arr) noexcept { for (int i=0; i<2; ++i) for (int j=0; j<2; ++j) data[i][j] = arr[i*2+j]; }

        FORCE_INLINE static Mat2 identity() noexcept { return Mat2(1,0, 0,1); }
        FORCE_INLINE static Mat2 rotate(const T& rad) noexcept
        {
            T c = cos(rad);
            T s = sin(rad);
            return Mat2(c, -s, s, c);
        }
        FORCE_INLINE static Mat2 scale(const T& x, const T& y) noexcept { return Mat2(x,0, 0,y); }

        // 直接使用Vec2（已由Vector.hpp定义，无编译错误）
        FORCE_INLINE Vec<2, T> operator*(const Vec<2, T>& v) const noexcept
        {
            return Vec<2, T>( // 这里同步改
                m00*v.x + m01*v.y,
                m10*v.x + m11*v.y
            );
        }
        FORCE_INLINE Mat2 operator*(const Mat2& other) const noexcept
        {
            return Mat2(
                m00*other.m00 + m01*other.m10, m00*other.m01 + m01*other.m11,
                m10*other.m00 + m11*other.m10, m10*other.m01 + m11*other.m11
            );
        }
    };

// 3x3矩阵（行优先，与Vec3配套，3D基础变换）
template <typename T = float>
struct Mat3 {
    T data[3][3] = {};

    // 单位矩阵
    FORCE_INLINE static Mat3 identity() noexcept {
        Mat3 m;
        for (int i = 0; i < 3; ++i) m.data[i][i] = 1;
        return m;
    }

    // 矩阵乘向量（3x3*Vec3）
FORCE_INLINE Vec<3, T> operator* (const Vec<3, T> &v) const noexcept {
    return Vec<3, T> ( // 这里同步改
        data[0][0]*v.x + data[0][1]*v.y + data[0][2]*v.z,
        data[1][0]*v.x + data[1][1]*v.y + data[1][2]*v.z,
        data[2][0]*v.x + data[2][1]*v.y + data[2][2]*v.z
    );
}
};

// 4x4矩阵
template <typename T = float>
struct Mat4 {
    T data[4][4] = {};

    // 单位矩阵
    FORCE_INLINE static Mat4 identity() noexcept {
        Mat4 m;
        for (int i = 0; i < 4; ++i) m.data[i][i] = 1;
        return m;
    }

    // 矩阵乘向量（4x4*Vec4）
FORCE_INLINE Vec<4, T> operator* (const Vec<4, T> &v) const noexcept {
    Vec<4, T> res; // 这里同步改
    for (int i=0; i<4; ++i)
        res[i] = data[i][0]*v[0] + data[i][1]*v[1] + data[i][2]*v[2] + data[i][3]*v[3];
    return res;
}
};

// 矩阵类型别名（与向量库保持一致）
using Mat2f = Mat2<float>;
using Mat3f = Mat3<float>;
using Mat4f = Mat4<float>;

// 点是否在矩形内（Vec2为点，rect_min/rect_max为矩形对角点）
template <typename T>
FORCE_INLINE bool point_in_rect(const Vec<2, T>& p, const Vec<2, T>& rect_min, const Vec<2, T>& rect_max) noexcept
{
    return p.x >= rect_min.x && p.x <= rect_max.x && p.y >= rect_min.y && p.y <= rect_max.y;
}

// 点是否在球体/圆内（center为球心，radius为半径，返回是否在内部）
template <size_t N, typename T>
FORCE_INLINE bool point_in_sphere (const Vec<N, T> &p, const Vec<N, T> &center, const T &radius) noexcept {
    return distance_sq (p, center) <= radius * radius;
}
}  // namespace Math

// 矩阵*标量 运算符重载（与向量库保持一致）
template <typename T>
FORCE_INLINE Math::Mat2<T> operator* (T scalar, const Math::Mat2<T> &mat) noexcept {
    Math::Mat2<T> res;
    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 2; ++j) res.data[i][j] = mat.data[i][j] * scalar;
    return res;
}

template <typename T>
FORCE_INLINE Math::Mat2<T> operator* (const Math::Mat2<T> &mat, T scalar) noexcept { return scalar * mat; }
