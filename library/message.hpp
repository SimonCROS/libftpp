//
// Created by Simon Cros on 06/12/2024.
//

#ifndef MESSAGE_HPP
#define MESSAGE_HPP
#include "data_buffer.hpp"

class Message
{
private:
    int m_type;
    DataBuffer m_data;

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

#endif //MESSAGE_HPP
