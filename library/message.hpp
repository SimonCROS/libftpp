//
// Created by Simon Cros on 06/12/2024.
//

#ifndef MESSAGE_HPP
#define MESSAGE_HPP
#include "data_buffer.hpp"

class Client;
class Server;

class Message
{
    friend class Client;
    friend class Server;
    friend auto operator<<(DataBuffer& buffer, const Message& value) -> DataBuffer&;
    friend auto operator>>(DataBuffer& buffer, const Message& value) -> DataBuffer&;

private:
    int m_type;
    DataBuffer m_data;

    Message(int type, DataBuffer&& data);

public:
    enum Type: int
    {
        GET,
        UPDATE,
    };

    explicit Message(int type);

    explicit Message(Type type);

    [[nodiscard]] auto type() const -> int;

    template <class T>
    auto operator<<(const T value) -> Message&
    {
        m_data << value;
        return *this;
    }

    template <class T>
    auto operator>>(T& value) -> Message&
    {
        m_data >> value;
        return *this;
    }
};

auto operator<<(DataBuffer& buffer, const Message& value) -> DataBuffer&;

auto operator>>(DataBuffer& buffer, Message& value) -> DataBuffer&;

#endif //MESSAGE_HPP
