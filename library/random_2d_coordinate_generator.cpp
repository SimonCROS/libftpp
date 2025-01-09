//
// Created by Simon Cros 15/12/2024.
//

#include <cstdint>

#include "random_2d_coordinate_generator.hpp"

auto Random2DCoordinateGenerator::get(const long x, const long y) const -> long
{
    constexpr uint64_t m = 0xc6a4a7935bd1e995LLU;
    constexpr int r = 47;

    uint64_t h = static_cast<uint64_t>(m_seed) ^ (2 * m);

    const auto iter = [&h](uint64_t k)
    {
        k *= m;
        k ^= k >> r;
        k *= m;

        h ^= k;
        h *= m;
    };

    iter(x);
    iter(y);

    h ^= h >> r;
    h *= m;
    h ^= h >> r;

    return static_cast<int64_t>(h);
}
