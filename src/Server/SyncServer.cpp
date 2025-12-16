#include "SyncServer.hpp"
#include <fcntl.h>
#include <cstring>

SyncServer::SyncServer(int port, const std::string& host)
        : m_port(port), m_host(host) {
    m_repository = std::make_shared<Repository>();
}

SyncServer::~SyncServer() {
    stop();
}

void SyncServer::sendAuthResponse(int client_fd, const Protocol::AuthResponse& response) {
    std::vector<uint8_t> response_buffer = Protocol::encodeAuthResponse(response);
    ssize_t sent = send(client_fd, response_buffer.data(), response_buffer.size(), 0);
    if (sent < 0) {
        perror("send");
    } else {
        std::cout << "Sent response to client " << client_fd << std::endl;
    }
}

void SyncServer::sendRegistrationResponse(int client_fd, const Protocol::RegistrationResponse& response) {
    std::vector<uint8_t> response_buffer = Protocol::encodeRegistrationResponse(response);
    ssize_t sent = send(client_fd, response_buffer.data(), response_buffer.size(), 0);
    if (sent < 0) {
        perror("send");
    } else {
        std::cout << "Sent response to client " << client_fd << std::endl;
    }
}

void SyncServer::sendGetNotesResponse(int client_fd, const Protocol::GetNotesResponse& response) {
    std::vector<uint8_t> response_buffer = Protocol::encodeGetNotesResponse(response);
    ssize_t sent = send(client_fd, response_buffer.data(), response_buffer.size(), 0);
    if (sent < 0) {
        perror("send");
    } else {
        std::cout << "Sent response to client " << client_fd << std::endl;
    }
}

void SyncServer::sendSyncResponse(int client_fd, const Protocol::SyncNoteResponse& response) {
    std::vector<uint8_t> response_buffer = Protocol::encodeSyncResponse(response);
    ssize_t sent = send(client_fd, response_buffer.data(), response_buffer.size(), 0);
    if (sent < 0) {
        perror("send");
    } else {
        std::cout << "Sent response to client " << client_fd << std::endl;
    }
}

void SyncServer::sendCreateNoteResponse(int client_fd, const Protocol::CreateNoteResponse& response) {
    std::vector<uint8_t> response_buffer = Protocol::encodeCreateNoteResponse(response);
    std::cout << "After data encoded for Note Response" << std::endl;
    ssize_t sent = send(client_fd, response_buffer.data(), response_buffer.size(), 0);
    if (sent < 0) {
        perror("send");
    } else {
        std::cout << "Sent response to client " << client_fd << std::endl;
    }
}

void SyncServer::handleAuthRequest(int client_fd, const std::vector<uint8_t>& buffer) {
    const auto& request = Protocol::decodeAuthRequest(buffer);
    std::cout << "Auth request from client " << client_fd << std::endl;
    std::cout << request->login << std::endl;
    std::cout << request->password << std::endl;

    Protocol::AuthResponse response;
    // mock data for test
//    {
//        response.op = Protocol::Operation::AUTH;
//        response.status = 0;
//        response.user_id = 1;
//    }

    response.op = Protocol::Operation::AUTH;
    auto result = m_repository->validateUser(request->login, request->password);
    if (result) {
        response.status = 0;
        response.user_id = *result;
    } else {
        response.user_id = 0;
        response.status = static_cast<uint8_t>(result.error());
    }

//    if (m_repository->isUserExists(client_fd)) {
//
//    }
    sendAuthResponse(client_fd, response);
}

void SyncServer::handleRegistrationRequest(int client_fd, const std::vector<uint8_t>& buffer) {
    const auto& request = Protocol::decodeRegistrationRequest(buffer);
    std::cout << "Registration request from client " << client_fd << std::endl;
    std::cout << request->login << std::endl;
    std::cout << request->password << std::endl;
    Protocol::RegistrationResponse response;

    // mock data for test
//    {
//        response.op = Protocol::Operation::REGISTRATION;
//        response.status = 0;
//        response.user_id = 34;
//    }

    response.op = Protocol::Operation::REGISTRATION;
    auto result = m_repository->validateUser(request->login, request->password);
    if (result) {
        response.status = 1;
        response.user_id = *result;
    } else {
        std::cout << "Registration: user not found, adding new user" << std::endl;
//        auto new_user_id = m_repository->addUser(request->login, request->password);
        response.status = 0;
        response.user_id = m_repository->addUser(request->login, request->password);
        std::cout << "Registration status is: " << static_cast<int>(response.status) << std::endl;
        std::cout << "New user id is: " << response.user_id << std::endl;
    }
    sendRegistrationResponse(client_fd, response);
}

void SyncServer::handleGetNotesRequest(int client_fd, const std::vector<uint8_t>& buffer) {
    const auto& request = Protocol::decodeGetNotesRequest(buffer);

}

void SyncServer::handleSyncRequest(int client_fd, const std::vector<uint8_t>& buffer) {
    const auto& request = Protocol::decodeSyncRequest(buffer);
}

void SyncServer::handleCreateNoteRequest(int client_fd, const std::vector<uint8_t>& buffer) {
    std::cout << "Create note request" << std::endl;
    const auto& request = Protocol::decodeCreateNoteRequest(buffer);
    std::cout << "User id, who want to create note: " << request->user_id << std::endl;
    std::cout << "Note title: " << request->note_title << std::endl;

    Protocol::CreateNoteResponse response;
    response.op = Protocol::Operation::CREATE_NOTE;

    if (m_repository->isNoteExists(request->user_id, request->note_title)) {
        std::cout << "Note with this name is already exists for this user" << std::endl;
        response.status = 1;
        response.note_id = 0;
    } else {
        std::cout << "Creating new note" << std::endl;
        response.status = 0;
        response.note_id = m_repository->addNote(request->user_id, request->note_title);
        response.note_title = request->note_title;
        std::cout << "After adding new note with title:" << response.note_title << " and id: " << response.note_id << std::endl;
    }

    sendCreateNoteResponse(client_fd, response);
}

size_t SyncServer::getMessageLength(Protocol::Operation op,
                                    const std::vector<uint8_t>& buffer,
                                    size_t offset) {
    switch (op) {
        case Protocol::Operation::AUTH: {
            if (buffer.size() - offset < 6) return 0;  // Минимум 6 байт

            uint16_t loginLen, passwordLen;
            std::memcpy(&loginLen, buffer.data() + offset + 2, sizeof(uint16_t));
            std::memcpy(&passwordLen, buffer.data() + offset + 4, sizeof(uint16_t));

            return 6 + loginLen + passwordLen;  // 2(op) + 2(loginLen) + 2(passwordLen) + login + password
        }
        case Protocol::Operation::REGISTRATION: {
            if (buffer.size() - offset < 6) return 0;

            uint16_t loginLen, passwordLen;
            std::memcpy(&loginLen, buffer.data() + offset + 2, sizeof(uint16_t));
            std::memcpy(&passwordLen, buffer.data() + offset + 4, sizeof(uint16_t));

            return 6 + loginLen + passwordLen;
        }
        case Protocol::Operation::GET_NOTES: {
            return 0;
//            break;
        }
        case Protocol::Operation::SYNC: {
            return 0;
        }
        case Protocol::Operation::CREATE_NOTE: {
            uint16_t titleLen;
            std::cout << "offset is: " << offset << std::endl;
            std::memcpy(&titleLen, buffer.data() + offset + 2 + 4, sizeof(uint16_t));
            std::cout << "Create note message size: " << 8 + titleLen << std::endl;
            return 8 + titleLen; // 2 (op) + 4 (user_id) + 2 (titleLen) + title;
        }
        default:
            std::cerr << "Unknown operation: " << static_cast<int>(op) << std::endl;
            return 2;  // Минимум код операции
    }
}

void SyncServer::processClientMessage(int client_fd, const std::vector<uint8_t>& message) {
    const Protocol::Operation op = Protocol::decodeOperation(message);
    std::cout << "Processing message with operation code: " << static_cast<int>(op) << std::endl;

    try {
        switch (op) {
            case Protocol::Operation::AUTH: {
                handleAuthRequest(client_fd, message);
                break;
            }
            case Protocol::Operation::REGISTRATION: {
                handleRegistrationRequest(client_fd, message);
                break;
            }
            case Protocol::Operation::GET_NOTES: {
                handleGetNotesRequest(client_fd, message);
                break;
            }
            case Protocol::Operation::SYNC: {
                handleSyncRequest(client_fd, message);
                break;
            }
            case Protocol::Operation::CREATE_NOTE: {
                handleCreateNoteRequest(client_fd, message);
                break;
            }

            default:
                std::cerr << "Unknown operation from client " << client_fd
                          << ": " << static_cast<uint16_t>(op) << std::endl;
                break;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error processing message from client " << client_fd
                  << ": " << e.what() << std::endl;
    }
}

void SyncServer::processClientMessages(int client_fd) {
    std::vector<uint8_t> buffer;
    char temp_buffer[4096];

    while (true) {
        ssize_t received = recv(client_fd, temp_buffer, sizeof(temp_buffer), 0);

        if (received <= 0) {
            if (received == 0) {
                std::cout << "Client " << client_fd << " disconnected" << std::endl;
            } else {
                perror("recv");
            }
            return;
        }

        buffer.insert(buffer.end(), temp_buffer, temp_buffer + received);
        std::cout << "Received " << received << " bytes from client " << client_fd
                  << ", total buffer size: " << buffer.size() << std::endl;

        size_t processed = 0;
        std::cout << "Buffer size - processed: ";
        std::cout << buffer.size() - processed << std::endl;
        while (buffer.size() - processed >= 2) {
            uint16_t opCode;
            std::memcpy(&opCode, buffer.data() + processed, sizeof(uint16_t));
            auto operation = static_cast<Protocol::Operation>(opCode);

            size_t messageLength = getMessageLength(operation, buffer, processed);

            if (messageLength == 0) {
                std::cerr << "Invalid message length for operation "
                          << static_cast<int>(operation) << std::endl;
                return;
            }

            if (buffer.size() - processed < messageLength) {
                break;  // Неполное сообщение
            }

            std::vector<uint8_t> message(
                    buffer.begin() + processed,
                    buffer.begin() + processed + messageLength
            );

            std::cout << "Before processing message with operation code: " << opCode << std::endl;
            processClientMessage(client_fd, message);
            processed += messageLength;
        }

        if (processed > 0) {
            buffer.erase(buffer.begin(), buffer.begin() + processed);
        }
    }
}

void SyncServer::handleClient(int client_fd) {
    std::cout << "New client connected: " << client_fd << std::endl;

    try {
        processClientMessages(client_fd);  // ← ИСПОЛЬЗУЕМ processClientMessages
    } catch (const std::exception& e) {
        std::cerr << "Error handling client " << client_fd << ": " << e.what() << std::endl;
    }

    close(client_fd);
    std::cout << "Connection with client " << client_fd << " closed" << std::endl;
}

void SyncServer::run() {
    m_server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_server_fd < 0) {
        perror("socket");
        return;
    }

    int opt = 1;
    setsockopt(m_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(m_port);
    inet_pton(AF_INET, m_host.c_str(), &server_addr.sin_addr);

    if (bind(m_server_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(m_server_fd);
        return;
    }

    if (listen(m_server_fd, SOMAXCONN) < 0) {
        perror("listen");
        close(m_server_fd);
        return;
    }

    std::cout << "Server started on " << m_host << ":" << m_port << std::endl;
    m_running = true;

    while (m_running) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);

        int client_fd = accept(m_server_fd, (sockaddr*)&client_addr, &client_len);

        if (client_fd < 0) {
            if (!m_running) break;
            perror("accept");
            continue;
        }

        std::cout << "Accepted connection from "
                  << inet_ntoa(client_addr.sin_addr) << ":"
                  << ntohs(client_addr.sin_port) << std::endl;

        m_client_threads.emplace_back(&SyncServer::handleClient, this, client_fd);
    }
}

void SyncServer::stop() {
    m_running = false;

    for (auto& thread : m_client_threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    m_client_threads.clear();

    if (m_server_fd >= 0) {
        close(m_server_fd);
        m_server_fd = -1;
    }

    std::cout << "Server stopped" << std::endl;
}