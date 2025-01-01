//
// Created by simon on 15/12/2024.
//

#include "perlin_noise_2d.hpp"

#include <random>
#include <cmath>
#include <algorithm>

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
    std::mt19937 generator{rd()};

    // generate [-1, 1] range, [-1, 1) without std::nextafter
    std::uniform_real_distribution<float> distribution{-1.0f, std::nextafter(1.0f, std::numeric_limits<float>::max())};

    const auto dice = [&distribution, &generator] -> float { return distribution(generator); };
    for (auto& v : m_gradient)
    {
        v = IVector2(dice(), dice()).normalize();
    }
}

auto PerlinNoise2D::sample(const float x, const float y) -> float
{
    const uint32_t ix0 = static_cast<int>(std::floor(x)) & m_table_size_mask;
    const uint32_t iy0 = static_cast<int>(std::floor(y)) & m_table_size_mask;

    const uint32_t ix1 = (ix0 + 1) & m_table_size_mask;
    const uint32_t iy1 = (iy0 + 1) & m_table_size_mask;

    const float tx = x - std::floor(x);
    const float ty = y - std::floor(y);

    const float u = smoothstep(tx);
    const float v = smoothstep(ty);

    const auto& gradient00 = m_gradient[gradient_index(ix0, iy0)];
    const auto& gradient01 = m_gradient[gradient_index(ix0, iy1)];
    const auto& gradient10 = m_gradient[gradient_index(ix1, iy0)];
    const auto& gradient11 = m_gradient[gradient_index(ix1, iy1)];

    const IVector2 offset00(tx, ty);
    const IVector2 offset01(tx, ty - 1);
    const IVector2 offset10(tx - 1, ty);
    const IVector2 offset11(tx - 1, ty - 1);

    const float a = interpolate(gradient00.dot(offset00), gradient10.dot(offset10), u);
    const float b = interpolate(gradient01.dot(offset01), gradient11.dot(offset11), u);

    return interpolate(a, b, v);
}

auto PerlinNoise2D::sample(const float x, const float y, float amplitude, float frequency, const int octaveCount,
                           const float persistence, const float lacunarity) -> float
{
    float value = 0;

    for (int i = 0; i < octaveCount; i++)
    {
        value += amplitude * sample(x * frequency, y * frequency);
        amplitude *= persistence;
        frequency *= lacunarity;
    }

    return std::clamp(value, -1.0f, 1.0f);
}

auto PerlinNoise2D::operator()(const float x, const float y) -> float
{
    return sample(x, y);
}
