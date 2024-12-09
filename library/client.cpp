//
// Created by Simon Cros on 06/12/2024.
//

#if __has_include(<sys/socket.h>) \
    && __has_include(<netinet/in.h>) \
    && __has_include(<fcntl.h>) \
    && __has_include(<unistd.h>) \
    && __has_include(<poll.h>) \
    && __has_include(<errno.h>)

#include "client.hpp"

auto Client::connect(const std::string& address, const size_t& port) -> void
{
    (void)address;
    (void)port;
}

auto Client::disconnect() -> void
{
}

auto Client::defineAction(const int messageType,
                          const std::function<void(Message& msg)>& action) -> void
{
    m_actionsNonConst[messageType] = action;
}

auto Client::defineAction(const Message::Type& messageType,
                          const std::function<void(Message& msg)>& action) -> void
{
    defineAction(static_cast<int>(messageType), action);
}

auto Client::defineAction(const int messageType,
                          const std::function<void(const Message& msg)>& action) -> void
{
    m_actions[messageType] = action;
}

auto Client::defineAction(const Message::Type& messageType,
                          const std::function<void(const Message& msg)>& action) -> void
{
    defineAction(static_cast<int>(messageType), action);
}

auto Client::send(const Message& message) -> void
{
    (void)message;
}

auto Client::update() -> void
{
}

#endif
