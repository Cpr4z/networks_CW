#include "Server.hpp"

#include <fcntl.h>
#include <cstring>
#include <iostream>

static bool sendAll(int fd, const uint8_t* data, size_t size) {
    while (size > 0) {
        ssize_t s = ::send(fd, data, size, 0);
        if (s <= 0) return false;
        data += (size_t)s;
        size -= (size_t)s;
    }
    return true;
}

Server::Server(int port, const std::string& host)
        : m_port(port), m_host(host) {
    m_repository = std::make_shared<Repository>();
    m_encryptionKey = "shared-secret-key-123";
}

Server::~Server() {
    stop();
}

void Server::setEncryptionKey(const std::string& key) {
    m_encryptionKey = key;
}

std::vector<uint8_t> Server::decryptClientData(int client_fd, const std::vector<uint8_t>& raw) {
    std::lock_guard<std::mutex> lock(m_clientDataMutex);
    auto it = m_clientData.find(client_fd);
    if (it == m_clientData.end() || !it->second.encryptor || raw.empty()) return raw;
    return it->second.encryptor->decrypt(raw);
}

std::vector<uint8_t> Server::encryptDataForClient(int client_fd, const std::vector<uint8_t>& plain) {
    std::lock_guard<std::mutex> lock(m_clientDataMutex);
    auto it = m_clientData.find(client_fd);
    if (it == m_clientData.end() || !it->second.encryptor || plain.empty()) return plain;
    return it->second.encryptor->encrypt(plain);
}

void Server::sendEncrypted(int client_fd, const std::vector<uint8_t>& plain) {
    auto payload = encryptDataForClient(client_fd, plain); // payload = ciphertext (пока)
    uint32_t len = htonl((uint32_t)payload.size());
    if (!sendAll(client_fd, reinterpret_cast<uint8_t*>(&len), sizeof(len))) {
        perror("send len");
        return;
    }
    if (!payload.empty() && !sendAll(client_fd, payload.data(), payload.size())) {
        perror("send payload");
        return;
    }
}

void Server::cleanupClientData(int client_fd) {
    std::lock_guard<std::mutex> lock(m_clientDataMutex);
    m_clientData.erase(client_fd);
}

void Server::broadcastToAllClients(int client_fd_, const std::vector<uint8_t>& data) {
    std::lock_guard<std::mutex> lock(m_clients_mutex);
    for (int client_fd : m_connected_clients) {
        if (client_fd != client_fd_) {
            sendEncrypted(client_fd, data);
        }
    }
}

void Server::sendAuthResponse(int client_fd, const Protocol::AuthResponse& response) {
    std::vector<uint8_t> response_buffer = Protocol::encodeAuthResponse(response);
    sendEncrypted(client_fd, response_buffer);
}

void Server::sendRegistrationResponse(int client_fd, const Protocol::RegistrationResponse& response) {
    std::vector<uint8_t> response_buffer = Protocol::encodeRegistrationResponse(response);
    sendEncrypted(client_fd, response_buffer);
}

void Server::sendGetNotesResponse(int client_fd, const Protocol::GetNotesResponse& response) {
    std::vector<uint8_t> response_buffer = Protocol::encodeGetNotesResponse(response);
    sendEncrypted(client_fd, response_buffer);
}

void Server::sendSyncResponse(int client_fd, const Protocol::SyncNoteResponse& response) {
    std::vector<uint8_t> response_buffer = Protocol::encodeSyncResponse(response);
    sendEncrypted(client_fd, response_buffer);
}

void Server::sendCreateNoteResponse(int client_fd, const Protocol::CreateNoteResponse& response) {
    std::vector<uint8_t> response_buffer = Protocol::encodeCreateNoteResponse(response);
    sendEncrypted(client_fd, response_buffer);
}

void Server::sendOpenNoteResponse(int client_fd, const Protocol::OpenNoteResponse& response) {
    std::vector<uint8_t> response_buffer = Protocol::encodeOpenNoteResponse(response);
    sendEncrypted(client_fd, response_buffer);
}

void Server::sendUpdateTextResponse(int client_fd, const Protocol::UpdateTextResponse& response) {
    std::vector<uint8_t> response_buffer = Protocol::encodeUpdateTextResponse(response);
    sendEncrypted(client_fd, response_buffer);
}

void Server::sendShareNoteNotifyRequest(int client_fd, const Protocol::ShareNoteNotifyRequest& request) {
    std::vector<uint8_t> notify_buffer = Protocol::encodeShareNoteNotifyRequest(request);
    broadcastToAllClients(client_fd, notify_buffer);
}

void Server::sendOwnerApproveMergeRequest(int client_fd, const Protocol::OwnerApproveMergeRequest& request) {
    std::vector<uint8_t> request_buffer = Protocol::encodeOwnerApproveMergeRequest(request);
    sendEncrypted(client_fd, request_buffer);
}

void Server::sendServerApproveMergeRequest(int client_fd, const Protocol::ServerApproveMergeRequest& request) {
    std::vector<uint8_t> request_buffer = Protocol::encodeServerApproveMergeRequest(request);
    sendEncrypted(client_fd, request_buffer);
}

void Server::sendUpdateTextMergedRequest(int client_fd, const Protocol::UpdateTextMergedRequest& request) {
    std::vector<uint8_t> request_buffer = Protocol::encodeUpdateTextMergedRequest(request);
    sendEncrypted(client_fd, request_buffer);
}

void Server::handleAuthRequest(int client_fd, const std::vector<uint8_t>& buffer) {
    const auto& request = Protocol::decodeAuthRequest(buffer);
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
    Protocol::RegistrationResponse response;
    response.op = Protocol::Operation::REGISTRATION;
    auto result = m_repository->validateUser(request->login, request->password);
    if (result) {
        response.status = 1;
        response.user_id = *result;
    } else {
        response.status = 0;
        response.user_id = m_repository->addUser(request->login, request->password);
        m_clients[static_cast<int>(response.user_id)] = client_fd;
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
    const auto& request = Protocol::decodeCreateNoteRequest(buffer);
    Protocol::CreateNoteResponse response;
    response.op = Protocol::Operation::CREATE_NOTE;
    if (m_repository->isNoteExists(request->user_id, request->note_title)) {
        response.status = 1;
        response.note_id = 0;
    } else {
        response.status = 0;
        auto result = m_repository->addNote(request->user_id, request->note_title);
        response.note_id = result.first;
        response.note_title = request->note_title;
        response.version = result.second;
    }
    sendCreateNoteResponse(client_fd, response);
}

void Server::handleOpenNoteRequest(int client_fd, const std::vector<uint8_t>& buffer) {
    const auto& request = Protocol::decodeOpenNoteRequest(buffer);
    Protocol::OpenNoteResponse response;
    response.op = Protocol::Operation::OPEN_NOTE;
    if (m_repository->isNoteExists(request->user_id, request->note_id)) {
        response.status = 0;
        response.note_id = request->note_id;
        response.text = m_repository->getNoteText(request->user_id, request->note_id);
        response.version = m_repository->getNoteVersion(request->user_id, request->note_id);
    } else {
        response.status = 1;
    }
    sendOpenNoteResponse(client_fd, response);
}

void Server::handleUpdateTextRequest(int client_fd, const std::vector<uint8_t>& buffer) {
    const auto& request = Protocol::decodeUpdateTextRequest(buffer);
    Protocol::UpdateTextResponse response;
    response.op = Protocol::Operation::UPDATE_TEXT;
    if (m_repository->isContainsConflict(request->user_id, request->note_id, request->version)) {
        response.status = 1;
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
    m_repository->shareNoteToAllUsers(req->user_id, req->note_id);
    Protocol::ShareNoteNotifyRequest request;
    request.op = Protocol::Operation::SHARE_NOTE_NOTIFY;
    request.note_id = req->note_id;
    request.owner_id = req->user_id;
    const auto& [title, text, version] = m_repository->getNoteInfo(req->note_id, req->user_id);
    request.note_title = title;
    request.version = version;
    sendShareNoteNotifyRequest(client_fd, request);
}

void Server::handleApproveMergeRequest(int client_fd, const std::vector<uint8_t>& buffer) {
    const auto& req = Protocol::decodeApproveMergeRequest(buffer);
    // status 0 -> owner approve
    if (req->status == 0) {
        Id note_owner_id = m_repository->getOwnerId(req->note_id);
        int client_fd_owner = m_clients[static_cast<int>(note_owner_id)];
        // посылаем запрос владельцу заметки для того, чтобы он одобрил merge request
        Protocol::OwnerApproveMergeRequest request;
        request.note_id = req->note_id;
        request.approve_text = req->merged_text;
        request.merge_sender_id = req->user_id;
        sendOwnerApproveMergeRequest(client_fd_owner, request);
    }
    // status 1 -> server approve
    else if (req->status == 1) {
        Protocol::ServerApproveMergeRequest request;
        request.note_id = req->note_id;
        const auto& [version, server_text] = m_repository->getOwnerDataVersion(req->note_id);
        request.version = version;
        request.server_version = server_text;
        sendServerApproveMergeRequest(client_fd, request);
    }
}

void Server::handleOwnerApproveMergeResponse(int client_fd, const std::vector<uint8_t>& buffer) {
    const auto& response = Protocol::decodeOwnerApproveMergeResponse(buffer);
    // владелец заметки принял изменения, нужно обновить данную заметку для всех пользователей
    // и разослать запросы с обновлением текста, если она у них открыта
    if (response->status == 0) {
        Protocol::UpdateTextMergedRequest request;
        request.note_id = response->note_id;
        request.version = response->new_version;
        request.merged_version = response->approved_text;
        const auto& user_to_send = m_repository->getNoteUsers(response->note_id);
        m_repository->updateNoteAfterMerge(response->note_id, response->new_version, response->approved_text);
        std::vector<int> fd_clients;
        fd_clients.reserve(user_to_send.size());
        for (const uint32_t client : user_to_send) {
            fd_clients.emplace_back(m_clients[static_cast<int>(client)]);
        }

        for (const uint32_t fd_client : fd_clients) {
            sendUpdateTextMergedRequest(static_cast<int>(fd_client), request);
        }
    }
    // владелец заметки отклонил изменения, нужно отправить создателю merge request запрос на то, что измненеия отклонены
    else if (response->status == 1) {
        Protocol::UpdateTextMergedRequest request;
        request.note_id = response->note_id;
        const auto& [owner_version, owner_text] = m_repository->getOwnerDataVersion(response->note_id);
        request.merged_version = owner_text;
        request.version = owner_version;
        int fd_merge_sender = m_clients[static_cast<int>(response->sender_id)];
        sendUpdateTextMergedRequest(fd_merge_sender, request);
    }
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
            std::memcpy(&titleLen, buffer.data() + offset + 2 + 4, sizeof(uint16_t));
            return 8 + titleLen;
        }
        case Protocol::Operation::OPEN_NOTE: {
            return 2 + 4 + 4;
        }
        case Protocol::Operation::UPDATE_TEXT: {
            uint32_t textLen;
            std::memcpy(&textLen, buffer.data() + offset + 2 + 4 + 4 + 4, sizeof(textLen));
            return 2 + 4 + 4 + 4 + 4 + textLen;
        }
        case Protocol::Operation::SHARE_NOTE: {
            return 2 + 4 + 4;
        }
        case Protocol::Operation::APPROVE_MERGE: {
            uint32_t textLen;
            std::memcpy(&textLen, buffer.data() + offset + 2 + 4 + 4 + 1, sizeof(textLen));
            return 2 + 4 + 4 + 4 + 1 + textLen;
        }
        case Protocol::Operation::OWNER_APPROVE_MERGE: {
            uint32_t textLen;
            std::memcpy(&textLen, buffer.data() + offset + 2 + 4 + 4 + 4 + 1, sizeof(textLen));
            return 2 + 4 + 4 + 4 + 1 + 4 + textLen;
        }
        default:
            std::cerr << "Unknown operation: " << static_cast<int>(op) << std::endl;
            return 2;
    }
}

void Server::processClientMessage(int client_fd, const std::vector<uint8_t>& message) {
    const Protocol::Operation op = Protocol::decodeOperation(message);
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
            case Protocol::Operation::OWNER_APPROVE_MERGE: {
                handleOwnerApproveMergeResponse(client_fd, message);
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
    uint8_t tmp[4096];

    while (true) {
        ssize_t received = recv(client_fd, tmp, sizeof(tmp), 0);
        if (received <= 0) return;
        {
            std::lock_guard<std::mutex> lk(m_clientDataMutex);
            m_clientData[client_fd].buffer.insert(
                    m_clientData[client_fd].buffer.end(),
                    tmp, tmp + received
            );
        }

        while (true) {
            std::vector<uint8_t> payload;

            {
                std::lock_guard<std::mutex> lk(m_clientDataMutex);
                auto &buf = m_clientData[client_fd].buffer;

                if (buf.size() < 4) break;

                uint32_t len_be;
                std::memcpy(&len_be, buf.data(), 4);
                uint32_t len = ntohl(len_be);
                if (len == 0 || len > 10 * 1024 * 1024) {
                    std::cerr << "Bad frame length: " << len << std::endl;
                    buf.clear();
                    break;
                }

                if (buf.size() < 4 + len) break;

                payload.assign(buf.begin() + 4, buf.begin() + 4 + len);
                buf.erase(buf.begin(), buf.begin() + 4 + len);
            }
            auto plain = decryptClientData(client_fd, payload);
            if (plain.size() < 2) continue;

            processClientMessage(client_fd, plain);
        }
    }
}

void Server::handleClient(int client_fd) {
    {
        std::lock_guard<std::mutex> lock(m_clients_mutex);
        m_connected_clients.insert(client_fd);
    }

    {
        std::lock_guard<std::mutex> lock(m_clientDataMutex);
        auto& cd = m_clientData[client_fd];
        cd.encryptor = std::make_unique<Encryptor>(m_encryptionKey);
    }

    try {
        processClientMessages(client_fd);
    } catch (const std::exception& e) {
        std::cerr << "Error handling client " << client_fd << ": " << e.what() << std::endl;
    }

    cleanupClientData(client_fd);
    close(client_fd);

    {
        std::lock_guard<std::mutex> lock(m_clients_mutex);
        m_connected_clients.erase(client_fd);
    }
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
        m_client_threads.emplace_back(&Server::handleClient, this, client_fd);
    }
}

void Server::stop() {
    m_running = false;

    if (m_server_fd >= 0) {
        shutdown(m_server_fd, SHUT_RDWR);
        close(m_server_fd);
        m_server_fd = -1;
    }

    {
        std::lock_guard<std::mutex> lk(m_clients_mutex);
        for (int fd : m_connected_clients) {
            if (fd >= 0) {
                shutdown(fd, SHUT_RDWR);
                close(fd);
            }
        }
        m_connected_clients.clear();
    }

    for (auto& t : m_client_threads) {
        if (t.joinable()) t.join();
    }
    m_client_threads.clear();
}