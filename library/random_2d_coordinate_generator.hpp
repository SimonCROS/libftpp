//
// Created by Simon Cros 15/12/2024.
//

#ifndef RANDOM_2_D_COORDINATE_GENERATOR_HPP
#define RANDOM_2_D_COORDINATE_GENERATOR_HPP

class Random2DCoordinateGenerator {
private:
    long long m_seed{};

public:
    Random2DCoordinateGenerator() = default;

    explicit Random2DCoordinateGenerator(long long seed);

    [[nodiscard]] auto seed() const -> long long;
    [[nodiscard]] auto operator()(const long long& x, const long long& y) const -> long long;
};

#endif //RANDOM_2_D_COORDINATE_GENERATOR_HPP
