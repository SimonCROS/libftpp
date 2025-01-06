//
// Created by Simon Cros on 06/12/2024.
//defineAction

#include <ranges>
#if __has_include(<sys/socket.h>) \
    && __has_include(<netinet/in.h>) \
    && __has_include(<fcntl.h>) \
    && __has_include(<unistd.h>) \
    && __has_include(<poll.h>)

#include "thread_safe_iostream.hpp"

#include <string>
#include <cerrno>

#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <netinet/in.h>

#include <cstring>
#include <iostream>

#include "server.hpp"
#include "internal/compile_helpers.hpp"

Server::~Server()
{
    stop();
}

auto Server::start(const size_t& p_port) -> void
{
    if (m_running)
        return;

    const int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        std::cerr << "Failed to start server: " << strerror(errno) << std::endl;
        close(fd);
        return;
    }

    constexpr int on = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) == -1)
    {
        std::cerr << "Failed to start server: " << strerror(errno) << std::endl;
        close(fd);
        return;
    }

    if (fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK) == -1)
    {
        std::cerr << "Failed to start server: " << strerror(errno) << std::endl;
        close(fd);
        return;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(p_port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(fd, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) == -1)
    {
        std::cerr << "Failed to start server: " << strerror(errno) << std::endl;
        close(fd);
        return;
    }

    if (listen(fd, 5) == -1)
    {
        std::cerr << "Failed to start server: " << strerror(errno) << std::endl;
        close(fd);
        return;
    }

    m_serverFd = fd;
#if __cpp_aggregate_paren_init
    m_pollfds.emplace_back(fd, POLLIN);
#else
    {
        pollfd pfd{};
        pfd.fd = fd;
        pfd.events = POLLIN;
        m_pollfds.push_back(pfd);
    }
#endif
    m_running = true;

    m_thread = std::thread(&Server::loop, this);
}

auto Server::stop() -> void
{
    if (!m_running)
        return;

    m_running = false;
    if (m_thread.joinable())
        m_thread.join();
    m_thread = {};
}

auto Server::defineAction(const int messageType,
                          const std::function<void(long long& clientID, Message& msg)>& action) -> void
{
    m_actionsNonConst[messageType] = action;
}

auto Server::defineAction(const Message::Type& messageType,
                          const std::function<void(long long& clientID, Message& msg)>& action) -> void
{
    defineAction(static_cast<int>(messageType), action);
}

auto Server::defineAction(const int messageType,
                          const std::function<void(long long& clientID, const Message& msg)>& action) -> void
{
    m_actions[messageType] = action;
}

auto Server::defineAction(const Message::Type& messageType,
                          const std::function<void(long long& clientID, const Message& msg)>& action) -> void
{
    defineAction(static_cast<int>(messageType), action);
}

auto Server::sendTo(const Message& message, const long long clientID) -> void
{
    std::scoped_lock lock(m_clientAccessMutex);

    const auto bytes = message.serialize();
    if (const auto o_client = getClientById(clientID))
    {
        if (o_client->get().fd > -1)
        {
            ::send(o_client->get().fd, bytes.data(), bytes.size(), 0);
        }
    }
}

// clientIds is not a reference as requested in the specifications
auto Server::sendToArray(const Message& message,
                         std::vector<long long> clientIDs) -> void // NOLINT(*-unnecessary-value-param)
{
    std::scoped_lock lock(m_clientAccessMutex);

    const auto bytes = message.serialize();
    for (const auto id : clientIDs)
    {
        if (const auto o_client = getClientById(id))
        {
            if (o_client->get().fd > -1)
            {
                ::send(o_client->get().fd, bytes.data(), bytes.size(), 0);
            }
        }
    }
}

auto Server::sendToAll(const Message& message) -> void
{
    std::scoped_lock lock(m_clientAccessMutex);

    const auto bytes = message.serialize();
#if defined(CAN_USE_RANGES) && __cpp_lib_ranges >= 201911L
    for (const auto& client : std::views::values(m_clients))
#else
    for (const auto& [_, client] : m_clients)
#endif
    {
        if (client.fd > -1)
        {
            ::send(client.fd, bytes.data(), bytes.size(), 0);
        }
    }
}

auto Server::update() -> void
{
    std::scoped_lock lock(m_clientAccessMutex);

    auto it = m_clients.begin();
    while (it != m_clients.end())
    {
        long long id = it->first;
        ServerClient& client = it->second;

        while (!client.messages.empty())
        {
            Message& message = client.messages.front();

            if (auto foundIt = m_actions.find(message.type()); foundIt != m_actions.end())
            {
                try
                {
                    std::invoke(foundIt->second, id, message);
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
                    std::invoke(foundIt->second, id, message);
                }
                catch (const std::exception& ex)
                {
                    threadSafeCout << "Catch exception: " << ex.what() << std::endl;
                }
            }

            client.messages.pop();
        }
        if (client.fd == -1)
            it = m_clients.erase(it);
        else
            ++it;
    }
}

auto Server::loop() -> void
{
    std::vector<pollfd> added;
    added.reserve(4);

    while (m_running)
    {
        if (poll(m_pollfds.data(), m_pollfds.size(), 0) == -1)
            break;

        auto it = m_pollfds.begin();
        while (it != m_pollfds.end())
        {
            if (it->revents == 0)
            {
                ++it;
                continue;
            }
            if (it->fd == m_serverFd)
            {
                added = acceptIncomingConnection();
                ++it;
                continue;
            }

            if (it->revents & POLLIN)
            {
                if (auto&& e_bytes = incomingRequest(it->fd))
                {
                    if (auto&& o_message = Message::deserialize(*e_bytes))
                    {
                        std::scoped_lock lock(m_clientAccessMutex);
                        if (const auto o_client = getClientByFd(it->fd))
                        {
                            o_client->get().messages.emplace(std::move(*o_message));
                        }
                    }
                }
                else
                {
                    disconnectClient(it->fd);
                    it = m_pollfds.erase(it);
                    continue;
                }
            }

            if (it->revents & POLLHUP)
            {
                disconnectClient(it->fd);
                it = m_pollfds.erase(it);
            }
            else
            {
                ++it;
            }
        }
        if (!added.empty())
        {
            m_pollfds.insert(m_pollfds.end(), std::make_move_iterator(added.begin()),
                             std::make_move_iterator(added.end()));
            added.clear();
        }

        std::this_thread::yield();
    }

    std::scoped_lock lock(m_clientAccessMutex);
    for (const auto& pfd : m_pollfds)
    {
        if (pfd.fd != m_serverFd)
        {
            shutdown(pfd.fd, SHUT_RDWR);
            close(pfd.fd);
        }
    }
    m_pollfds.clear();
    m_clients.clear();
    m_fdToClientId.clear();

    close(m_serverFd);
    m_serverFd = -1;
}

auto Server::getClientByFd(const int fd) -> std::optional<std::reference_wrapper<ServerClient>>
{
    if (const auto it = m_fdToClientId.find(fd); it != m_fdToClientId.end())
    {
        return m_clients[it->second];
    }
    return std::nullopt;
}

auto Server::getClientById(const client_id_t id) -> std::optional<std::reference_wrapper<ServerClient>>
{
    if (const auto it = m_clients.find(id); it != m_clients.end())
    {
        return it->second;
    }
    return std::nullopt;
}

auto Server::addClient(const int fd) -> client_id_t
{
    std::scoped_lock lock(m_clientAccessMutex);

    const client_id_t id = m_nextClientId;

    m_fdToClientId[fd] = id;
#if __cpp_aggregate_paren_init
    m_clients.try_emplace(id, fd);
#else
    if (!m_clients.contains(id))
        m_clients[id] = ServerClient{.fd = fd, .messages = {}};
#endif
    ++m_nextClientId;
    return id;
}

auto Server::disconnectClient(const int fd) -> void
{
    std::scoped_lock lock(m_clientAccessMutex);

    if (const auto it = m_fdToClientId.find(fd); it != m_fdToClientId.end())
    {
        threadSafeCout << "Client disconnected (" << it->second << ")" << std::endl;
        shutdown(fd, SHUT_RDWR);
        close(fd);
        m_clients[it->second].fd = -1;
        m_fdToClientId.erase(it);
    }
}

auto Server::removeClient(const client_id_t id) -> bool
{
    std::scoped_lock lock(m_clientAccessMutex);

    if (const auto it = m_clients.find(id); it != m_clients.end())
    {
        m_fdToClientId.erase(it->second.fd);
        m_clients.erase(it);
        return true;
    }
    return false;
}

auto Server::acceptIncomingConnection() -> std::vector<pollfd>
{
    // ReSharper disable once CppDFAConstantConditions CppDFAUnreachableCode
    if (!m_running)
        return {};

    std::vector<pollfd> added;

    int newFd;
    do
    {
        newFd = accept(m_serverFd, nullptr, nullptr);
        if (newFd < 0)
            break;

        if (fcntl(newFd, F_SETFL, fcntl(newFd, F_GETFL, 0) | O_NONBLOCK) == -1)
        {
            std::cerr << "Failed to accept new user: " << strerror(errno) << std::endl;
            shutdown(newFd, O_RDWR);
            close(newFd);
            continue;
        }

#if __cpp_aggregate_paren_init
        added.emplace_back(newFd, POLLIN);
#else
        {
            pollfd pfd{};
            pfd.fd = newFd;
            pfd.events = POLLIN;
            added.push_back(pfd);
        }
#endif
        auto id = addClient(newFd);
        threadSafeCout << "Client connected (" << id << ")" << std::endl;
    }
    while (newFd != -1);

    return added;
}

auto Server::incomingRequest(const int fd) const -> Expected<std::vector<uint8_t>, int>
{
    // ReSharper disable once CppDFAConstantConditions CppDFAUnreachableCode
    if (!m_running)
        return Unexpected(-2);

    constexpr size_t bufferSize = 50;
    static_assert(bufferSize > 1);

    uint8_t buffer[bufferSize];
    std::vector<uint8_t> bytes;

    ssize_t recvResult;
    do
    {
        recvResult = recv(fd, buffer, bufferSize, 0);
        if (recvResult == 0)
            return Unexpected(-1);
        if (recvResult > 0)
            bytes.insert(bytes.end(), buffer, buffer + recvResult);
    }
    while (recvResult != -1);

    return bytes;
}

#endif
