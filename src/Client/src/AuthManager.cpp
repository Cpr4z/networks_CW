#include "AuthManager.hpp"
#include <QDebug>

void AuthManager::login(const QString& username, const QString& password) {
    if (username.isEmpty() || password.isEmpty()) {
        emit loginFailed("Заполните все поля");
        return;
    }

    m_client->sendAuthRequest(username, password);

    // операция AUTH
//    if (response.status != 1) {
//        emit loginFailed(QString::fromStdString(response.error_msg));
//    } else {
//        emit loginSuccess(response.user_id);
//    }
}

void AuthManager::registerUser(const QString &username, const QString &password) {
    if (username.isEmpty() || password.isEmpty()) {
        emit registrationFailed("Заполните все поля");
        return;
    }

    m_client->sendRegistrationRequest(username, password);
//    if (response.status != 1) {
//        emit registrationFailed(QString::fromStdString(response.error_msg));
//    } else {
//        emit registrationSuccess(response.user_id);
//    }
}
