//
// Created by Simon Cros on 06/12/2024.
//

#ifndef SERVER_HPP
#define SERVER_HPP

#if __has_include(<sys/socket.h>) \
    && __has_include(<netinet/in.h>) \
    && __has_include(<fcntl.h>) \
    && __has_include(<unistd.h>) \
    && __has_include(<poll.h>)

#include <functional>
#include <thread>
#include <optional>
#include <mutex>
#include <memory>
#include <queue>
#include <poll.h>

#include "expected.hpp"
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
    std::thread m_thread;

    // Only used by server thread
    int m_serverFd = -1;
    std::vector<pollfd> m_pollfds;

    std::recursive_mutex m_clientAccessMutex;
    client_id_t m_nextClientId = 1;
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

    auto getClientByFd(int fd) -> std::optional<std::reference_wrapper<ServerClient>>;
    auto getClientById(client_id_t id) -> std::optional<std::reference_wrapper<ServerClient>>;

    auto addClient(int fd) -> client_id_t;
    auto disconnectClient(int fd) -> void;
    auto removeClient(client_id_t id) -> bool;

    auto acceptIncomingConnection() -> std::vector<pollfd>;
    [[nodiscard]] auto incomingRequest(int fd) const -> Expected<std::vector<uint8_t>, int>;
};

#endif
#endif //SERVER_HPP
