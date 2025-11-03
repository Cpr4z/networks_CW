#pragma once

#include <vector>

#include <Document.hpp>
#include <User.hpp>

#include "Reactor.hpp"
#include "Task.hpp"

#include <json/json.hpp>

class NoteServer {
public:
    explicit NoteServer(int port, const std::string& host);

    Task<void> runAsync();
    Task<void> handleClient(int client_fd);
    void stop();

    void processMessage(const nlohmann::json& msg, int server_fd);

    void createDocument();
    void createUser(const std::string& name);

    void closeDocument(Id id);
    void deleteUser(Id id);

    std::shared_ptr<Reactor> getReactor() const { return m_reactor; }

private:
    Id getNextUserId();
    Id getNextDocId();

private:
    int m_port = 0;
    std::string m_host;
    int m_server_fd = -1;

    std::shared_ptr<Reactor> m_reactor;
    std::atomic<bool> m_running = false;

    std::mutex m_mutex;
    DocumentsMap m_documents;
    UsersMap m_users;
};
