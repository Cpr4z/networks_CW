#include "Server.hpp"

#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#include <json/json.hpp>

#include "AcceptAwaiter.hpp"

#include <iostream>

#include <Protocol.hpp>

NoteServer::NoteServer(int port, const std::string& host)
    : m_port(port), m_host(host) {
    m_reactor = std::make_shared<Reactor>();
}

void processMessage(const nlohmann::json& msg, int server_fd) {

}


Task<void> NoteServer::handleClient(int client_fd) {
    char buffer[4096];
    while (true) {
        ssize_t received = recv(client_fd, buffer, sizeof(buffer), 0);
        if (received <= 0) {
            close(client_fd);
            co_return;
        }

        auto msg_opt = Protocol::decode(buffer, received);
        if (!msg_opt) {
            continue;
        }
        const nlohmann::json& msg = *msg_opt;
        processMessage(msg, client_fd);
    }
}

Task<void> NoteServer::runAsync() {
    m_running = true;
    m_server_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (m_server_fd == -1) {
        perror("socket");
        co_return;
    }

    int opt = 1;
    setsockopt(m_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(m_port);
    inet_pton(AF_INET, m_host.c_str(), &addr.sin_addr);

    if (bind(m_server_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(m_server_fd);
        co_return;
    }

    if (listen(m_server_fd, SOMAXCONN) < 0) {
        perror("listen");
        close(m_server_fd);
        co_return;
    }

    int flags = fcntl(m_server_fd, F_GETFL, 0);
    fcntl(m_server_fd, F_SETFL, flags | O_NONBLOCK);

    std::cout << "Server is running\n";

    while (m_running) {
        int client_fd = co_await AcceptAwaiter(m_server_fd, m_reactor);
        if (client_fd >= 0) {
            // Можно запустить отдельную корутину для клиента:
            // co_spawn(handleClient(client_fd));
        }
    }
}

Id NoteServer::getNextUserId() {
    return m_users.size() + 1;
}

Id NoteServer::getNextDocId() {
    return m_users.size() + 1;
}

void NoteServer::stop() {
    m_running = false;
    if (m_server_fd >= 0) {
        close(m_server_fd);
        m_server_fd = -1;
    }
}

void NoteServer::createDocument() {
    Id new_id = getNextDocId();
    std::lock_guard<std::mutex> lock(m_mutex);
    m_documents.insert({new_id, std::make_shared<Document>(new_id)});
}

void NoteServer::createUser(const std::string& name) {
    Id new_id = getNextUserId();
    std::lock_guard<std::mutex> lock(m_mutex);
    m_users.insert({new_id, std::make_shared<User>(name, new_id)});
}

void NoteServer::closeDocument(Id id) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_documents.erase(id);
}

void NoteServer::deleteUser(Id id) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_users.erase(id);
}