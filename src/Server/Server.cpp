#include "Server.hpp"
#include <fcntl.h>
#include <cstring>

Server::Server(int port, const std::string& host)
        : m_port(port), m_host(host) {
    m_repository = std::make_shared<Repository>();
}

Server::~Server() {
    stop();
}

void Server::broadcastToAllClients(int client_fd_, const std::vector<uint8_t>& data) {
    std::lock_guard<std::mutex> lock(m_clients_mutex);
    for (int client_fd : m_connected_clients) {
        if (client_fd != client_fd_) {
            ssize_t sent = send(client_fd, data.data(), data.size(), 0);
            if (sent < 0) {
                perror("broadcast send");
            } else {
                std::cout << "  Sent to client " << client_fd << std::endl;
            }
        }
    }
}

void Server::sendAuthResponse(int client_fd, const Protocol::AuthResponse& response) {
    std::vector<uint8_t> response_buffer = Protocol::encodeAuthResponse(response);
    ssize_t sent = send(client_fd, response_buffer.data(), response_buffer.size(), 0);
    if (sent < 0) {
        perror("send");
    } else {
        std::cout << "Sent response to client " << client_fd << std::endl;
    }
}

void Server::sendRegistrationResponse(int client_fd, const Protocol::RegistrationResponse& response) {
    std::vector<uint8_t> response_buffer = Protocol::encodeRegistrationResponse(response);
    ssize_t sent = send(client_fd, response_buffer.data(), response_buffer.size(), 0);
    if (sent < 0) {
        perror("send");
    } else {
        std::cout << "Sent response to client " << client_fd << std::endl;
    }
}

void Server::sendGetNotesResponse(int client_fd, const Protocol::GetNotesResponse& response) {
    std::vector<uint8_t> response_buffer = Protocol::encodeGetNotesResponse(response);
    ssize_t sent = send(client_fd, response_buffer.data(), response_buffer.size(), 0);
    if (sent < 0) {
        perror("send");
    } else {
        std::cout << "Sent response to client " << client_fd << std::endl;
    }
}

void Server::sendSyncResponse(int client_fd, const Protocol::SyncNoteResponse& response) {
    std::vector<uint8_t> response_buffer = Protocol::encodeSyncResponse(response);
    ssize_t sent = send(client_fd, response_buffer.data(), response_buffer.size(), 0);
    if (sent < 0) {
        perror("send");
    } else {
        std::cout << "Sent response to client " << client_fd << std::endl;
    }
}

void Server::sendCreateNoteResponse(int client_fd, const Protocol::CreateNoteResponse& response) {
    std::vector<uint8_t> response_buffer = Protocol::encodeCreateNoteResponse(response);
    std::cout << "After data encoded for Note Response" << std::endl;
    ssize_t sent = send(client_fd, response_buffer.data(), response_buffer.size(), 0);
    if (sent < 0) {
        perror("send");
    } else {
        std::cout << "Sent response to client " << client_fd << std::endl;
    }
}

void Server::sendOpenNoteResponse(int client_fd, const Protocol::OpenNoteResponse& response) {
    std::vector<uint8_t> response_buffer = Protocol::encodeOpenNoteResponse(response);
    std::cout << "After data encoded for Note Response" << std::endl;
    ssize_t sent = send(client_fd, response_buffer.data(), response_buffer.size(), 0);
    if (sent < 0) {
        perror("send");
    } else {
        std::cout << "Sent response to client " << client_fd << std::endl;
    }
}

void Server::sendUpdateTextResponse(int client_fd, const Protocol::UpdateTextResponse& response) {
    std::vector<uint8_t> response_buffer = Protocol::encodeUpdateTextResponse(response);
    ssize_t sent = send(client_fd, response_buffer.data(), response_buffer.size(), 0);
    if (sent < 0) {
        perror("send");
    } else {
        std::cout << "Sent response to client " << client_fd << std::endl;
    }
}

void Server::sendShareNoteResponse(int client_fd, const Protocol::ShareNoteResponse& response) {

}

void Server::sendShareNoteNotifyRequest(int client_fd, const Protocol::ShareNoteNotifyRequest& request) {
    std::vector<uint8_t> notify_buffer = Protocol::encodeShareNoteNotifyRequest(request);
    broadcastToAllClients(client_fd, notify_buffer);
}

void Server::sendOwnerApproveMergeRequest(int client_fd, const Protocol::OwnerApproveMergeRequest& request) {
    std::vector<uint8_t> request_buffer = Protocol::encodeOwnerApproveMergeRequest(request);
    ssize_t sent = send(client_fd, request_buffer.data(), request_buffer.size(), 0);
    if (sent < 0) {
        perror("send");
    } else {
        std::cout << "Sent response to client " << client_fd << std::endl;
    }
}

void Server::sendApproveMergeResponse(int client_fd, const Protocol::ApproveMergeResponse& response) {
    std::vector<uint8_t> response_buffer = Protocol::encodeApproveMergeResponse(response);
    ssize_t sent = send(client_fd, response_buffer.data(), response_buffer.size(), 0);
    if (sent < 0) {
        perror("send");
    } else {
        std::cout << "Sent response to client " << client_fd << std::endl;
    }
}

void Server::sendOwnerApproveMergeResponse(int client_fd, const Protocol::OwnerApproveMergeResponse& response) {
    std::vector<uint8_t> response_buffer = Protocol::encodeOwnerApproveMergeResponse(response);
    ssize_t sent = send(client_fd, response_buffer.data(), response_buffer.size(), 0);
    if (sent < 0) {
        perror("send");
    } else {
        std::cout << "Sent response to client " << client_fd << std::endl;
    }
}

void Server::handleAuthRequest(int client_fd, const std::vector<uint8_t>& buffer) {
    const auto& request = Protocol::decodeAuthRequest(buffer);
    std::cout << "Auth request from client " << client_fd << std::endl;
    std::cout << request->login << std::endl;
    std::cout << request->password << std::endl;

    Protocol::AuthResponse response;
    response.op = Protocol::Operation::AUTH;
    auto result = m_repository->validateUser(request->login, request->password);
    if (result) {
        response.status = 0;
        response.user_id = *result;
    } else {
        response.user_id = 0;
        response.status = static_cast<uint8_t>(result.error());
    }

    sendAuthResponse(client_fd, response);
}

void Server::handleRegistrationRequest(int client_fd, const std::vector<uint8_t>& buffer) {
    const auto& request = Protocol::decodeRegistrationRequest(buffer);
    std::cout << "Registration request from client " << client_fd << std::endl;
    std::cout << request->login << std::endl;
    std::cout << request->password << std::endl;
    Protocol::RegistrationResponse response;

    response.op = Protocol::Operation::REGISTRATION;
    auto result = m_repository->validateUser(request->login, request->password);
    if (result) {
        response.status = 1;
        response.user_id = *result;
    } else {
        std::cout << "Registration: user not found, adding new user" << std::endl;
        response.status = 0;
        response.user_id = m_repository->addUser(request->login, request->password);
        m_clients[static_cast<int>(response.user_id)] = client_fd;
        std::cout << "Registration status is: " << static_cast<int>(response.status) << std::endl;
        std::cout << "New user id is: " << response.user_id << std::endl;
        std::cout << "m_clients map value by key " << static_cast<int>(response.user_id) << "is: " << m_clients[static_cast<int>(response.user_id)] << std::endl;
    }
    sendRegistrationResponse(client_fd, response);
}

void Server::handleGetNotesRequest(int client_fd, const std::vector<uint8_t>& buffer) {
    const auto& request = Protocol::decodeGetNotesRequest(buffer);

    Protocol::GetNotesResponse response;
    response.op = Protocol::Operation::GET_NOTES;
    response.notes = m_repository->getNotesForUser(request->user_id);
    sendGetNotesResponse(client_fd, response);
}

void Server::handleSyncRequest(int client_fd, const std::vector<uint8_t>& buffer) {
    const auto& request = Protocol::decodeSyncRequest(buffer);
    const auto& [_, text, version] = m_repository->getNoteInfoToSync(request->note_id, request->user_id);

    Protocol::SyncNoteResponse response;
    response.op = Protocol::Operation::SYNC;
    response.text = text;
    response.version = version;

    sendSyncResponse(client_fd, response);
}

void Server::handleCreateNoteRequest(int client_fd, const std::vector<uint8_t>& buffer) {
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
        auto result = m_repository->addNote(request->user_id, request->note_title);
        response.note_id = result.first;
        response.note_title = request->note_title;
        response.version = result.second;
        std::cout << "After adding new note with title:" << response.note_title << " and id: " << response.note_id << std::endl;
    }

    sendCreateNoteResponse(client_fd, response);
}

void Server::handleOpenNoteRequest(int client_fd, const std::vector<uint8_t>& buffer) {
    const auto& request = Protocol::decodeOpenNoteRequest(buffer);

    Protocol::OpenNoteResponse response;
    response.op = Protocol::Operation::OPEN_NOTE;

    if (m_repository->isNoteExists(request->user_id, request->note_id)) {
        std::cout << "Note exits when trying to open note" << std::endl;
        response.status = 0;
        response.note_id = request->note_id;
        response.text = m_repository->getNoteText(request->user_id, request->note_id);
        response.version = m_repository->getNoteVersion(request->user_id, request->note_id);
    } else {
        std::cout << "Note doesn't exist when trying to open note" << std::endl;
        response.status = 1;
    }

    sendOpenNoteResponse(client_fd, response);
}

void Server::handleUpdateTextRequest(int client_fd, const std::vector<uint8_t>& buffer) {
    const auto& request = Protocol::decodeUpdateTextRequest(buffer);
    std::cout << "Update note request" << std::endl;
    std::cout << "User with id - " << request->user_id << std::endl;
    std::cout << "What to update note with id - " << request->note_id << std::endl;
    std::cout << "New text for this note is: " << request->text << std::endl;
    Protocol::UpdateTextResponse response;
    response.op = Protocol::Operation::UPDATE_TEXT;

    if (m_repository->isContainsConflict(request->user_id, request->note_id, request->version)) {
        response.status = 1;
        std::cerr << "we have conflict after update request" << std::endl;
    } else {
        response.note_id = request->note_id;
        response.status = 0;
        m_repository->updateNoteText(request->note_id, request->user_id, request->text);
        response.version = m_repository->getNoteVersion(request->user_id, request->note_id);
    }
    m_repository->updateVersionForSharedNotes(request->user_id, request->note_id);
    sendUpdateTextResponse(client_fd, response);
}

void Server::handleShareNoteRequest(int client_fd, const std::vector<uint8_t>& buffer) {
    const auto& req = Protocol::decodeShareNoteRequest(buffer);
    std::cout << "User with id: " << req->user_id << std::endl;
    std::cout << "What to share note with id: " << req->note_id << std::endl;

    m_repository->shareNoteToAllUsers(req->user_id, req->note_id);

    Protocol::ShareNoteNotifyRequest request;
    request.op = Protocol::Operation::SHARE_NOTE_NOTIFY;
    request.note_id = req->note_id;
    request.owner_id = req->user_id;

    const auto& [title, text, version] = m_repository->getNoteInfo(req->note_id, req->user_id);

    request.note_title = title;
    request.version = version;
//    request.version = m_repository->getNoteVersion(req->user_id, req->note_id);

    sendShareNoteNotifyRequest(client_fd, request);
}

void Server::handleApproveMergeRequest(int client_fd, const std::vector<uint8_t>& buffer) {
    const auto& req = Protocol::decodeApproveMergeRequest(buffer);
    std::cout << "Got approve merge request" << std::endl;
    Id note_owner_id = m_repository->getOwnerId(req->note_id);
    std::cout << "Note owner id is: " << note_owner_id << std::endl;
    int client_fd_owner = m_clients[static_cast<int>(note_owner_id)];

    // посылаем запрос владельцу заметки для того, чтобы он одобрил merge request
    Protocol::OwnerApproveMergeRequest request;
    request.note_id = req->note_id;
    request.approve_text = req->merged_text;
    request.merge_sender_id = req->user_id;

//    Protocol::ApproveMergeResponse response;
//    response.op = Protocol::Operation::APPROVE_MERGE;
//    response.new_text = req->merged_text;
//    response.note_id = req->note_id;
    // отправляем запрос владельцу заметки, по идее нужно отправить айдишник отправителя, чтобы потом можно было послать ему же ответ
    sendOwnerApproveMergeRequest(client_fd_owner, request);
}

void Server::handleOwnerApproveMergeRequest(int client_fd, const std::vector<uint8_t>& buffer) {
    const auto& req = Protocol::decodeOwnerApproveMergeRequest(buffer);

    Protocol::OwnerApproveMergeResponse response;

    sendOwnerApproveMergeResponse(client_fd, response);
}

void Server::handleShareNoteNotifyRequest(int client_fd, const std::vector<uint8_t>& buffer) {
}

size_t Server::getMessageLength(Protocol::Operation op,
                                    const std::vector<uint8_t>& buffer,
                                    size_t offset) {
    switch (op) {
        case Protocol::Operation::AUTH: {
            if (buffer.size() - offset < 6) return 0;

            uint16_t loginLen, passwordLen;
            std::memcpy(&loginLen, buffer.data() + offset + 2, sizeof(uint16_t));
            std::memcpy(&passwordLen, buffer.data() + offset + 4, sizeof(uint16_t));

            return 6 + loginLen + passwordLen;
        }
        case Protocol::Operation::REGISTRATION: {
            if (buffer.size() - offset < 6) return 0;

            uint16_t loginLen, passwordLen;
            std::memcpy(&loginLen, buffer.data() + offset + 2, sizeof(uint16_t));
            std::memcpy(&passwordLen, buffer.data() + offset + 4, sizeof(uint16_t));

            return 6 + loginLen + passwordLen;
        }
        case Protocol::Operation::GET_NOTES: {
            return 2 + 4;
        }
        case Protocol::Operation::SYNC: {
            return 2 + 4 + 4;
        }
        case Protocol::Operation::CREATE_NOTE: {
            uint16_t titleLen;
            std::cout << "offset is: " << offset << std::endl;
            std::memcpy(&titleLen, buffer.data() + offset + 2 + 4, sizeof(uint16_t));
            std::cout << "Create note message size: " << 8 + titleLen << std::endl;
            return 8 + titleLen;
        }
        case Protocol::Operation::OPEN_NOTE: {
            return 2 + 4 + 4;
        }
        case Protocol::Operation::UPDATE_TEXT: {
            uint32_t textLen;
            std::memcpy(&textLen, buffer.data() + offset + 2 + 4 + 4 + 4, sizeof(textLen));
            std::cout << "Update note text message size: " << 2 + 4 + 4 + 4 + 4 + textLen << std::endl;
            return 2 + 4 + 4 + 4 + 4 + textLen;
        }
        case Protocol::Operation::SHARE_NOTE: {
            return 2 + 4 + 4;
        }
        case Protocol::Operation::APPROVE_MERGE: {
//            uint32_t note_id;
//            uint32_t user_id;
//            uint8_t type;
//            uint8_t status;
//            std::string merged_text;
            uint32_t textLen;
            std::memcpy(&textLen, buffer.data() + offset + 2 + 4 + 4 + 1 + 1, sizeof(textLen));
            return 2 + 4 + 4 + 4 + 1 + 1 + textLen;
        }
        case Protocol::Operation::OWNER_APPROVE_MERGE: {
            return 0;
        }
        default:
            std::cerr << "Unknown operation: " << static_cast<int>(op) << std::endl;
            return 2;
    }
}

void Server::processClientMessage(int client_fd, const std::vector<uint8_t>& message) {
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
            case Protocol::Operation::OPEN_NOTE: {
                handleOpenNoteRequest(client_fd, message);
                break;
            }
            case Protocol::Operation::UPDATE_TEXT: {
                handleUpdateTextRequest(client_fd, message);
                break;
            }
            case Protocol::Operation::SHARE_NOTE: {
                handleShareNoteRequest(client_fd, message);
                break;
            }
            case Protocol::Operation::APPROVE_MERGE: {
                handleApproveMergeRequest(client_fd, message);
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

void Server::processClientMessages(int client_fd) {
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
                break;
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

void Server::handleClient(int client_fd) {
    std::cout << "New client connected: " << client_fd << std::endl;

    {
        std::lock_guard<std::mutex> lock(m_clients_mutex);
        m_connected_clients.insert(client_fd);
    }

    try {
        processClientMessages(client_fd);
    } catch (const std::exception& e) {
        std::cerr << "Error handling client " << client_fd << ": " << e.what() << std::endl;
    }

    close(client_fd);
    std::cout << "Connection with client " << client_fd << " closed" << std::endl;
}

void Server::run() {
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

        m_client_threads.emplace_back(&Server::handleClient, this, client_fd);
    }
}

void Server::stop() {
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