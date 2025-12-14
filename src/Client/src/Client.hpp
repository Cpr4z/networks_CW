#pragma once

#include <string>
#include <vector>

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QTcpSocket>


#include <Protocol.hpp>

#include <Document.hpp>

class NoteClient : public QObject {
    Q_OBJECT
public:
    NoteClient(QObject* parent = nullptr);

    void sendAuthRequest(const QString& login, const QString& password);
    void sendRegistrationRequest(const QString& login, const QString& password);
    void sendSyncRequest();
    void sendGetNotesRequest();

    // запрос на синхронизацию данных о заметках для пользователя
    void fetchNotes();

    // отправка данных о редактировании конкретной заметки
    void sendNote(const QString& title, const QString& content);

    void setUsername(const QString& username) { m_username = username; }

signals:
    void authSuccess(uint32_t userId);
    void authFailed(const QString& reason);

    void registrationSuccess(uint32_t userId);
    void registrationFailed(const QString& reason);

    void notesReceived(const QList<QJsonObject>& notes);
    void noteSaved(const QString& title);

private slots:
    void onReadyRead();

private:
    void handleAuthResponse(const std::vector<uint8_t>& buffer);
    void handleRegistrationResponse(const std::vector<uint8_t>& buffer);
    void handleSyncResponse(const std::vector<uint8_t>& buffer);
    void handleGetNotesResponse(const std::vector<uint8_t>& buffer);

private:
    QString m_username;
    QTcpSocket m_socket;
    DocumentsMap m_documents;
};

using ClientPtr = std::unique_ptr<NoteClient>;
using ClientsMap = Map<Id, ClientPtr>;