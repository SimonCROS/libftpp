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
#include <sys/socket.h>

#include <cstring>
#include <iostream>

#include "server.hpp"
#include "thread_safe_iostream.hpp"

Client::~Client()
{
    disconnect();
}

auto Client::connect(const std::string& address, const size_t& port) -> void
{
    if (m_running)
        return;

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

    m_running = true;
    m_clientPollfd.fd = fd;
    m_clientPollfd.events = POLLIN | POLLOUT;
    m_clientPollfd.revents = {};

    bool connected = false;
    while (true)
    {
        if (poll(&m_clientPollfd, 1, 0) == -1)
        {
            threadSafeCout << "Cannot establish a connection:" << strerror(errno) << std::endl;
            break;
        }

        if (m_clientPollfd.revents & (POLLERR | POLLHUP))
        {
            threadSafeCout << "Cannot establish a connection" << std::endl;
            break;
        }
        if (m_clientPollfd.revents & POLLOUT) {
            threadSafeCout << "Connected" << std::endl;
            m_clientPollfd.events &= ~POLLOUT;
            connected = true;
            break;
        }
    }

    if (connected)
    {
        m_thread = std::thread(&Client::loop, this);
    }
    else
    {
        m_running = false;
        shutdown(m_clientPollfd.fd, SHUT_RDWR);
        close(m_clientPollfd.fd);
        m_clientPollfd = {};
    }
}

auto Client::disconnect() -> void
{
    m_running = false;
    if (m_thread.joinable())
        m_thread.join();
    m_thread = {};
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
    std::scoped_lock lock{m_clientAccessMutex};

    if (m_clientPollfd.fd == -1)
        return;

    const auto bytes = message.serialize();
    ::send(m_clientPollfd.fd, bytes.data(), bytes.size(), 0);
}

auto Client::update() -> void
{
    std::scoped_lock lock{m_clientAccessMutex};

    while (!m_messages.empty())
    {
        Message& message = m_messages.front();

        if (auto foundIt = m_actions.find(message.type()); foundIt != m_actions.end())
        {
            try
            {
                std::invoke(foundIt->second, message);
            }
            catch (const std::exception& ex)
            {
                threadSafeCout << "Catch exception: " << ex.what() << std::endl;
            }
        }
        if (auto foundIt = m_actionsNonConst.find(message.type()); foundIt != m_actionsNonConst.end())
        {
            try
            {
                std::invoke(foundIt->second, message);
            }
            catch (const std::exception& ex)
            {
                threadSafeCout << "Catch exception: " << ex.what() << std::endl;
            }
        }

        m_messages.pop();
    }
}

auto Client::loop() -> void
{
    std::vector<pollfd> added;
    added.reserve(4);

    while (m_running)
    {
        if (poll(&m_clientPollfd, 1, 0) == -1)
            break;

        if (m_clientPollfd.revents & POLLIN)
        {
            if (auto&& e_bytes = incomingRequest(m_clientPollfd.fd))
            {
                if (auto&& o_message = Message::deserialize(*e_bytes))
                {
                    std::scoped_lock lock{m_clientAccessMutex};
                    m_messages.emplace(std::move(*o_message));
                }
            }
            else
            {
                threadSafeCout << "Failed to read message." << std::endl;
                break;
            }
        }

        if (m_clientPollfd.revents & (POLLERR | POLLHUP))
        {
            threadSafeCout << "Connection lost" << std::endl;
            break;
        }

        std::this_thread::yield();
    }

    std::scoped_lock lock{m_clientAccessMutex};
    m_running = false;
    shutdown(m_clientPollfd.fd, SHUT_RDWR);
    close(m_clientPollfd.fd);
    m_clientPollfd = {};
}

auto Client::incomingRequest(const int fd) const
#if __cpp_lib_expected >= 202211L
    -> std::expected<std::vector<uint8_t>, int>
#else
    -> std::optional<std::vector<uint8_t>>
#endif
{
    // ReSharper disable once CppDFAConstantConditions CppDFAUnreachableCode
    if (!m_running)
#if __cpp_lib_expected >= 202211L
        return std::unexpected(-2);
#else
            return std::nullopt;
#endif

    constexpr size_t bufferSize = 50;
    static_assert(bufferSize > 1);

    uint8_t buffer[bufferSize];
    std::vector<uint8_t> bytes;

    ssize_t recvResult;
    do
    {
        recvResult = recv(fd, buffer, bufferSize, 0);
        if (recvResult == 0)
#if __cpp_lib_expected >= 202211L
            return std::unexpected(-1);
#else
                return std::nullopt;
#endif
        if (recvResult > 0)
            bytes.insert(bytes.end(), buffer, buffer + recvResult);
    }
    while (recvResult != -1);

    return bytes;
}

#endif
