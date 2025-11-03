#pragma once

#include <vector>

#include "User.hpp"

class Document {
public:
    explicit Document(Id id);

    void addUser(const User& user);
    void removeUser(Id user_id);

    void sync();

private:
    Id m_id = 0;
    Id m_cursor_pos = 0;
    Users m_current_users;
};

using DocumentPtr = std::shared_ptr<Document>;
using Documents = Vector<DocumentPtr>;
using DocumentsMap = Map<Id, DocumentPtr>;
