#include "Client.hpp"

#include <QJsonDocument>

#include <iostream>

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
    m_socket.connectToHost("127.0.0.1", 8080); // данные сервера, на который мы отправляем запросы
//    m_socket.connectToHost("192.168.64.4", 8080);

}

void NoteClient::sendAuthRequest(const QString& login, const QString& password) {
    Protocol::AuthRequest req = { login.toStdString(), password.toStdString() };
    std::vector<uint8_t> requestData = Protocol::encodeAuthRequest(req);
//    std::cout << "Before calling Auth Request" << std::endl;
    m_socket.write(reinterpret_cast<char*>(requestData.data()), requestData.size());
}

void NoteClient::sendRegistrationRequest(const QString& login, const QString& password) {
    Protocol::RegistrationRequest req = { login.toStdString(), password.toStdString()};
    std::vector<uint8_t> requestData = Protocol::encodeRegistrationRequest(req);
//    std::cout << "Before calling Registration Request" << std::endl;
    m_socket.write(reinterpret_cast<char*>(requestData.data()), requestData.size());
}

void NoteClient::sendUpdateTextRequest(uint32_t note_id, const QString& text) {
//    uint32_t user_id;
//    uint32_t note_id;
//    std::string text;
    Protocol::UpdateTextRequest req = { m_user_id, note_id, text.toStdString()};
    std::cout << "User with id: " << m_user_id << std::endl;
    std::cout << "Want to update note with id: " << note_id << std::endl;
    std::cout << "New text of note is: " << text.toStdString() << std::endl;
    std::vector<uint8_t> requestData = Protocol::encodeUpdateTextRequest(req);
    const auto& request = Protocol::decodeUpdateTextRequest(requestData);
    std::cout << "Decoded data of update text request: " << request->text << std::endl;
    m_socket.write(reinterpret_cast<char*>(requestData.data()), requestData.size());
}

void NoteClient::sendSyncRequest() {

}

void NoteClient::sendGetNotesRequest() {

}

void NoteClient::sendCreateNoteRequest(const QString& title) {
    Protocol::CreateNoteRequest req = { m_user_id, title.toStdString() };
    std::cout << "User who want to create note: " << m_user_id << std::endl;
    std::cout << "Note title: " << title.toStdString() << std::endl;
    std::vector<uint8_t> requestData = Protocol::encodeCreateNoteRequest(req);
    m_socket.write(reinterpret_cast<char*>(requestData.data()), requestData.size());
}

void NoteClient::sendOpenNoteRequest(uint32_t note_id) {
    Protocol::OpenNoteRequest req = {note_id, m_user_id};
    std::cout << "User who want to open note: " << m_user_id << std::endl;
    std::cout << "Note id: " << note_id << std::endl;
    std::vector<uint8_t> requestData = Protocol::encodeOpenNoteRequest(req);
    m_socket.write(reinterpret_cast<char*>(requestData.data()), requestData.size());
}

void NoteClient::sendShareNoteRequest(uint32_t note_id) {
    Protocol::ShareNoteRequest req = {m_user_id, note_id};
    std::cout << "User with id - " << m_user_id << " want to share note with id - " << note_id << std::endl;
    std::vector<uint8_t> requestData = Protocol::encodeShareNoteRequest(req);
    m_socket.write(reinterpret_cast<char*>(requestData.data()), requestData.size());
}

void NoteClient::handleAuthResponse(const std::vector<uint8_t>& buffer) {
    const auto& response_opt = Protocol::decodeAuthResponse(buffer);
    if (response_opt.has_value()) {
        const auto& response = response_opt.value();
        if (response.status == 0) {
            std::cout << "Auth response status is: " << static_cast<int>(response.status) << std::endl;
            std::cout << "User id is: " << response.user_id << std::endl;
            m_user_id = response.user_id;
            emit authSuccess(response.user_id);
        } else {
            std::cout << "Auth response status is: " << static_cast<int>(response.status) << std::endl;
            emit authFailed(QString::fromStdString(mapAuthErrorCode(response.status)));
        }
    } else {
        emit authFailed("Error while handling response from server");
    }
}

void NoteClient::handleRegistrationResponse(const std::vector<uint8_t>& buffer) {
    const auto& response_opt = Protocol::decodeRegistrationResponse(buffer);
    if (response_opt.has_value()) {
        std::cout << "Registration response is not nullopt" << std::endl;
        const auto& response = response_opt.value();
        std::cout << static_cast<int>(response.status) << std::endl;
        if (static_cast<int>(response.status) == 0) {
            std::cout << "Registration response status is: " << static_cast<int>(response.status) << std::endl;
            std::cout << "User id is: " << response.user_id << std::endl;
            emit registrationSuccess(response.user_id);
        } else {
            std::cout << "Registration response status is: " << static_cast<int>(response.status) << std::endl;
            emit registrationFailed(QString::fromStdString("Пользователь с таким именем уже зарегистрирован"));
        }
    } else {
        emit registrationFailed("Error while handling response from server");
    }
}

void NoteClient::handleSyncResponse(const std::vector<uint8_t>& buffer) {

}

void NoteClient::handleGetNotesResponse(const std::vector<uint8_t>& buffer) {

}

void NoteClient::handleCreateNoteResponse(const std::vector<uint8_t>& buffer) {
    const auto& response_opt = Protocol::decodeCreateNoteResponse(buffer);
    if (response_opt.has_value()) {
        const auto& response = response_opt.value();
        std::cout << "Status of creating new note: " << static_cast<int>(response.status) << std::endl;
        std::cout << "Id of created note: " << response.note_id << std::endl;
        std::cout << "Title of new note is: " << response.note_title << std::endl;
        if (response.status == 0) {
            emit noteCreationSuccess(QString::fromStdString(response.note_title), response.note_id, m_user_id);
        } else {
            emit noteCreationFailed("Заметка с таким именем уже существует");
        }
    }
}

void NoteClient::handleOpenNoteResponse(const std::vector<uint8_t>& buffer) {
    const auto& response_opt = Protocol::decodeOpenNoteResponse(buffer);
    if (response_opt.has_value()) {
        const auto& response = response_opt.value();
//        uint32_t note_id;
//        std::string title;
//        std::string text;
        std::cout << "Opened note id is: " << response.note_id << std::endl;
        std::cout << "Opened note title: " << response.title << std::endl;
        std::cout << "Opened note text is: " << response.text << std::endl;
        if (response.status == 0) {
            emit noteOpenSuccess(response.note_id, QString::fromStdString(response.text));
        } else {
            emit noteOpenFailed(response.note_id, "Не удалось открыть заметку");
        }
    }
}

void NoteClient::handleUpdateTextResponse(const std::vector<uint8_t>& buffer) {
    const auto& response_opt = Protocol::decodeUpdateTextResponse(buffer);
    if (response_opt.has_value()) {
        const auto& response = response_opt.value();
        std::cout << "Result of update text request: " << static_cast<int>(response.status) << std::endl;
        std::cout << "Id of update text note: " << response.note_id << std::endl;
        if (response.status == 0) {
            emit updateTextSuccess(response.note_id, m_user_id);
        } else {
            emit updateTextFailed("Ошибка во время редактирования заметки");
        }
    }
}

void NoteClient::handleShareNoteResponse(const std::vector<uint8_t>& buffer) {

}

void NoteClient::handleShareNoteNotifyRequest(const std::vector<uint8_t>& buffer) {
    const auto& request_opt = Protocol::decodeShareNoteNotifyRequest(buffer);
    if (request_opt.has_value()) {
        const auto& request = request_opt.value();
        std::cout << "Note share notify data:" << std::endl;
        std::cout << request.note_id << std::endl;
        std::cout << request.owner_id << std::endl;
        std::cout << request.note_title << std::endl;
//        std::cout << request.note_text << std::endl;

        emit createSharedNote(request.note_id, QString::fromStdString(request.note_title), request.owner_id);

    }
}

void NoteClient::onReadyRead() {
    while (m_socket.bytesAvailable() > 0) {
        std::cout << "Some bytes avaliable" << std::endl;
        QByteArray data = m_socket.readAll();
        std::vector<uint8_t> buffer(data.begin(), data.end());
        if (buffer.size() >= 2) {
            std::cout << "Decoding response" << std::endl;
            Protocol::Operation op = Protocol::decodeOperation(buffer);
            switch (op) {
                case Protocol::Operation::AUTH:
                    std::cout << "Got auth response" << std::endl;
                    handleAuthResponse(buffer);
                    break;

                case Protocol::Operation::REGISTRATION:
                    std::cout << "Got registration response" << std::endl;
                    handleRegistrationResponse(buffer);
                    break;

                case Protocol::Operation::SYNC:
                    handleSyncResponse(buffer);
                    break;

                case Protocol::Operation::GET_NOTES:
                    handleGetNotesResponse(buffer);
                    break;

                case Protocol::Operation::CREATE_NOTE:
                    std::cout << "Got create note response" << std::endl;
                    handleCreateNoteResponse(buffer);
                    break;

                case Protocol::Operation::OPEN_NOTE:
                    std::cout << "Got open note response" << std::endl;
                    handleOpenNoteResponse(buffer);
                    break;

                case Protocol::Operation::UPDATE_TEXT:
                    std::cout << "Got update text response" << std::endl;
                    handleUpdateTextResponse(buffer);
                    break;

                case Protocol::Operation::SHARE_NOTE:
                    std::cout << "Got share note response" << std::endl;
                    handleShareNoteResponse(buffer);
                    break;

                case Protocol::Operation::SHARE_NOTE_NOTIFY:
                    std::cout << "Got share note notify response" << std::endl;
                    handleShareNoteNotifyRequest(buffer);
                    break;
                default:
                    std::cout << "Unknown operation code: " << static_cast<uint16_t>(op) << std::endl;
                    break;
            }
        }
    }
}