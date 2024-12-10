//
// Created by Simon Cros on 06/12/2024.
//

#if __has_include(<sys/socket.h>) \
    && __has_include(<netinet/in.h>) \
    && __has_include(<fcntl.h>) \
    && __has_include(<unistd.h>) \
    && __has_include(<poll.h>)

#include <string>
#include <cerrno>

#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <netinet/in.h>

#include <cstring>
#include <iostream>

#include "server.hpp"
#include "data_buffer.hpp"

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

    if (fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK) == -1)
    {
        std::cerr << "Failed to start server: " << strerror(errno) << std::endl;
        close(fd);
        return;
    }

    sockaddr_in serverAddress{
        .sin_family = AF_INET,
        .sin_port = htons(p_port),
        .sin_addr = {
            .s_addr = INADDR_ANY
        },
        .sin_zero = {},
    };

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
    m_pollfds.emplace_back(fd, POLLIN);
    m_running = true;

    m_thread = std::thread(&Server::loop, this);
}

auto Server::stop() -> void
{
    if (!m_running)
        return;

    m_running = false;
    m_thread.join();
    for (const auto& pfd : m_pollfds)
    {
        shutdown(pfd.fd, SHUT_RDWR);
        close(pfd.fd);
    }
    m_pollfds.clear();
    m_clients.clear();
    m_fdToClientId.clear();
    m_serverFd = -1;
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
                std::invoke(foundIt->second, id, message);
            }
            if (auto foundIt = m_actionsNonConst.find(message.type()); foundIt != m_actionsNonConst.end())
            {
                std::invoke(foundIt->second, id, message);
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
        poll(m_pollfds.data(), m_pollfds.size(), 0);

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
                added = std::move(acceptIncomingConnection());
                ++it;
                continue;
            }

            auto result = incomingRequest(it->fd);
            if (!result)
            {
                // TODO better checks on expected error
                disconnectClient(it->fd);
                it = m_pollfds.erase(it);
            }
            else
            {
                int type;
                if (result->size() < sizeof(type))
                {
                    ++it;
                    continue;
                }

                const auto ptr = static_cast<uint8_t*>(static_cast<void*>(&type));
                std::ranges::copy_n(result->begin(), sizeof(type), ptr);
                result->erase(result->begin(), result->begin() + sizeof(type));

                DataBuffer buffer;
                buffer.pushBytes(result->data(), result->size());

                auto clientId = m_fdToClientId[it->fd];
                m_clients[clientId].messages.push(Message(type, std::move(buffer)));
                ++it;
            }
        }
        if (!added.empty())
        {
            m_pollfds.insert(m_pollfds.end(), std::make_move_iterator(added.begin()), std::make_move_iterator(added.end()));
            added.clear();
        }
    }
}

auto Server::addClient(const int fd) -> client_id_t
{
    const client_id_t id = m_nextClientId;

    m_fdToClientId[fd] = id;
    m_clients.try_emplace(id, fd);
    ++m_nextClientId;
    return id;
}

auto Server::disconnectClient(const int fd) -> void
{
    if (const auto it = m_fdToClientId.find(fd); it != m_fdToClientId.end())
    {
        shutdown(fd, SHUT_RDWR);
        close(fd);
        m_clients[it->second].fd = -1;
        m_fdToClientId.erase(it);
    }
}

auto Server::removeClient(const client_id_t id) -> bool
{
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

        added.emplace_back(newFd, POLLIN);
        addClient(newFd);
    }
    while (newFd != -1);

    return added;
}

auto Server::incomingRequest(const int fd) const
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
