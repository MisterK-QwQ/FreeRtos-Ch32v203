#pragma once

#include <cmath>
#include <type_traits>

#ifdef __embedded__
#define FORCE_INLINE [[gnu::always_inline]] inline
#else
#define FORCE_INLINE inline
#endif

template <size_t N, typename T = float>
struct Vec
{
    static_assert(N >= 2 && N <= 4, "Vector dimension must be 2, 3 or 4");
    
    union
    {
        T data[N];

        struct {
            T x [[maybe_unused]];
            T y [[maybe_unused]];
            T z [[maybe_unused]]; 
            T w [[maybe_unused]]; 
        };
        struct {
            T r [[maybe_unused]];
            T g [[maybe_unused]];
            T b [[maybe_unused]];
            T a [[maybe_unused]];
        };
    };

    FORCE_INLINE Vec() noexcept : data{} {}

    template <typename... Args>
    FORCE_INLINE explicit Vec(Args&&... args) noexcept
    {
        static_assert(sizeof...(args) == N, "Argument count mismatch with vector dimension");
        T* ptr = data;
        ((*ptr++ = static_cast<T>(args)), ...); 
    }

    // 数组构造
    FORCE_INLINE explicit Vec(const T* arr) noexcept
    {
        for (size_t i = 0; i < N; ++i)
            data[i] = arr[i];
    }

    // 下标访问（读写）
    FORCE_INLINE T& operator[](size_t idx) noexcept { return data[idx]; }

    // 下标访问（只读）
    FORCE_INLINE const T& operator[](size_t idx) const noexcept { return data[idx]; }

    // 向量加法
    FORCE_INLINE Vec operator+(const Vec& other) const noexcept
    {
        Vec res;
        for (size_t i = 0; i < N; ++i)
            res.data[i] = data[i] + other.data[i];
        return res;
    }

    // 向量减法
    FORCE_INLINE Vec operator-(const Vec& other) const noexcept
    {
        Vec res;
        for (size_t i = 0; i < N; ++i)
            res.data[i] = data[i] - other.data[i];
        return res;
    }

    // 向量数乘
    FORCE_INLINE Vec operator*(T scalar) const noexcept
    {
        Vec res;
        for (size_t i = 0; i < N; ++i)
            res.data[i] = data[i] * scalar;
        return res;
    }

    // 向量数除
    FORCE_INLINE Vec operator/(T scalar) const noexcept
    {
        Vec res;
        T inv = 1.0f / scalar; 
        for (size_t i = 0; i < N; ++i)
            res.data[i] = data[i] * inv;
        return res;
    }

    // 复合赋值
    FORCE_INLINE Vec& operator+=(const Vec& other) noexcept
    {
        for (size_t i = 0; i < N; ++i)
            data[i] += other.data[i];
        return *this;
    }

    FORCE_INLINE Vec& operator-=(const Vec& other) noexcept
    {
        for (size_t i = 0; i < N; ++i)
            data[i] -= other.data[i];
        return *this;
    }

    FORCE_INLINE Vec& operator*=(T scalar) noexcept
    {
        for (size_t i = 0; i < N; ++i)
            data[i] *= scalar;
        return *this;
    }

    FORCE_INLINE Vec& operator/=(T scalar) noexcept
    {
        T inv = 1.0f / scalar;
        for (size_t i = 0; i < N; ++i)
            data[i] *= inv;
        return *this;
    }

    // 点积（内积）
    FORCE_INLINE T dot(const Vec& other) const noexcept
    {
        T res = 0;
        for (size_t i = 0; i < N; ++i)
            res += data[i] * other.data[i];
        return res;
    }

    // 向量长度（模）
    FORCE_INLINE T length() const noexcept
    {
        return std::sqrt(dot(*this));
    }

    // 向量长度平方
    FORCE_INLINE T length_sq() const noexcept
    {
        return dot(*this);
    }

    // 单位化（归一化）
    FORCE_INLINE Vec normalized() const noexcept
    {
        T len = length();
        if (len < 1e-8f)
            return Vec{};
        return *this / len;
    }

    FORCE_INLINE Vec operator-() const noexcept
    {
        Vec res;
        for (size_t i = 0; i < N; ++i)
            res.data[i] = -data[i];
        return res;
    }

    // 补充：元素级乘法（Hadamard积）
    FORCE_INLINE Vec operator*(const Vec& other) const noexcept
    {
        Vec res;
        for (size_t i = 0; i < N; ++i)
            res.data[i] = data[i] * other.data[i];
        return res;
    }

    // 补充：元素级除法
    FORCE_INLINE Vec operator/(const Vec& other) const noexcept
    {
        Vec res;
        for (size_t i = 0; i < N; ++i)
            res.data[i] = data[i] / other.data[i];
        return res;
    }
};

using Vec2 = Vec<2, float>;
using Vec3 = Vec<3, float>;
using Vec4 = Vec<4, float>;
using Vec2i = Vec<2, int>;
using Vec3i = Vec<3, int>;
using Vec4i = Vec<4, int>;

FORCE_INLINE Vec3 cross(const Vec3& a, const Vec3& b) noexcept
{
    return Vec3{
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

template <size_t N, typename T>
FORCE_INLINE Vec<N, T> operator*(T scalar, const Vec<N, T>& vec) noexcept
{
    return vec * scalar;
}

template <size_t N, typename T>
FORCE_INLINE bool operator==(const Vec<N, T>& a, const Vec<N, T>& b) noexcept
{
    if constexpr (std::is_floating_point_v<T>) {
        constexpr T epsilon = 1e-6f;
        for (size_t i = 0; i < N; ++i) {
            if (std::abs(a[i] - b[i]) > epsilon)
                return false;
        }
    } else {
        for (size_t i = 0; i < N; ++i) {
            if (a[i] != b[i])
                return false;
        }
    }
    return true;
}

template <size_t N, typename T>
FORCE_INLINE bool operator!=(const Vec<N, T>& a, const Vec<N, T>& b) noexcept
{
    return !(a == b);
}