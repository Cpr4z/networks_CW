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
}

void NoteClient::sendAuthRequest(const QString& login, const QString& password) {
    Protocol::AuthRequest req = { login.toStdString(), password.toStdString() };
    std::vector<uint8_t> requestData = Protocol::encodeAuthRequest(req);
    std::cout << "Before calling Auth Request" << std::endl;
    m_socket.write(reinterpret_cast<char*>(requestData.data()), requestData.size());
}

void NoteClient::sendRegistrationRequest(const QString& login, const QString& password) {
    Protocol::RegistrationRequest req = { login.toStdString(), password.toStdString()};
    std::vector<uint8_t> requestData = Protocol::encodeRegistrationRequest(req);
    std::cout << "Before calling Registration Request" << std::endl;
    m_socket.write(reinterpret_cast<char*>(requestData.data()), requestData.size());
}

void NoteClient::fetchNotes() {
    QJsonObject obj;
    obj["type"] = "get_notes";
    obj["user"] = m_username;
    m_socket.write(QJsonDocument(obj).toJson(QJsonDocument::Compact) + '\n');
}

void NoteClient::sendNote(const QString& title, const QString& content) {
    QJsonObject obj;
    obj["type"] = "save_note";
    obj["user"] = m_username;
    obj["title"] = title;
    obj["content"] = content;
    m_socket.write(QJsonDocument(obj).toJson(QJsonDocument::Compact) + '\n');
}

void NoteClient::handleAuthResponse(const std::vector<uint8_t>& buffer) {
    const auto& response_opt = Protocol::decodeAuthResponse(buffer);
    if (response_opt.has_value()) {
        const auto& response = response_opt.value();
        if (response.status == 0) {
            std::cout << "Auth response status is: " << static_cast<int>(response.status) << std::endl;
            std::cout << "User id is: " << response.user_id << std::endl;
            emit authSuccess(response.user_id);
        } else {
            emit authFailed(QString::fromStdString(mapAuthErrorCode(response.status)));
        }
    }
    emit registrationFailed("Error while handling response from server");
}

void NoteClient::handleRegistrationResponse(const std::vector<uint8_t> &buffer) {
    const auto& response_opt = Protocol::decodeRegistrationResponse(buffer);
    if (response_opt.has_value()) {
        const auto& response = response_opt.value();
        if (response.status == 0) {
            std::cout << "Registration response status is: " << static_cast<int>(response.status) << std::endl;
            std::cout << "User id is: " << response.user_id << std::endl;
            emit registrationSuccess(response.user_id);
        } else {
            emit registrationFailed(QString::fromStdString("Пользователь с таким именем уже зарегистрирован"));
        }
    }
    emit registrationFailed("Error while handling response from server");
}

void NoteClient::handleSyncResponse(const std::vector<uint8_t>& buffer) {

}

void NoteClient::handleGetNotesResponse(const std::vector<uint8_t>& buffer) {

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

                default:
                    break;
            }
        }
    }
}