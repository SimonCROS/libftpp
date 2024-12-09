//
// Created by Simon Cros on 06/12/2024.
//

#ifndef CLIENT_HPP
#define CLIENT_HPP

#if __has_include(<sys/socket.h>) \
    && __has_include(<netinet/in.h>) \
    && __has_include(<fcntl.h>) \
    && __has_include(<unistd.h>) \
    && __has_include(<poll.h>) \
    && __has_include(<errno.h>)

#include <functional>
#include <string>
#include <sys/socket.h>

#include "message.hpp"

class Client
{
private:
    std::unordered_map<int, std::function<void(const Message& msg)>> m_actions;
    std::unordered_map<int, std::function<void(Message& msg)>> m_actionsNonConst;

public:
    auto connect(const std::string& address, const size_t& port) -> void;
    auto disconnect() -> void;
    auto defineAction(int messageType, const std::function<void(Message& msg)>& action) -> void;
    auto defineAction(const Message::Type& messageType, const std::function<void(Message& msg)>& action) -> void;
    auto defineAction(int messageType, const std::function<void(const Message& msg)>& action) -> void;
    auto defineAction(const Message::Type& messageType, const std::function<void(const Message& msg)>& action) -> void;
    auto send(const Message& message) -> void;
    auto update() -> void;
};

#endif
#endif //CLIENT_HPP
