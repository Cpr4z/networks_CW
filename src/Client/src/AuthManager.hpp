#pragma once
#include <QObject>
#include <QString>

#include "Client.hpp"

#include <iostream>

class AuthManager : public QObject {
    Q_OBJECT
public:
    explicit AuthManager(NoteClient* client, QObject* parent = nullptr) : QObject(parent), m_client(client) {
        connect(m_client, &NoteClient::authSuccess, this, &AuthManager::onAuthSuccess);
        connect(m_client, &NoteClient::authFailed, this, &AuthManager::onAuthFailed);
        connect(m_client, &NoteClient::registrationSuccess, this, &AuthManager::onRegistrationSuccess);
        connect(m_client, &NoteClient::registrationFailed, this, &AuthManager::onRegistrationFailed);
    }

    Q_INVOKABLE void login(const QString& username, const QString& password);
    Q_INVOKABLE void registerUser(const QString& username, const QString& password);

signals:
    void loginSuccess(uint32_t userId);
    void loginFailed(const QString& reason);

    void registrationSuccess(uint32_t userId);
    void registrationFailed(const QString& reason);

private slots:
    void onAuthSuccess(uint32_t id) {
    emit loginSuccess(id);
}
    void onAuthFailed(const QString& err) {
    emit loginFailed(err);
}

    void onRegistrationSuccess(uint32_t id) {
    emit registrationSuccess(id);
}
    void onRegistrationFailed(const QString& err) {
    emit registrationFailed(err);
}

private:
    NoteClient* m_client = nullptr;
};