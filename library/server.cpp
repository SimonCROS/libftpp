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

#include "server.hpp"
#include "data_buffer.hpp"

Server::~Server()
{
    stop();
}

auto Server::start(const size_t& p_port) -> void
{
    const int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        close(fd);
        return;
    }

    const int fcntlResult = fcntl(fd, F_SETFL, O_NONBLOCK);
    if (fcntlResult == -1)
    {
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

    const int bindResult = bind(fd, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress));
    if (bindResult == -1)
    {
        close(fd);
        return;
    }

    const int listenResult = listen(fd, 5);
    if (listenResult == -1)
    {
        close(fd);
        return;
    }

    m_serverFd = fd;
    m_pollfds.emplace_back(fd, POLLIN);
    m_running = true;
}

auto Server::stop() -> void
{
    m_running = false;
    for (const auto& pfd : m_pollfds)
    {
        shutdown(pfd.fd, SHUT_RDWR);
        close(pfd.fd);
    }
    m_pollfds.clear();
    m_clientToFd.clear();
    m_fdToClient.clear();
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
    if (!m_running)
        return;

    poll(m_pollfds.data(), m_pollfds.size(), -1);

    auto it = m_pollfds.begin();
    while (it != m_pollfds.end())
    {
        if (it->revents == 0)
            continue;
        if (it->fd == m_serverFd)
        {
            acceptIncomingConnection();
            continue;
        }

        auto result = incomingRequest(it->fd);
        if (!result)
        {
            // TODO better checks on expected error
            removeClient(it->fd);
            it = m_pollfds.erase(it);
        }
        else
        {
            auto clientId = m_fdToClientId[it->fd];

            ++it;
        }
    }
}

auto Server::addClient(const int fd) -> client_id_t
{
    const size_t id = m_nextClientId;

    m_fdToClientId[fd] = id;
    m_clients.try_emplace(id, fd);
    ++m_nextClientId;
    return id;
}

auto Server::removeClient(const int fd) -> void
{
    if (const auto it = m_fdToClient.find(fd); it != m_fdToClient.end())
    {
        m_clientToFd.erase(it->second);
        m_fdToClient.erase(it);
    }
}

auto Server::acceptIncomingConnection() -> void
{
    // ReSharper disable once CppDFAConstantConditions CppDFAUnreachableCode
    if (!m_running)
        return;

    int newFd;
    do
    {
        newFd = accept(m_serverFd, nullptr, nullptr);
        if (newFd < 0)
            break;

        m_pollfds.emplace_back(newFd, POLLIN);
        addClient(newFd);
    }
    while (newFd != -1);
}

auto Server::incomingRequest(const int fd) const
#if __cpp_lib_expected >= 202211L
    -> std::expected<DataBuffer, int>
#else
    -> std::optional<DataBuffer>
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

    char buffer[bufferSize];
    DataBuffer bytes;

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
            bytes.pushBytes(buffer, recvResult);
    }
    while (recvResult != -1);

    return bytes;
}

#endif
