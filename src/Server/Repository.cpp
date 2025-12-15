#include "Repository.hpp"

#include <algorithm>

std::expected<uint32_t, AuthError> Repository::validateUser(const std::string& login, const std::string& password) {
    const auto user = std::ranges::find_if(m_user_map, [&](const auto& item){
        return item.second->getUserName() == login;
    });

    if (user == m_user_map.end()) {
        return std::unexpected<AuthError>(AuthError::UserNotFound);
    }

    if (user->second->getPassword() != password) {
        return std::unexpected<AuthError>(AuthError::WrongPassword);
    }
    return (*user).second->getId();
}

uint32_t Repository::addUser(const std::string& login, const std::string& password) {
    m_user_map[++m_users_count] = std::make_shared<User>(m_users_count, login, password);
    return m_users_count;
}

void Repository::addDocument(int user_id, const std::string& name) {

}