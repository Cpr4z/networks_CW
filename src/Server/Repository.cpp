#include "Repository.hpp"

#include <algorithm>

bool Repository::validateUser(int user_id, const std::string& login, const std::string& password) {
    const auto user = std::ranges::find_if(m_user_map, [&](const auto& item){
        return item.second->getId() == user_id;
    });

    if (user == m_user_map.end()) {
        return false;
    }

    if (user->second->getUserName() != login) {

    }

    if (user->second->getPassword() == password) {

    }

    return true;
}

bool Repository::isUserExists(int user_id) {
    const auto user = std::ranges::find_if(m_user_map, [&](const auto& item){
        return item.second->getId() == user_id;
    });
    return user != m_user_map.end();
}

void Repository::addUser(int user_id, const std::string& login, const std::string& password) {
    m_user_map[user_id] = std::make_shared<User>(user_id, login, password);
}

void Repository::addDocument(int user_id, const std::string& name) {

}