//
// Created by simon on 15/12/2024.
//

#ifndef IVECTOR3_HPP
#define IVECTOR3_HPP

#include <cmath>
#include <concepts>

#include "mathematics_concepts.hpp"

template <class TType>
class IVector3 {
public:
    using value_type = TType;

    TType x, y, z;

    IVector3() requires std::default_initializable<TType> : x{}, y{}, z{}
    {
    }

    IVector3(TType x, TType y, TType z) requires std::copy_constructible<TType> : x(x), y(y), z(z)
    {
    }

    template <std::constructible_from<TType> UType>
    explicit IVector3(const IVector3<UType>& other) : x(other.x), y(other.y), z(other.z)
    {
    }

    template <std::convertible_to<TType> UType>
        requires (!std::constructible_from<TType>)
    explicit IVector3(const IVector3<UType>& other)
    {
        x = static_cast<TType>(other.x);
        y = static_cast<TType>(other.y);
        z = static_cast<TType>(other.z);
    }

    template <std::equality_comparable_with<TType> UType>
    auto operator==(const IVector3<UType>& rhs) const -> bool
    {
        return x == rhs.x && y == rhs.y && z == rhs.z;
    }

    template <std::equality_comparable_with<TType> UType>
    auto operator!=(const IVector3<UType>& rhs) const -> bool
    {
        return !(*this == rhs);
    }

    template <class UType>
        requires assign_addable_with<TType, const UType>
    auto operator+=(const IVector3<UType>& rhs) -> IVector3&
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }

    template <class UType>
        requires assign_subtractable_with<TType, const UType>
    auto operator-=(const IVector3<UType>& rhs) -> IVector3&
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }

    template <class UType>
        requires (!std::same_as<std::decay_t<UType>, IVector3>) && assign_multipliable_with<TType, const UType>
    auto operator*=(const IVector3<UType>& rhs) -> IVector3&
    {
        x *= rhs.x;
        y *= rhs.y;
        z *= rhs.z;
        return *this;
    }

    template <class UType>
        requires assign_multipliable_with<TType, const UType>
    auto operator*=(const UType&& rhs) -> IVector3&
    {
        x *= std::forward<const UType>(rhs);
        y *= std::forward<const UType>(rhs);
        z *= std::forward<const UType>(rhs);
        return *this;
    }

    template <class UType>
        requires (!std::same_as<std::decay_t<UType>, IVector3>) && assign_dividable_with<TType, const UType>
    auto operator/=(const IVector3<UType>& rhs) -> IVector3&
    {
        x /= rhs.x;
        y /= rhs.y;
        z /= rhs.z;
        return *this;
    }

    template <class UType>
        requires assign_dividable_with<TType, const UType>
    auto operator/=(const UType&& rhs) -> IVector3&
    {
        x /= std::forward<const UType>(rhs);
        y /= std::forward<const UType>(rhs);
        z /= std::forward<const UType>(rhs);
        return *this;
    }

    template <class UType>
        requires addable_with<TType, const UType>
    friend auto operator+(IVector3 lhs, const IVector3<UType>& rhs) -> IVector3<decltype(lhs.x + rhs.x)>
    {
        return {lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z};
    }

    template <class UType>
        requires subtractable_with<TType, const UType>
    friend auto operator-(IVector3 lhs, const IVector3<UType>& rhs) -> IVector3<decltype(lhs.x - rhs.x)>
    {
        return {lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z};
    }

    template <class UType>
        requires multipliable_with<TType, const UType>
    friend auto operator*(IVector3 lhs, const IVector3<UType>& rhs) -> IVector3<decltype(lhs.x * rhs.x)>
    {
        return {lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z};
    }

    template <class UType>
        requires (!std::same_as<std::decay_t<UType>, IVector3>) && multipliable_with<TType, const UType>
    friend auto operator*(IVector3 lhs, const UType&& rhs) -> IVector3<decltype(lhs.x * rhs)>
    {
        return {lhs.x * std::forward<const UType>(rhs), lhs.y * std::forward<const UType>(rhs), lhs.z * std::forward<const UType>(rhs)};
    }

    template <class UType>
        requires dividable_with<TType, UType>
    friend auto operator/(IVector3 lhs, const IVector3<UType>& rhs) -> IVector3<decltype(lhs.x / rhs.x)>
    {
        return {lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z};
    }

    template <class UType>
        requires (!std::same_as<std::decay_t<UType>, IVector3>) && dividable_with<TType, const UType>
    friend auto operator/(IVector3 lhs, const UType&& rhs) -> IVector3<decltype(lhs.x / rhs)>
    {
        return {lhs.x / std::forward<const UType>(rhs), lhs.y / std::forward<const UType>(rhs), lhs.z / std::forward<const UType>(rhs)};
    }

    [[nodiscard]] auto length() const -> float requires std::convertible_to<TType, float>
    {
        return std::sqrt(static_cast<float>(x * x + y * y + z * z));
    }

    [[nodiscard]] auto normalize() const -> IVector3<float> requires dividable_with<TType, float>
    {
        return *this / length();
    }

    [[nodiscard]] auto dot(const IVector3& rhs) const -> decltype(x * rhs.x + y * rhs.y + z * rhs.z)
    {
        return x * rhs.x + y * rhs.y + z * rhs.z;
    }

    [[nodiscard]] auto cross(const IVector3& rhs) const -> IVector3<decltype(y * rhs.z - z * rhs.y)>
    {
        return {
            y * rhs.z - z * rhs.y,
            z * rhs.x - x * rhs.z,
            x * rhs.y - y * rhs.x,
        };
    }
};

#endif //IVECTOR3_HPP
