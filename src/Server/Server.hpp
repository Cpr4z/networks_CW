#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <unordered_set>

#include <Protocol.hpp>

#include "Repository.hpp"

class Server {
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

private:

    void handleAuthRequest(int client_fd, const std::vector<uint8_t>& buffer);
    void handleRegistrationRequest(int client_fd, const std::vector<uint8_t>& buffer);
    void handleGetNotesRequest(int client_fd, const std::vector<uint8_t>& buffer);
    void handleSyncRequest(int client_fd, const std::vector<uint8_t>& buffer);
    void handleCreateNoteRequest(int client_fd, const std::vector<uint8_t>& buffer);
    void handleOpenNoteRequest(int client_fd, const std::vector<uint8_t>& buffer);
    void handleUpdateTextRequest(int client_fd, const std::vector<uint8_t>& buffer);
    void handleShareNoteRequest(int client_fd, const std::vector<uint8_t>& buffer);
    void handleShareNoteNotifyRequest(int client_fd, const std::vector<uint8_t>& buffer);
    void handleApproveMergeRequest(int client_fd, const std::vector<uint8_t>& buffer);
    void handleOwnerApproveMergeRequest(int client_fd, const std::vector<uint8_t>& buffer);

    void sendAuthResponse(int client_fd, const Protocol::AuthResponse& response);
    void sendRegistrationResponse(int client_fd, const Protocol::RegistrationResponse& response);
    void sendGetNotesResponse(int client_fd, const Protocol::GetNotesResponse& response);
    void sendSyncResponse(int client_fd, const Protocol::SyncNoteResponse& response);
    void sendCreateNoteResponse(int client_fd, const Protocol::CreateNoteResponse& response);
    void sendOpenNoteResponse(int client_fd, const Protocol::OpenNoteResponse& response);
    void sendUpdateTextResponse(int client_fd, const Protocol::UpdateTextResponse& response);
    void sendShareNoteResponse(int client_fd, const Protocol::ShareNoteResponse& response);
    void sendApproveMergeResponse(int client_fd, const Protocol::ApproveMergeResponse& response);
    void sendOwnerApproveMergeResponse(int client_fd, const Protocol::OwnerApproveMergeResponse& response);


    void sendShareNoteNotifyRequest(int client_fd, const Protocol::ShareNoteNotifyRequest& request);
    void sendOwnerApproveMergeRequest(int client_fd, const Protocol::OwnerApproveMergeRequest& request);

private:
    void broadcastToAllClients(int client_fd, const std::vector<uint8_t>& data);

public:
    Server(int port, const std::string& host);
    ~Server();

    void run();
    void stop();

private:
    std::mutex m_clients_mutex;
    std::unordered_set<int> m_connected_clients;
    std::unordered_map<int, int> m_clients; // user_id -> client_fd
    RepositoryPtr m_repository;
};


