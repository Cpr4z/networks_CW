#pragma once

#include <memory>
#include <string>

#include <BasicTypes.hpp>

struct UserData {
    Id user_id;
    std::string user_name;
    std::string password;
};

class User {
public:
    explicit User(Id user_id, const std::string& login, const std::string& password);
//    explicit User(const std::string& login, const std::string& password);

    [[nodiscard]] Id getCursorPosition() const { return m_cursor_pos; }
    [[nodiscard]] Id getDocId() const { return m_doc_id; }
    [[nodiscard]] Id getId() const { return m_user_data.user_id; }
    [[nodiscard]] std::string getUserName() const { return m_user_data.user_name; }
    [[nodiscard]] std::string getPassword() const { return m_user_data.password; }

    void updateCursorPos(Id doc_id);

private:
    Id m_doc_id = 0;
    Id m_cursor_pos = 0;
    UserData m_user_data;
};

using UserPtr = std::shared_ptr<User>;
using Users = Vector<UserPtr>;
using UsersMap = Map<Id, UserPtr>;
