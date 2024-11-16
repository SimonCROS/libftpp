//
// Created by scros on 11/10/24.
//

#include "data_buffer.hpp"

#include <cassert>

auto DataBuffer::pushBytes(const void* data, const std::size_t size) -> void
{
    assert(data != nullptr);

    if (size > max_push_size)
        throw DataDeserializationException();

    auto ptr = static_cast<const uint8_t*>(data);
    mBytes.insert(mBytes.end(), ptr, ptr + size);
}

auto DataBuffer::frontBytes(const std::size_t size) -> std::ranges::subrange<decltype(mBytes)::const_iterator>
{
    if (size > max_push_size)
        throw DataDeserializationException();

    if (mBytes.size() < size)
        throw DataDeserializationException();

    return std::ranges::subrange(mBytes.begin(), mBytes.begin() + static_cast<decltype(mBytes)::difference_type>(size));
}

auto DataBuffer::unshiftBytes(const std::size_t size, void* out) -> void
{
    if (size > max_push_size)
        throw DataDeserializationException();

    if (mBytes.size() < size)
        throw DataDeserializationException();

    if (out != nullptr)
    {
        auto ptr = static_cast<uint8_t*>(out);
        std::copy_n(mBytes.begin(), size, ptr);
    }
    mBytes.erase(mBytes.begin(), mBytes.begin() + static_cast<decltype(mBytes)::difference_type>(size));
}
