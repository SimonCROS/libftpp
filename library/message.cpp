//
// Created by Simon Cros on 06/12/2024.
//

#include "message.hpp"

Message::Message(const int type): m_type(type)
{
}

Message::Message(const Type type): m_type(type)
{
}

auto Message::type() const -> int { return m_type; }
