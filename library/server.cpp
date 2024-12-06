//
// Created by Simon Cros on 06/12/2024.
//

#include "server.hpp"

auto Server::start(const size_t& p_port) -> void
{
    (void)p_port;
}

auto Server::defineAction(const Message::Type& messageType,
    const std::function<void(long long& clientID, const Message& msg)>& action) -> void
{
    (void)messageType;
    (void)action;
}

auto Server::sendTo(const Message& message, long long clientID) -> void
{
    (void)message;
    (void)clientID;
}

auto Server::sendToArray(const Message& message, std::vector<long long> clientIDs) -> void
{
    (void)message;
    (void)clientIDs;
}

auto Server::sendToAll(const Message& message) -> void
{
    (void)message;
}

auto Server::update() -> void
{
}
