#include "Client.hpp"

#include <arpa/inet.h>

namespace {
    std::string mapAuthErrorCode(int code) {
        switch(code) {
            case 1:
                return "Пользователь не найден";
            case 2:
                return "Неверный пароль";
            default:
                return "Неизвестная ошибка";
        }
    }
}

NoteClient::NoteClient(QObject *parent) : QObject(parent) {
    connect(&m_socket, &QTcpSocket::readyRead, this, &NoteClient::onReadyRead);
#ifdef __APPLE__
    m_socket.connectToHost("127.0.0.1", 8080);
#elif __linux__
    m_socket.connectToHost("192.168.64.4", 8080);
#endif

}

void NoteClient::setEncryptionKey(const QString& password) {
    if (!password.isEmpty()) {
        m_encryptor = std::make_unique<Encryptor>(password.toStdString());
        m_encryptionEnabled = true;
    }
}

std::vector<uint8_t> NoteClient::encryptData(const std::vector<uint8_t>& data) {
    if (!m_encryptionEnabled || !m_encryptor) {
        return data;
    }

    try {
        return m_encryptor->encrypt(data);
    } catch (const std::exception& e) {
        qWarning() << "Encryption failed:" << e.what();
        return data;
    }
}

std::vector<uint8_t> NoteClient::decryptData(const std::vector<uint8_t>& data) {
    if (!m_encryptionEnabled || !m_encryptor || data.empty()) {
        return data;
    }

    try {
        return m_encryptor->decrypt(data);
    } catch (const std::exception& e) {
        qWarning() << "Decryption failed:" << e.what();
        return data;
    }
}

void NoteClient::sendEncryptedData(const std::vector<uint8_t>& plain) {
    auto payload = encryptData(plain);
    uint32_t len = htonl((uint32_t)payload.size());

    m_socket.write(reinterpret_cast<const char*>(&len), 4);
    if (!payload.empty()) {
        m_socket.write(reinterpret_cast<const char*>(payload.data()),
                       (qint64)payload.size());
    }
    m_socket.flush();
}

void NoteClient::sendAuthRequest(const QString& login, const QString& password) {
    Protocol::AuthRequest req = { login.toStdString(), password.toStdString() };
    std::vector<uint8_t> requestData = Protocol::encodeAuthRequest(req);
    sendEncryptedData(requestData);
}

void NoteClient::sendRegistrationRequest(const QString& login, const QString& password) {
    Protocol::RegistrationRequest req = { login.toStdString(), password.toStdString()};
    std::vector<uint8_t> requestData = Protocol::encodeRegistrationRequest(req);
    sendEncryptedData(requestData);
}

void NoteClient::sendUpdateTextRequest(uint32_t note_id, const QString& text, uint32_t version) {
    Protocol::UpdateTextRequest req = { m_user_id, note_id, version, text.toStdString()};
    std::vector<uint8_t> requestData = Protocol::encodeUpdateTextRequest(req);
    sendEncryptedData(requestData);
}

void NoteClient::sendGetNotesRequest() {
    Protocol::GetNotesRequest req = { m_user_id };
    std::vector<uint8_t> requestData = Protocol::encodeGetNotesRequest(req);
    sendEncryptedData(requestData);
}

void NoteClient::sendCreateNoteRequest(const QString& title) {
    Protocol::CreateNoteRequest req = { m_user_id, title.toStdString() };
    std::vector<uint8_t> requestData = Protocol::encodeCreateNoteRequest(req);
    sendEncryptedData(requestData);
}

void NoteClient::sendOpenNoteRequest(uint32_t note_id, uint32_t version) {
    Protocol::OpenNoteRequest req = {note_id, m_user_id, version};
    std::vector<uint8_t> requestData = Protocol::encodeOpenNoteRequest(req);
    sendEncryptedData(requestData);
}

void NoteClient::sendShareNoteRequest(uint32_t note_id, uint32_t version) {
    Protocol::ShareNoteRequest req = {m_user_id, note_id, version};
    std::vector<uint8_t> requestData = Protocol::encodeShareNoteRequest(req);
    sendEncryptedData(requestData);
}

void NoteClient::sendSyncNoteRequest(uint32_t note_id) {
    Protocol::SyncNoteRequest req = {note_id, m_user_id};
    std::vector<uint8_t> requestData = Protocol::encodeSyncRequest(req);
    sendEncryptedData(requestData);
}

void NoteClient::sendApproveMergeRequest(uint32_t noteId, uint8_t status, const QString& merged_version) {
    Protocol::ApproveMergeRequest req = { noteId, m_user_id, status, merged_version.toStdString() };
    std::vector<uint8_t> requestData = Protocol::encodeApproveMergeRequest(req);
    sendEncryptedData(requestData);
}

void NoteClient::sendOwnerApproveMergeResponse(uint32_t noteId, uint32_t new_version, uint32_t merge_sender_id, uint8_t result_code, const QString& approved_version) {
    Protocol::OwnerApproveMergeResponse resp = {Protocol::Operation::OWNER_APPROVE_MERGE, noteId, merge_sender_id, new_version, result_code, approved_version.toStdString()};
    std::vector<uint8_t> requestData = Protocol::encodeOwnerApproveMergeResponse(resp);
    sendEncryptedData(requestData);
}

void NoteClient::handleAuthResponse(const std::vector<uint8_t>& buffer) {
    const auto& response_opt = Protocol::decodeAuthResponse(buffer);
    if (response_opt.has_value()) {
        const auto& response = response_opt.value();
        if (response.status == 0) {
            m_user_id = response.user_id;
            emit authSuccess(response.user_id);
            return;
        } else {
            emit authFailed(QString::fromStdString(mapAuthErrorCode(response.status)));
            return;
        }
    } else {
        emit authFailed("Error while handling response from server");
    }
}

void NoteClient::handleRegistrationResponse(const std::vector<uint8_t>& buffer) {
    const auto& response_opt = Protocol::decodeRegistrationResponse(buffer);
    if (response_opt.has_value()) {
        const auto& response = response_opt.value();
        if (static_cast<int>(response.status) == 0) {
            emit registrationSuccess(response.user_id);
        } else {
            emit registrationFailed(QString::fromStdString("Пользователь с таким именем уже зарегистрирован"));
        }
    } else {
        emit registrationFailed("Error while handling response from server");
    }
}

void NoteClient::handleSyncResponse(const std::vector<uint8_t>& buffer) {
    const auto& response_opt = Protocol::decodeSyncResponse(buffer);
    if (response_opt.has_value()) {
        const auto& response = response_opt.value();
        emit createSyncDialog(QString::fromStdString(response.text));
    }
}

void NoteClient::handleGetNotesResponse(const std::vector<uint8_t>& buffer) {
    const auto& response_opt = Protocol::decodeGetNotesResponse(buffer);
    if (response_opt.has_value()) {
        const auto& response = response_opt.value();
        QMap<uint32_t, std::tuple<QString, bool, uint32_t, uint32_t>> qtNotes;
        for (const auto& [id, note_info] : response.notes) {
            const auto& [title, is_shared, version, owner_id] = note_info;
            qtNotes.insert(id, std::make_tuple(QString::fromStdString(title), is_shared, version, owner_id));
        }
        emit getNotes(qtNotes);
    }
}

void NoteClient::handleCreateNoteResponse(const std::vector<uint8_t>& buffer) {
    const auto& response_opt = Protocol::decodeCreateNoteResponse(buffer);
    if (response_opt.has_value()) {
        const auto& response = response_opt.value();
        if (response.status == 0) {
            emit noteCreationSuccess(QString::fromStdString(response.note_title), response.note_id, m_user_id, response.version);
        } else {
            emit noteCreationFailed("Заметка с таким именем уже существует");
        }
    }
}

void NoteClient::handleOpenNoteResponse(const std::vector<uint8_t>& buffer) {
    const auto& response_opt = Protocol::decodeOpenNoteResponse(buffer);
    if (response_opt.has_value()) {
        const auto& response = response_opt.value();
        if (response.status == 0) {
            emit noteOpenSuccess(response.note_id, response.version, QString::fromStdString(response.text));
        } else {
            emit noteOpenFailed(response.note_id, "Не удалось открыть заметку");
        }
    }
}

void NoteClient::handleUpdateTextResponse(const std::vector<uint8_t>& buffer) {
    const auto& response_opt = Protocol::decodeUpdateTextResponse(buffer);
    if (response_opt.has_value()) {
        const auto& response = response_opt.value();
        if (response.status == 0) {
            emit updateTextSuccess(response.note_id, m_user_id, response.version);
        } else {
            emit updateTextFailed(response.status);
        }
    }
}

void NoteClient::handleShareNoteNotifyRequest(const std::vector<uint8_t>& buffer) {
    const auto& request_opt = Protocol::decodeShareNoteNotifyRequest(buffer);
    if (request_opt.has_value()) {
        const auto& request = request_opt.value();
        emit createSharedNote(request.note_id, QString::fromStdString(request.note_title), request.owner_id, request.version);
    }
}

void NoteClient::handleOwnerApproveMergeRequest(const std::vector<uint8_t>& buffer) {
    const auto& request_opt = Protocol::decodeOwnerApproveMergeRequest(buffer);
    if (request_opt.has_value()) {
        const auto& request = request_opt.value();
        emit createOwnerApproveDialog(request.note_id, request.merge_sender_id, QString::fromStdString(request.approve_text));
    }
}

void NoteClient::handleServerApproveMergeRequest(const std::vector<uint8_t>& buffer) {
    const auto& request_opt = Protocol::decodeServerApproveMergeRequest(buffer);
    if (request_opt.has_value()) {
        const auto& request = request_opt.value();
        emit approveServerVersion(request.note_id, request.version, QString::fromStdString(request.server_version));
    }
}

void NoteClient::handleUpdateTextMergedRequest(const std::vector<uint8_t>& buffer) {
    const auto& request_opt = Protocol::decodeUpdateTextMergedRequest(buffer);
    if (request_opt.has_value()) {
        const auto& request = request_opt.value();
        emit updateTextMerged(request.note_id, request.version, QString::fromStdString(request.merged_version));
    }
}

void NoteClient::onReadyRead() {
    QByteArray chunk = m_socket.readAll();
    m_inBuffer.insert(m_inBuffer.end(), chunk.begin(), chunk.end());

    while (true) {
        if (m_inBuffer.size() < 4) return;

        uint32_t len_be;
        std::memcpy(&len_be, m_inBuffer.data(), 4);
        uint32_t len = ntohl(len_be);

        if (len == 0 || len > 10 * 1024 * 1024) {
            m_inBuffer.clear();
            return;
        }

        if (m_inBuffer.size() < 4 + len) return;

        std::vector<uint8_t> payload(m_inBuffer.begin() + 4, m_inBuffer.begin() + 4 + len);
        m_inBuffer.erase(m_inBuffer.begin(), m_inBuffer.begin() + 4 + len);

        auto decrypted = decryptData(payload);
        if (decrypted.size() < 2) continue;

        Protocol::Operation op = Protocol::decodeOperation(decrypted);
        switch (op) {
            case Protocol::Operation::AUTH: handleAuthResponse(decrypted); break;
            case Protocol::Operation::REGISTRATION: handleRegistrationResponse(decrypted); break;
            case Protocol::Operation::SYNC: handleSyncResponse(decrypted); break;
            case Protocol::Operation::GET_NOTES: handleGetNotesResponse(decrypted); break;
            case Protocol::Operation::CREATE_NOTE: handleCreateNoteResponse(decrypted); break;
            case Protocol::Operation::OPEN_NOTE: handleOpenNoteResponse(decrypted); break;
            case Protocol::Operation::UPDATE_TEXT: handleUpdateTextResponse(decrypted); break;
            case Protocol::Operation::SHARE_NOTE_NOTIFY: handleShareNoteNotifyRequest(decrypted); break;
            case Protocol::Operation::OWNER_APPROVE_MERGE: handleOwnerApproveMergeRequest(decrypted); break;
            case Protocol::Operation::SERVER_APPROVE_MERGE: handleServerApproveMergeRequest(decrypted); break;
            case Protocol::Operation::UPDATE_TEXT_MERGED: handleUpdateTextMergedRequest(decrypted); break;
            default: break;
        }
    }
}