//
// Created by Simon Cros on 06/12/2024.
//

#include <algorithm>

#include "message.hpp"

Message::Message(const int type, DataBuffer&& data): m_type(type), m_data(std::move(data)) {}

Message::Message(const int type): m_type(type) {}

Message::Message(const Type type): m_type(type) {}

auto Message::type() const -> int { return m_type; }

auto Message::serialize() const -> std::vector<uint8_t>
{
    std::vector<uint8_t> bytes;

    const auto ptr = static_cast<const uint8_t*>(static_cast<const void*>(&m_type));
    bytes.insert(bytes.end(), ptr, ptr + sizeof(m_type));

    bytes.insert(bytes.end(), m_data.cbegin(), m_data.cend());

    return bytes;
}

auto Message::deserialize(const std::vector<uint8_t>& bytes) -> std::optional<Message>
{
    int type;
    if (bytes.size() < sizeof(type))
        return std::nullopt;

    const auto ptr = static_cast<uint8_t*>(static_cast<void*>(&type));
    std::ranges::copy_n(bytes.begin(), sizeof(type), ptr);

    DataBuffer buffer;
    buffer.pushBytes(bytes.data() + sizeof(type), bytes.size() - sizeof(type));

    return Message(type, std::move(buffer));
}
