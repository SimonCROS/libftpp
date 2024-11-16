//
// Created by scros on 11/10/24.
//

#ifndef DATA_BUFFER_HPP
#define DATA_BUFFER_HPP
#include <cstdint>
#include <deque>
#include <optional>
#include <streambuf>
#include <vector>
#include <string_view>

class DataBuffer
{
private:
    std::deque<uint8_t> mBytes;

public:
    static constexpr auto max_push_size = std::numeric_limits<decltype(mBytes)::difference_type>::max();

    class DataDeserializationException final : public std::exception
    {
        [[nodiscard]] const char* what() const noexcept override
        {
            return "Failed to retrieve data from buffer";
        }
    };

    auto pushBytes(const void *data, std::size_t size) -> void;

    auto frontBytes(std::size_t size) -> std::ranges::subrange<decltype(mBytes)::const_iterator>;

    auto unshiftBytes(std::size_t size, void* out) -> void;
};

template<class T>
requires std::is_integral_v<T>
auto operator<<(DataBuffer& buffer, const T& value) -> DataBuffer&
{
    buffer.pushBytes(&value, sizeof(T));
    return buffer;
}

template<class T>
requires std::is_integral_v<T>
auto operator>>(DataBuffer& buffer, T& value) -> DataBuffer&
{
    buffer.unshiftBytes(sizeof(T), &value);
    return buffer;
}

inline auto operator<<(DataBuffer& buffer, const std::string_view& value) -> DataBuffer&
{
    buffer << value.size();
    buffer.pushBytes(value.data(), value.size());
    return buffer;
}

inline auto operator>>(DataBuffer& buffer, std::string& value) -> DataBuffer&
{
    size_t size = 0;
    buffer >> size;
    value.assign_range(buffer.frontBytes(size));
    buffer.unshiftBytes(size, nullptr);
    return buffer;
}

#endif //DATA_BUFFER_HPP
