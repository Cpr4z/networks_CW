#include "AuthManager.hpp"
#include <QDebug>

void AuthManager::login(const QString& username, const QString& password) {
    if (username.isEmpty() || password.isEmpty()) {
        emit loginFailed("Заполните все поля");
        return;
    }
    m_client->sendAuthRequest(username, password);
}

void AuthManager::registerUser(const QString &username, const QString &password) {
    if (username.isEmpty() || password.isEmpty()) {
        emit registrationFailed("Заполните все поля");
        return;
    }
    m_client->sendRegistrationRequest(username, password);
}
