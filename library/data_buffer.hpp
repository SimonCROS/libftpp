//
// Created by Simon Cros on 11/10/24.
//

#ifndef DATA_BUFFER_HPP
#define DATA_BUFFER_HPP
#include <version>
#include <cstdint>
#include <deque>
#include <limits>
#include <optional>
#include <streambuf>
#include <string_view>

#include "internal/compile_helpers.hpp"

#if defined(CAN_USE_RANGES) && __cpp_lib_ranges >= 201911L
#include <ranges>
#else
#include <vector>
#endif

class DataBuffer
{
public:
    using container_type = std::deque<uint8_t>;
    using difference_type = container_type::difference_type;
    static constexpr auto size_max_v = std::numeric_limits<difference_type>::max();

    class DataDeserializationException final : public std::exception
    {
        [[nodiscard]] const char* what() const noexcept override;
    };

    DataBuffer() = default;

    [[nodiscard]] auto size() const -> container_type::size_type;
    [[nodiscard]] auto empty() const -> bool;
    [[nodiscard]] auto cbegin() const -> container_type::const_iterator;
    [[nodiscard]] auto cend() const -> container_type::const_iterator;

    auto pushBytes(const void* data, std::size_t size) -> void;

    [[nodiscard]] auto frontBytes(std::size_t size) ->
#if defined(CAN_USE_RANGES) && __cpp_lib_ranges >= 201911L
        std::ranges::subrange<container_type::const_iterator>;
#else
        std::vector<container_type::value_type>;
#endif

    auto unshiftBytes(std::size_t size, void* out) -> void;

private:
    std::deque<uint8_t> m_bytes;
};

template <class T>
    requires std::is_integral_v<T>
auto operator<<(DataBuffer& buffer, const T& value) -> DataBuffer&
{
    buffer.pushBytes(&value, sizeof(T));
    return buffer;
}

template <class T>
    requires std::is_integral_v<T>
auto operator>>(DataBuffer& buffer, T& value) -> DataBuffer&
{
    buffer.unshiftBytes(sizeof(T), &value);
    return buffer;
}

auto operator<<(DataBuffer& buffer, const std::string_view& value) -> DataBuffer&;

auto operator>>(DataBuffer& buffer, std::string& value) -> DataBuffer&;

#endif //DATA_BUFFER_HPP
