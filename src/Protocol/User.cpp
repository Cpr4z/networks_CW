#include "User.hpp"

User::User(const std::string& user_name, size_t user_id, size_t doc_id) : m_user_data({user_id, user_name}), m_doc_id(doc_id){
}

void User::updateCursorPos(size_t doc_id) {
    m_cursor_pos = doc_id;
}
