//
// Created by Simon Cros on 06/12/2024.
//

#ifndef CLIENT_HPP
#define CLIENT_HPP
#include <functional>
#include <string>

#include "message.hpp"

class Client
{
public:
    auto connect(const std::string& address, const size_t& port) -> void;
    auto disconnect() -> void;
    auto defineAction(const Message::Type& messageType, const std::function<void(Message& msg)>& action) -> void;
    auto defineAction(const Message::Type& messageType, const std::function<void(const Message& msg)>& action) -> void;
    auto send(const Message& message) -> void;
    auto update() -> void;
};

#endif //CLIENT_HPP
