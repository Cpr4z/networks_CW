#pragma once

#include "User.hpp"
#include "Note.hpp"

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
    uint32_t addNote(uint32_t user_id, const std::string& title);
    bool isNoteExists(uint32_t user_id, const std::string& title);
    bool isNoteExists(uint32_t user_id, uint32_t note_id);
    std::string getNoteText(uint32_t user_id, uint32_t note_id);
    void updateNoteText(uint32_t note_id, uint32_t user_id, const std::string& text);
//    void addDocument(int user_id, const std::string& name);

private:
//    Users m_users;
    int m_users_count = 0;
    int m_notes_count = 0;
    UsersMap m_users_map;
    NotesMap m_notes_map;
};

using RepositoryPtr = std::shared_ptr<Repository>;