//
// Created by Simon Cros on 06/12/2024.
//

#include "client.hpp"

auto Client::connect(const std::string& address, const size_t& port) -> void
{
    (void)address;
    (void)port;
}

auto Client::disconnect() -> void
{
}

auto Client::defineAction(const Message::Type& messageType,
                          const std::function<void(Message& msg)>& action) -> void
{
    (void)messageType;
    (void)action;
}

auto Client::defineAction(const Message::Type& messageType,
                          const std::function<void(const Message& msg)>& action) -> void
{
    (void)messageType;
    (void)action;
}

auto Client::send(const Message& message) -> void
{
    (void)message;
}

auto Client::update() -> void
{
}
