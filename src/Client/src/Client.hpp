#pragma once

#include <string>
#include <vector>

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QTcpSocket>


#include <Protocol.hpp>

#include <Note.hpp>

class NoteClient : public QObject {
    Q_OBJECT
public:
    NoteClient(QObject* parent = nullptr);

    void sendAuthRequest(const QString& login, const QString& password);
    void sendRegistrationRequest(const QString& login, const QString& password);
    void sendSyncRequest();
    void sendGetNotesRequest(); // user_id
    void sendCreateNoteRequest(const QString& title); // user_id

    void setUsername(const QString& username) { m_username = username; }

signals:
    // AUTH
    void authSuccess(uint32_t userId);
    void authFailed(const QString& reason);

    // REGISTRATION
    void registrationSuccess(uint32_t userId);
    void registrationFailed(const QString& reason);

    // CREATE_NOTE
    void noteCreationSuccess(const QString& title, uint32_t noteId);
    void noteCreationFailed(const QString reason);


private slots:
    void onReadyRead();

private:
    void handleAuthResponse(const std::vector<uint8_t>& buffer);
    void handleRegistrationResponse(const std::vector<uint8_t>& buffer);
    void handleSyncResponse(const std::vector<uint8_t>& buffer);
    void handleGetNotesResponse(const std::vector<uint8_t>& buffer);
    void handleCreateNoteResponse(const std::vector<uint8_t>& buffer);

private:
    uint32_t m_user_id = 0;
    QString m_username;
    QTcpSocket m_socket;
//    DocumentsMap m_documents;
};

using ClientPtr = std::unique_ptr<NoteClient>;
using ClientsMap = Map<Id, ClientPtr>;