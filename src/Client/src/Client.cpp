#include "Client.hpp"

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

void NoteClient::sendAuthRequest(const QString& login, const QString& password) {
    Protocol::AuthRequest req = { login.toStdString(), password.toStdString() };
    std::vector<uint8_t> requestData = Protocol::encodeAuthRequest(req);
    qint64 bytesWritten = m_socket.write(
            reinterpret_cast<const char*>(requestData.data()),
            static_cast<qint64>(requestData.size())
    );
    m_socket.flush();
}

void NoteClient::sendRegistrationRequest(const QString& login, const QString& password) {
    Protocol::RegistrationRequest req = { login.toStdString(), password.toStdString()};
    std::vector<uint8_t> requestData = Protocol::encodeRegistrationRequest(req);
    qint64 bytesWritten = m_socket.write(
            reinterpret_cast<const char*>(requestData.data()),
            static_cast<qint64>(requestData.size())
    );
    m_socket.flush();
}

void NoteClient::sendUpdateTextRequest(uint32_t note_id, const QString& text, uint32_t version) {
    Protocol::UpdateTextRequest req = { m_user_id, note_id, version, text.toStdString()};
    std::vector<uint8_t> requestData = Protocol::encodeUpdateTextRequest(req);
    const auto& request = Protocol::decodeUpdateTextRequest(requestData);
    qint64 bytesWritten = m_socket.write(
            reinterpret_cast<const char*>(requestData.data()),
            static_cast<qint64>(requestData.size())
    );
    m_socket.flush();
}

void NoteClient::sendGetNotesRequest() {
    Protocol::GetNotesRequest req = { m_user_id };
    std::vector<uint8_t> requestData = Protocol::encodeGetNotesRequest(req);
    qint64 bytesWritten = m_socket.write(
            reinterpret_cast<const char*>(requestData.data()),
            static_cast<qint64>(requestData.size())
    );
    m_socket.flush();
}

void NoteClient::sendCreateNoteRequest(const QString& title) {
    Protocol::CreateNoteRequest req = { m_user_id, title.toStdString() };
    std::vector<uint8_t> requestData = Protocol::encodeCreateNoteRequest(req);
    qint64 bytesWritten = m_socket.write(
            reinterpret_cast<const char*>(requestData.data()),
            static_cast<qint64>(requestData.size())
    );
    m_socket.flush();
}

void NoteClient::sendOpenNoteRequest(uint32_t note_id, uint32_t version) {
    Protocol::OpenNoteRequest req = {note_id, m_user_id, version};
    std::vector<uint8_t> requestData = Protocol::encodeOpenNoteRequest(req);
    qint64 bytesWritten = m_socket.write(
            reinterpret_cast<const char*>(requestData.data()),
            static_cast<qint64>(requestData.size())
    );
    m_socket.flush();
}

void NoteClient::sendShareNoteRequest(uint32_t note_id, uint32_t version) {
    Protocol::ShareNoteRequest req = {m_user_id, note_id, version};
    std::vector<uint8_t> requestData = Protocol::encodeShareNoteRequest(req);
    qint64 bytesWritten = m_socket.write(
            reinterpret_cast<const char*>(requestData.data()),
            static_cast<qint64>(requestData.size())
    );
    m_socket.flush();
}

void NoteClient::sendSyncNoteRequest(uint32_t note_id) {
    Protocol::SyncNoteRequest req = {note_id, m_user_id};
    std::vector<uint8_t> requestData = Protocol::encodeSyncRequest(req);
    qint64 bytesWritten = m_socket.write(
            reinterpret_cast<const char*>(requestData.data()),
            static_cast<qint64>(requestData.size())
    );
    m_socket.flush();
}

void NoteClient::sendApproveMergeRequest(uint32_t noteId, uint8_t status, const QString& merged_version) {
    Protocol::ApproveMergeRequest req = { noteId, m_user_id, status, merged_version.toStdString() };
    std::vector<uint8_t> requestData = Protocol::encodeApproveMergeRequest(req);
    qint64 bytesWritten = m_socket.write(
            reinterpret_cast<const char*>(requestData.data()),
            static_cast<qint64>(requestData.size())
    );
    m_socket.flush();
}

void NoteClient::sendOwnerApproveMergeResponse(uint32_t noteId, uint32_t new_version, uint32_t merge_sender_id, uint8_t result_code, const QString& approved_version) {
    Protocol::OwnerApproveMergeResponse resp = {Protocol::Operation::OWNER_APPROVE_MERGE, noteId, merge_sender_id, new_version, result_code, approved_version.toStdString()};
    std::vector<uint8_t> requestData = Protocol::encodeOwnerApproveMergeResponse(resp);
    qint64 bytesWritten = m_socket.write(
            reinterpret_cast<const char*>(requestData.data()),
            static_cast<qint64>(requestData.size())
    );
    m_socket.flush();
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

void NoteClient::handleShareNoteResponse(const std::vector<uint8_t>& buffer) {

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
    while (m_socket.bytesAvailable() > 0) {
        QByteArray data = m_socket.readAll();
        std::vector<uint8_t> buffer(data.begin(), data.end());
        if (buffer.size() >= 2) {
            Protocol::Operation op = Protocol::decodeOperation(buffer);
            switch (op) {
                case Protocol::Operation::AUTH:
                    handleAuthResponse(buffer);
                    break;

                case Protocol::Operation::REGISTRATION:
                    handleRegistrationResponse(buffer);
                    break;

                case Protocol::Operation::SYNC:
                    handleSyncResponse(buffer);
                    break;

                case Protocol::Operation::GET_NOTES:
                    handleGetNotesResponse(buffer);
                    break;

                case Protocol::Operation::CREATE_NOTE:
                    handleCreateNoteResponse(buffer);
                    break;

                case Protocol::Operation::OPEN_NOTE:
                    handleOpenNoteResponse(buffer);
                    break;

                case Protocol::Operation::UPDATE_TEXT:
                    handleUpdateTextResponse(buffer);
                    break;

                case Protocol::Operation::SHARE_NOTE:
                    handleShareNoteResponse(buffer);
                    break;

                case Protocol::Operation::SHARE_NOTE_NOTIFY:
                    handleShareNoteNotifyRequest(buffer);
                    break;

                case Protocol::Operation::OWNER_APPROVE_MERGE:
                    handleOwnerApproveMergeRequest(buffer);
                    break;

                case Protocol::Operation::SERVER_APPROVE_MERGE:
                    handleServerApproveMergeRequest(buffer);
                    break;

                case Protocol::Operation::UPDATE_TEXT_MERGED:
                    handleUpdateTextMergedRequest(buffer);
                    break;
                default:
                    break;
            }
        }
    }
}