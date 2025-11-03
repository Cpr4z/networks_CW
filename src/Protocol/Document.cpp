#include "Document.hpp"

#include <ranges>

Document::Document(Id id) : m_id(id) {
}

void Document::addUser(const User& user) {
    m_current_users.emplace_back(std::make_shared<User>(user));
}

void Document::removeUser(Id user_id)
{
    std::erase_if(m_current_users, [user_id](const auto& user){
        return user->getId() == user_id;
    });
}