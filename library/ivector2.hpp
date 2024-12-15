//
// Created by simon on 15/12/2024.
//

#ifndef I_VECTOR_2_HPP
#define I_VECTOR_2_HPP
#include <cmath>
#include <concepts>

template <class T, class U>
concept addable_with = requires(T t, U u)
{
    t + u;
};

template <class T, class U>
concept subtractable_with = requires(T t, U u)
{
    t - u;
};

template <class T, class U>
concept multipliable_with = requires(T t, U u)
{
    t * u;
};

template <class T, class U>
concept dividable_with = requires(T t, U u)
{
    t / u;
};

template <class T, class U>
concept assign_addable_with = requires(T t, U u)
{
    t += u;
};

template <class T, class U>
concept assign_subtractable_with = requires(T t, U u)
{
    t -= u;
};

template <class T, class U>
concept assign_multipliable_with = requires(T t, U u)
{
    t *= u;
};

template <class T, class U>
concept assign_dividable_with = requires(T t, U u)
{
    t /= u;
};

template <class TType>
class IVector2
{
public:
    using value_type = TType;

    TType x, y;

    IVector2() : x(0), y(0)
    {
    }

    IVector2(TType x, TType y) : x(x), y(y)
    {
    }

    template <std::constructible_from<TType> UType>
    explicit IVector2(const IVector2<UType>& other) : x(other.x), y(other.y)
    {
    }

    template <std::convertible_to<TType> UType>
        requires (!std::constructible_from<TType>)
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

    auto operator+=(const IVector2& rhs) -> IVector2&
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    auto operator-=(const IVector2& rhs) -> IVector2&
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    auto operator*=(const IVector2& rhs) -> IVector2&
    {
        x *= rhs.x;
        y *= rhs.y;
        return *this;
    }

    auto operator*=(const TType& rhs) -> IVector2&
    {
        x *= rhs;
        y *= rhs;
        return *this;
    }

    auto operator/=(const IVector2& rhs) -> IVector2&
    {
        x /= rhs.x;
        y /= rhs.y;
        return *this;
    }

    auto operator/=(const TType& rhs) -> IVector2&
    {
        x /= rhs;
        y /= rhs;
        return *this;
    }

    template <class UType>
        requires addable_with<TType, const UType>
    friend auto operator+(IVector2 lhs, const IVector2<UType>& rhs) -> IVector2<decltype(lhs.x + rhs.x)>
    {
        return {lhs.x + rhs.x, lhs.y + rhs.y};
    }

    template <class UType>
        requires subtractable_with<TType, const UType>
    friend auto operator-(IVector2 lhs, const IVector2<UType>& rhs) -> IVector2<decltype(lhs.x - rhs.x)>
    {
        return {lhs.x - rhs.x, lhs.y - rhs.y};
    }

    template <class UType>
        requires multipliable_with<TType, const UType>
    friend auto operator*(IVector2 lhs, const IVector2<UType>& rhs) -> IVector2<decltype(lhs.x * rhs.x)>
    {
        return {lhs.x * rhs.x, lhs.y * rhs.y};
    }

    template <class UType>
        requires (!std::same_as<std::decay_t<UType>, IVector2>) && multipliable_with<TType, const UType>
    friend auto operator*(IVector2 lhs, const UType&& rhs) -> IVector2<decltype(lhs.x * rhs)>
    {
        return {lhs.x * rhs, lhs.y * rhs};
    }

    template <class UType>
        requires dividable_with<TType, UType>
    friend auto operator/(IVector2 lhs, const IVector2<UType>& rhs) -> IVector2<decltype(lhs.x / rhs.x)>
    {
        return {lhs.x / rhs.x, lhs.y / rhs.y};
    }

    template <class UType>
        requires (!std::same_as<std::decay_t<UType>, IVector2>) && dividable_with<TType, const UType>
    friend auto operator/(IVector2 lhs, const UType&& rhs) -> IVector2<decltype(lhs.x / rhs)>
    {
        return {lhs.x / rhs, lhs.y / rhs};
    }

    [[nodiscard]] auto length() const -> float requires std::convertible_to<TType, float>
    {
        return std::sqrt(static_cast<float>(this->x * this->x + this->y * this->y));
    }

    [[nodiscard]] auto normalize() const -> IVector2<float> requires dividable_with<TType, float>
    {
        return *this / length();
    }

    [[nodiscard]] auto dot(const IVector2& rhs) const -> decltype(x * rhs.x + y * rhs.y)
    {
        return x * rhs.x + y * rhs.y;
    }
};

#endif //I_VECTOR_2_HPP
