//
// Created by Simon Cros on 06/12/2024.
//

#ifndef SERVER_HPP
#define SERVER_HPP
#include <functional>

#include "message.hpp"

class Server
{
public:
    auto start(const size_t& p_port) -> void;
    auto defineAction(const Message::Type& messageType,
                      const std::function<void(long long& clientID, const Message& msg)>& action) -> void;
    auto sendTo(const Message& message, long long clientID) -> void;
    auto sendToArray(const Message& message, std::vector<long long> clientIDs) -> void;
    auto sendToAll(const Message& message) -> void;
    auto update() -> void;
};

#endif //SERVER_HPP
