//
// Created by Simon Cros 15/12/2024.
//

#ifndef RANDOM_2_D_COORDINATE_GENERATOR_HPP
#define RANDOM_2_D_COORDINATE_GENERATOR_HPP
#include "vector2.hpp"

class Random2DCoordinateGenerator {
private:
    long m_seed{};

public:
    Random2DCoordinateGenerator() = default;

    constexpr explicit Random2DCoordinateGenerator(const long seed): m_seed{seed} {}

    [[nodiscard]] constexpr auto seed() const -> long
    {
        return m_seed;
    }

    [[nodiscard]] auto get(long x, long y) const -> long;

    [[nodiscard]] auto get(Vector2<long> point) const -> long
    {
        return get(point.x, point.y);
    }

    [[nodiscard]] auto operator()(const long x, const long y) const -> long
    {
        return get(x, y);
    }

    [[nodiscard]] auto operator()(Vector2<long> point) const -> long
    {
        return get(point.x, point.y);
    }
};

#endif //RANDOM_2_D_COORDINATE_GENERATOR_HPP
