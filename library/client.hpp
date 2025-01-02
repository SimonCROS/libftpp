//
// Created by Simon Cros on 06/12/2024.
//

#ifndef CLIENT_HPP
#define CLIENT_HPP

#if __has_include(<sys/socket.h>) \
    && __has_include(<netinet/in.h>) \
    && __has_include(<fcntl.h>) \
    && __has_include(<unistd.h>) \
    && __has_include(<poll.h>) \
    && __has_include(<errno.h>)

#include <version>
#include <mutex>
#include <atomic>
#include <thread>
#include <functional>
#include <string>
#include <queue>
#include <poll.h>

#include "expected.hpp"
#include "message.hpp"

class Client
{
private:
    std::atomic_bool m_running = false;
    std::thread m_thread;

    pollfd m_clientPollfd = {};

    std::recursive_mutex m_clientAccessMutex;
    std::queue<Message> m_in_messages;
    std::queue<std::vector<u_int8_t>> m_out_messages;

    std::unordered_map<int, std::function<void(const Message& msg)>> m_actions;
    std::unordered_map<int, std::function<void(Message& msg)>> m_actionsNonConst;

public:
    ~Client();

    auto connect(const std::string& address, const size_t& port) -> void;
    auto disconnect() -> void;
    auto defineAction(int messageType, const std::function<void(Message& msg)>& action) -> void;
    auto defineAction(const Message::Type& messageType, const std::function<void(Message& msg)>& action) -> void;
    auto defineAction(int messageType, const std::function<void(const Message& msg)>& action) -> void;
    auto defineAction(const Message::Type& messageType, const std::function<void(const Message& msg)>& action) -> void;
    auto send(const Message& message) -> void;
    auto update() -> void;

private:
    auto loop() -> void;

    [[nodiscard]] auto incomingRequest(int fd) const -> Expected<std::vector<uint8_t>, int>;
};

#endif
#endif //CLIENT_HPP
