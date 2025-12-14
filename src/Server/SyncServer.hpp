#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <vector>
#include <atomic>

#include <Protocol.hpp>

#include "Repository.hpp"

class SyncServer {
private:
    int m_port;
    std::string m_host;
    int m_server_fd = -1;
    std::atomic<bool> m_running{false};
    std::vector<std::thread> m_client_threads;

    void handleClient(int client_fd);
    void processClientMessages(int client_fd);
    void processClientMessage(int client_fd,
                         const std::vector<uint8_t>& message);
    size_t getMessageLength(Protocol::Operation op,
    const std::vector<uint8_t>& buffer,
            size_t offset);

    void handleAuthRequest(int client_fd, const std::vector<uint8_t>& buffer);
    void handleRegistrationRequest(int client_fd, const std::vector<uint8_t>& buffer);

    void sendAuthResponse(int client_fd, const Protocol::AuthResponse& response);
    void sendRegistrationResponse(int client_fd, const Protocol::RegistrationResponse& response);

public:
    SyncServer(int port, const std::string& host);
    ~SyncServer();

    void run();  // Синхронная версия
    void stop();


private:
    RepositoryPtr m_repository;
};


