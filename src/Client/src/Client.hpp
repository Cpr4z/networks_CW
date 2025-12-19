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
    void sendOpenNoteRequest(uint32_t note_id);
    void sendUpdateTextRequest(uint32_t note_id, const QString& text);
    void sendShareNoteRequest(uint32_t note_id);

    void setUsername(const QString& username) { m_username = username; }

signals:
    // AUTH
    void authSuccess(uint32_t userId);
    void authFailed(const QString& reason);

    // REGISTRATION
    void registrationSuccess(uint32_t userId);
    void registrationFailed(const QString& reason);

    // CREATE_NOTE
    void noteCreationSuccess(const QString& title, uint32_t noteId, uint32_t ownerId);
    void noteCreationFailed(const QString& reason);

    // OPEN_NOTE
    void noteOpenSuccess(uint32_t note_id, const QString& text);
    void noteOpenFailed(uint32_t note_id, const QString& reason);

    // UPDATE_TEXT
    void updateTextSuccess(uint32_t note_id, uint32_t user_id);
    void updateTextFailed(const QString& reason);

    void createSharedNote(uint32_t note_id, const QString& title, uint32_t user_id);


private slots:
    void onReadyRead();

private:
    void handleAuthResponse(const std::vector<uint8_t>& buffer);
    void handleRegistrationResponse(const std::vector<uint8_t>& buffer);
    void handleSyncResponse(const std::vector<uint8_t>& buffer);
    void handleGetNotesResponse(const std::vector<uint8_t>& buffer);
    void handleCreateNoteResponse(const std::vector<uint8_t>& buffer);
    void handleOpenNoteResponse(const std::vector<uint8_t>& buffer);
    void handleUpdateTextResponse(const std::vector<uint8_t>& buffer);
    void handleShareNoteResponse(const std::vector<uint8_t>& buffer);


    void handleShareNoteNotifyRequest(const std::vector<uint8_t>& buffer);

private:
    uint32_t m_user_id = 0;
    QString m_username;
    QTcpSocket m_socket;
//    DocumentsMap m_documents;
};

using ClientPtr = std::unique_ptr<NoteClient>;
using ClientsMap = Map<Id, ClientPtr>;