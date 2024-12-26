//
// Created by simon on 15/12/2024.
//

#include "perlin_noise_2d.hpp"

#include <random>
#include <cmath>

auto PerlinNoise2D::smoothstep(const float t) -> float
{
    if (t <= 0.0f)
        return 0.0f;
    if (t >= 1.0f)
        return 1.0f;
    return t * t * (3.0f - 2.0f * t);
}

auto PerlinNoise2D::interpolate(const float a, const float b, const float t) -> float
{
    return a + (b - a) * smoothstep(t);
}

auto PerlinNoise2D::gradient_index(const uint32_t x, const uint32_t y) -> uint8_t
{
    return m_permutation[(m_permutation[x] + y) & m_table_size_mask];
}

PerlinNoise2D::PerlinNoise2D()
{
    std::random_device rd;
    std::mt19937 generator{rd};

    // generate [-1, 1] range, [-1, 1) without std::nextafter
    std::uniform_real_distribution<float> distribution{-1.0f, std::nextafter(1.0f, std::numeric_limits<float>::max())};

    constexpr auto dice = [&distribution, &generator] -> float { return distribution(generator); };
    for (auto& v : m_gradient)
    {
        v = IVector2{dice(), dice()};
    }
}

auto PerlinNoise2D::sample(const float x, const float y) -> float
{
    const uint32_t x0 = static_cast<int>(std::floor(x)) & m_table_size_mask;
    const uint32_t y0 = static_cast<int>(std::floor(y)) & m_table_size_mask;

    const uint32_t x1 = (x0 + 1) & m_table_size_mask;
    const uint32_t y1 = (y0 + 1) & m_table_size_mask;

    const float tx = x - std::floor(x);
    const float ty = y - std::floor(y);

    const float u = smoothstep(tx);
    const float v = smoothstep(ty);

    const IVector2<float>& p00 = m_gradient[gradient_index(x0, y0)];
    const IVector2<float>& p01 = m_gradient[gradient_index(x0, y1)];
    const IVector2<float>& p10 = m_gradient[gradient_index(x1, y0)];
    const IVector2<float>& p11 = m_gradient[gradient_index(x1, y1)];

    const float a = interpolate(, u);
}
