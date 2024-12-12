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
#include <netdb.h>
#include <string>
#include <cerrno>

#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <netinet/in.h>

#include <cstring>
#include <iostream>

#include "server.hpp"

auto Client::connect(const std::string& address, const size_t& port) -> void
{
    hostent* host = gethostbyname(address.c_str());
    if (!host)
    {
        std::cerr << "Failed to resolve hostname: " << address << std::endl;
        return;
    }

    const int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        std::cerr << "Failed to start client: " << strerror(errno) << std::endl;
        close(fd);
        return;
    }

    if (fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK) == -1)
    {
        std::cerr << "Failed to start client: " << strerror(errno) << std::endl;
        close(fd);
        return;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    std::memcpy(&serverAddress.sin_addr, host->h_addr, host->h_length);

    if (::connect(fd, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) == -1 && errno != EINPROGRESS)
    {
        std::cerr << "Failed to start client: " << strerror(errno) << std::endl;
        close(fd);
        return;
    }

    // m_thread = std::thread(&Server::loop, this);
}

auto Client::disconnect() -> void
{
}

auto Client::defineAction(const int messageType, const std::function<void(Message& msg)>& action) -> void
{
    m_actionsNonConst[messageType] = action;
}

auto Client::defineAction(const Message::Type& messageType, const std::function<void(Message& msg)>& action) -> void
{
    defineAction(static_cast<int>(messageType), action);
}

auto Client::defineAction(const int messageType, const std::function<void(const Message& msg)>& action) -> void
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
