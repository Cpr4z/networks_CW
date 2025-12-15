#pragma once

#include "User.hpp"
#include "Document.hpp"

#include <expected>

enum class AuthError : uint8_t {
    UserNotFound = 1,
    WrongPassword = 2
};

class Repository;

using RepositoryPtr = std::shared_ptr<Repository>;

class Repository {
public:
    static RepositoryPtr instance() {
        static RepositoryPtr repo;
        return repo;
    }

//    bool isUserExists(const std::string& login);

//    void addUser(int user_id, const std::string& login, const std::string& password);
    uint32_t addUser(const std::string& login, const std::string& password);
    std::expected<uint32_t, AuthError> validateUser(const std::string& login, const std::string& password);
    void addDocument(int user_id, const std::string& name);

private:
//    Users m_users;
    int m_users_count = 0;
    UsersMap m_user_map;
    DocumentsMap m_document_map;
};

using RepositoryPtr = std::shared_ptr<Repository>;