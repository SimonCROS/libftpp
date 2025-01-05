//
// Created by Simon Cros 15/12/2024.
//

#ifndef IVECTOR2_HPP
#define IVECTOR2_HPP

#include <cmath>
#include <concepts>
#include <string>

#if __llvm__
#include <utility>
#endif

#include "mathematics_concepts.hpp"

template <class TType>
class IVector2
{
public:
    using value_type = TType;

    TType x, y;

    IVector2() requires std::default_initializable<TType> : x{}, y{}
    {
    }

    IVector2(TType x, TType y) requires std::copy_constructible<TType> : x(x), y(y)
    {
    }

    template <class UType>
        requires std::constructible_from<TType, UType>
    explicit IVector2(const IVector2<UType>& other) : x(other.x), y(other.y)
    {
    }

    template <class UType>
        requires (!std::constructible_from<TType, UType>)
    explicit IVector2(const IVector2<UType>& other)
    {
        x = static_cast<TType>(other.x);
        y = static_cast<TType>(other.y);
    }

    template <std::equality_comparable_with<TType> UType>
    auto operator==(const IVector2<UType>& rhs) const -> bool
    {
        return x == rhs.x && y == rhs.y;
    }

    template <std::equality_comparable_with<TType> UType>
    auto operator!=(const IVector2<UType>& rhs) const -> bool
    {
        return !(*this == rhs);
    }

    template <class UType>
        requires assign_addable_with<TType, const UType>
    auto operator+=(const IVector2<UType>& rhs) -> IVector2&
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    template <class UType>
        requires assign_subtractable_with<TType, const UType>
    auto operator-=(const IVector2<UType>& rhs) -> IVector2&
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    template <class UType>
        requires assign_multipliable_with<TType, const UType>
    auto operator*=(const IVector2<UType>& rhs) -> IVector2&
    {
        x *= rhs.x;
        y *= rhs.y;
        return *this;
    }

    template <class UType>
        requires (!std::same_as<std::decay_t<UType>, IVector2>) && assign_multipliable_with<TType, UType>
    auto operator*=(UType&& rhs) -> IVector2&
    {
        x *= std::forward<UType>(rhs);
        y *= std::forward<UType>(rhs);
        return *this;
    }

    template <class UType>
        requires assign_dividable_with<TType, const UType>
    auto operator/=(const IVector2<UType>& rhs) -> IVector2&
    {
        x /= rhs.x;
        y /= rhs.y;
        return *this;
    }

    template <class UType>
        requires (!std::same_as<std::decay_t<UType>, IVector2>) && assign_dividable_with<TType, UType>
    auto operator/=(UType&& rhs) -> IVector2&
    {
        x /= std::forward<UType>(rhs);
        y /= std::forward<UType>(rhs);
        return *this;
    }

    // Bug with member access in trailing return type of locally defined friend functions
    // https://github.com/llvm/llvm-project/issues/63526
#if __llvm__
#ifndef _vec2_vec_operation_result
#define _vec2_vec_operation_result(op) IVector2<decltype(std::declval<TType>() op std::declval<UType>())>
#endif
#ifndef _vec2_scl_operation_result
#define _vec2_scl_operation_result(op) IVector2<decltype(std::declval<TType>() op std::declval<UType>())>
#endif
#else
#ifndef _vec2_vec_operation_result
#define _vec2_vec_operation_result(op) IVector2<decltype(lhs.x op rhs.x)>
#endif
#ifndef _vec2_scl_operation_result
#define _vec2_scl_operation_result(op) IVector2<decltype(lhs.x op rhs)>
#endif
#endif

    template <class UType>
        requires addable_with<TType, const UType>
    friend auto operator+(IVector2 lhs, const IVector2<UType>& rhs) -> _vec2_vec_operation_result(+)
    {
        return {lhs.x + rhs.x, lhs.y + rhs.y};
    }

    template <class UType>
        requires subtractable_with<TType, const UType>
    friend auto operator-(IVector2 lhs, const IVector2<UType>& rhs) -> _vec2_vec_operation_result(-)
    {
        return {lhs.x - rhs.x, lhs.y - rhs.y};
    }

    template <class UType>
        requires multipliable_with<TType, const UType>
    friend auto operator*(IVector2 lhs, const IVector2<UType>& rhs) -> _vec2_vec_operation_result(*)
    {
        return {lhs.x * rhs.x, lhs.y * rhs.y};
    }

    template <class UType>
        requires (!std::same_as<std::decay_t<UType>, IVector2>) && multipliable_with<TType, UType>
    friend auto operator*(IVector2 lhs, UType&& rhs) -> _vec2_scl_operation_result(*)
    {
        return {lhs.x * std::forward<const UType>(rhs), lhs.y * std::forward<const UType>(rhs)};
    }

    template <class UType>
        requires dividable_with<TType, UType>
    friend auto operator/(IVector2 lhs, const IVector2<UType>& rhs) -> _vec2_vec_operation_result(/)
    {
        return {lhs.x / rhs.x, lhs.y / rhs.y};
    }

    template <class UType>
        requires (!std::same_as<std::decay_t<UType>, IVector2>) && dividable_with<TType, UType>
    friend auto operator/(IVector2 lhs, UType&& rhs) -> _vec2_scl_operation_result(/)
    {
        return {lhs.x / std::forward<UType>(rhs), lhs.y / std::forward<UType>(rhs)};
    }

    [[nodiscard]] auto length() const -> float requires std::convertible_to<TType, float>
    {
        return std::sqrt(static_cast<float>(x * x + y * y));
    }

    [[nodiscard]] auto normalize() const -> IVector2<float> requires std::convertible_to<TType, float> &&
        std::constructible_from<float, TType>
    {
        const float len = length();
        if (len > std::numeric_limits<float>::epsilon())
        {
            if constexpr (std::is_same_v<TType, float>)
                return *this / len;
            else
                return IVector2<float>(*this) / len;
        }
        return {};
    }

    [[nodiscard]] auto dot(const IVector2& rhs) const -> decltype(x * rhs.x + y * rhs.y)
    {
        return x * rhs.x + y * rhs.y;
    }

    // cross product of a single 2D vector... requested by the subject
    [[nodiscard]] auto cross() const -> IVector2 requires negatable<TType>
    {
        return {y, -x};
    }
};

#endif //IVECTOR2_HPP
