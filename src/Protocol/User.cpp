#include "User.hpp"

User::User(Id user_id, const std::string& login, const std::string& password) : m_user_data({user_id, login, password}){
}

//User::User(const std::string& login, const std::string& password) : m_user_data({login, password}){
//}

void User::updateCursorPos(size_t doc_id) {
    m_cursor_pos = doc_id;
}
