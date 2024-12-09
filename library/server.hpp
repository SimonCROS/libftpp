//
// Created by Simon Cros on 06/12/2024.
//

#ifndef SERVER_HPP
#define SERVER_HPP
#include <memory>

#if __has_include(<sys/socket.h>) \
    && __has_include(<netinet/in.h>) \
    && __has_include(<fcntl.h>) \
    && __has_include(<unistd.h>) \
    && __has_include(<poll.h>)

#include <version>
#include <functional>
#if __cpp_lib_expected >= 202211L
#include <expected>
#else
#include <optional>
#endif

#include <poll.h>

#include "message.hpp"

class Server
{
public:
    using client_id_t = size_t;

    struct Client
    {
        int fd;
    };

private:
    bool m_running;
    client_id_t m_nextClientId = 1;

    int m_serverFd = -1;
    std::vector<pollfd> m_pollfds;
    std::unordered_map<int, size_t> m_fdToClientId;
    std::unordered_map<size_t, Client> m_clients;

    std::unordered_map<int, std::function<void(long long& clientID, const Message& msg)>> m_actions;
    std::unordered_map<int, std::function<void(long long& clientID, Message& msg)>> m_actionsNonConst;

public:
    ~Server();

    auto start(const size_t& p_port) -> void;
    auto stop() -> void;
    auto defineAction(int messageType,
                      const std::function<void(long long& clientID, Message& msg)>& action) -> void;
    auto defineAction(const Message::Type& messageType,
                      const std::function<void(long long& clientID, Message& msg)>& action) -> void;
    auto defineAction(int messageType,
                      const std::function<void(long long& clientID, const Message& msg)>& action) -> void;
    auto defineAction(const Message::Type& messageType,
                      const std::function<void(long long& clientID, const Message& msg)>& action) -> void;
    auto sendTo(const Message& message, long long clientID) -> void;
    auto sendToArray(const Message& message, std::vector<long long> clientIDs) -> void;
    auto sendToAll(const Message& message) -> void;
    auto update() -> void;

private:
    auto addClient(int fd) -> client_id_t;
    auto removeClient(int fd) -> void;

    auto acceptIncomingConnection() -> void;
    [[nodiscard]] auto incomingRequest(int fd) const
#if __cpp_lib_expected >= 202211L
        -> std::expected<DataBuffer, int>;
#else
        -> std::optional<DataBuffer>;
#endif
};

#endif
#endif //SERVER_HPP
