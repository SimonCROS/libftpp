//
// Created by scros on 11/10/24.
//

#include "data_buffer.hpp"

#include <cassert>
#include <algorithm>
#include <ranges>

const char* DataBuffer::DataDeserializationException::what() const noexcept
{
    return "Failed to retrieve data from buffer";
}

auto DataBuffer::pushBytes(const void* data, const std::size_t size) -> void
{
    assert(data != nullptr);

    if (size > size_max_v)
        throw DataDeserializationException();

    const auto ptr = static_cast<const uint8_t*>(data);
    mBytes.insert(mBytes.end(), ptr, ptr + size);
}

auto DataBuffer::frontBytes(const std::size_t size) -> std::ranges::subrange<decltype(mBytes)::const_iterator>
{
    if (size > size_max_v)
        throw DataDeserializationException();

    if (mBytes.size() < size)
        throw DataDeserializationException();

    return std::ranges::subrange(mBytes.begin(), mBytes.begin() + static_cast<decltype(mBytes)::difference_type>(size));
}

auto DataBuffer::unshiftBytes(const std::size_t size, void* out) -> void
{
    if (size > size_max_v)
        throw DataDeserializationException();

    if (mBytes.size() < size)
        throw DataDeserializationException();

    if (out != nullptr)
    {
        const auto ptr = static_cast<uint8_t*>(out);
        std::ranges::copy_n(mBytes.begin(), static_cast<difference_type>(size), ptr);
    }
    mBytes.erase(mBytes.begin(), mBytes.begin() + static_cast<decltype(mBytes)::difference_type>(size));
}

auto operator<<(DataBuffer& buffer, const std::string_view& value) -> DataBuffer&
{
    buffer << value.size();
    buffer.pushBytes(value.data(), value.size());
    return buffer;
}

auto operator>>(DataBuffer& buffer, std::string& value) -> DataBuffer&
{
    size_t size = 0;
    buffer >> size;

#ifdef __cpp_lib_containers_ranges
    value.assign_range(buffer.frontBytes(size));
#else
    auto bytes = buffer.frontBytes(size);
    value.assign(bytes.begin(), bytes.end());
#endif

    buffer.unshiftBytes(size, nullptr);
    return buffer;
}
