#pragma once

#include "User.hpp"
#include "Document.hpp"

class Repository;

using RepositoryPtr = std::shared_ptr<Repository>;

class Repository {
public:
    static RepositoryPtr instance() {
        static RepositoryPtr repo;
        return repo;
    }

    bool isUserExists(int id);

    void addUser(int user_id, const std::string& login, const std::string& password);
    bool validateUser(int user_id, const std::string& login, const std::string& password);
    void addDocument(int user_id, const std::string& name);

private:
    UsersMap m_user_map;
    DocumentsMap m_document_map;
};

using RepositoryPtr = std::shared_ptr<Repository>;