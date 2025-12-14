#pragma once

#include <vector>

#include <Document.hpp>
#include <User.hpp>

#include "Reactor.hpp"
#include "Task.hpp"
#include "Operation.hpp"

#include "Repository.hpp"

#include <json/json.hpp>

class NoteServer {
public:
    explicit NoteServer(int port, const std::string& host);

    ~NoteServer() {
        stop();
        if (m_reactor_thread.joinable()) {
            m_reactor_thread.join();
        }
    }

    Task<void> runAsync();
//    Task<void> handleClient(int client_fd);
    void stop();

    void createDocument();
//    void createUser(const std::string& name);

    void closeDocument(Id id);
    void deleteUser(Id id);

    void startReactor() {
        if (m_reactor) {
            m_reactor->run();
        }
    }

    std::shared_ptr<Reactor> getReactor() const { return m_reactor; }

private:
    Task<void> handleClient(int client_fd);
    Task<void> processClientMessages(int client_fd);
    void onClientDisconnected(int client_fd);

    size_t getMessageLength(Protocol::Operation op,
                            const std::vector<uint8_t>& buffer,
                            size_t offset);


    Id getNextUserId();
    Id getNextDocId();

    void processClientMessage(int client_fd, const std::vector<uint8_t>& message);


    void handleAuthRequest(int client_fd, const std::vector<uint8_t>& buffer);
    void handleRegistrationRequest(int client_fd, const std::vector<uint8_t>& buffer);

private:
    int m_port = 0;
    std::string m_host;
    int m_server_fd = -1;

    std::shared_ptr<Reactor> m_reactor;
    std::atomic<bool> m_running = false;

    RepositoryPtr m_repository;

    std::unordered_map<int, std::shared_ptr<User>> m_client_users;
    std::mutex m_mutex;
    DocumentsMap m_documents;
    UsersMap m_users;

    std::thread m_reactor_thread;
};
