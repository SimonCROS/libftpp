//
// Created by Simon Cros on 06/12/2024.
//

#ifndef SERVER_HPP
#define SERVER_HPP
#include <memory>
#include <queue>

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
#include <thread>

#include "message.hpp"

class Server
{
public:
    using client_id_t = long long;

private:
    struct ServerClient
    {
        int fd = -1;
        std::queue<Message> messages;
    };

    std::atomic_bool m_running = false;
    client_id_t m_nextClientId = 1;
    std::thread m_thread;

    int m_serverFd = -1;
    std::vector<pollfd> m_pollfds;
    std::unordered_map<int, client_id_t> m_fdToClientId;
    std::unordered_map<client_id_t, ServerClient> m_clients;

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
    auto loop() -> void;
    auto addClient(int fd) -> client_id_t;
    auto disconnectClient(int fd) -> void;
    auto removeClient(client_id_t id) -> bool;

    auto acceptIncomingConnection() -> std::vector<pollfd>;
    [[nodiscard]] auto incomingRequest(int fd) const
#if __cpp_lib_expected >= 202211L
        -> std::expected<std::vector<uint8_t>, int>;
#else
        -> std::optional<std::vector<uint8_t>>;
#endif
};

#endif
#endif //SERVER_HPP
