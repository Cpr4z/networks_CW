#pragma once

#include <memory>
#include <string>

#include <BasicTypes.hpp>

struct UserData {
    Id user_id;
    std::string user_name;
};

class User {
public:
    explicit User(const std::string& user_name, Id user_id, Id doc_id = 0);

    [[nodiscard]] Id getCursorPosition() const { return m_cursor_pos; }
    [[nodiscard]] Id getDocId() const { return m_doc_id; }
    [[nodiscard]] Id getId() const { return m_user_data.user_id; }
    [[nodiscard]] std::string getName() const { return m_user_data.user_name; }

    void updateCursorPos(Id doc_id);

private:
    Id m_doc_id = 0;
    Id m_cursor_pos = 0;
    UserData m_user_data;
};

using UserPtr = std::shared_ptr<User>;
using Users = Vector<UserPtr>;
using UsersMap = Map<Id, UserPtr>;
