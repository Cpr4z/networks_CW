#include "Repository.hpp"

#include <iostream>
#include <algorithm>

std::expected<uint32_t, AuthError> Repository::validateUser(const std::string& login, const std::string& password) {
    const auto user = std::ranges::find_if(m_users_map, [&](const auto& item){
        return item.second->getUserName() == login;
    });

    if (user == m_users_map.end()) {
        return std::unexpected<AuthError>(AuthError::UserNotFound);
    }

    if (user->second->getPassword() != password) {
        return std::unexpected<AuthError>(AuthError::WrongPassword);
    }
    return (*user).second->getId();
}

uint32_t Repository::addUser(const std::string& login, const std::string& password) {
    const uint32_t newUserId = ++m_users_count;
    const auto newUser = std::make_shared<User>(newUserId, login, password);
    m_users_map[newUserId] = newUser;
//    m_notes_map.emplace();
    m_notes_map[newUser];
//    m_notes_map.insert(newUser, {});
    return m_users_count;
}

bool Repository::isNoteExists(uint32_t user_id, const std::string& title) {
    const auto& user = std::ranges::find_if(m_notes_map, [&](const auto& item){
        return item.first->getId() == user_id;
    });

    if (user == m_notes_map.end()) {
        std::cerr << "Error while searching note for user" << std::endl;
        std::cerr << "There is not user with id: " << user_id << std::endl;
    }

    const auto& note = std::ranges::find_if(user->second, [&](const auto& item){
        return item->getTitle() == title;
    });

    return note != user->second.end();
}

bool Repository::isNoteExists(uint32_t user_id, uint32_t note_id) {
    const auto& user = std::ranges::find_if(m_notes_map, [&](const auto& item){
        return item.first->getId() == user_id;
    });

    if (user == m_notes_map.end()) {
        std::cerr << "Error while adding new note from user" << std::endl;
        std::cerr << "There is not user with id: " << user_id << std::endl;
    }

    const auto& note = std::ranges::find_if(user->second, [&](const auto& item){
        return item->getId() == note_id;
    });

    return note != user->second.end();
}

uint32_t Repository::addNote(uint32_t user_id, const std::string& title) {
    const auto& user = std::ranges::find_if(m_notes_map, [&](const auto& item){
        return item.first->getId() == user_id;
    });

    if (user == m_notes_map.end()) {
        std::cerr << "Error while adding new note from user" << std::endl;
        std::cerr << "There is not user with id: " << user_id << std::endl;
        return 0;
    }

    const uint32_t newNoteId = ++m_notes_count;

    user->second.emplace_back(std::make_shared<Note>(newNoteId, title));

    return newNoteId;
}

std::string Repository::getNoteText(uint32_t user_id, uint32_t note_id) {
    const auto& user = std::ranges::find_if(m_notes_map, [&](const auto& item){
        return item.first->getId() == user_id;
    });

    if (user == m_notes_map.end()) {
        std::cerr << "Error while adding new note from user" << std::endl;
        std::cerr << "There is not user with id: " << user_id << std::endl;
        return {};
    }

    const auto& note = std::ranges::find_if(user->second, [&](const auto& item){
        return item->getId() == note_id;
    });

    if (note == user->second.end()) {
        std::cerr << "Error while opening note with id - " << note_id << " for user with id - " << user_id << std::endl;
        return {};
    }
    return (*note)->getText();
}

void Repository::updateNoteText(uint32_t note_id, uint32_t user_id, const std::string& text) {
    auto user = std::ranges::find_if(m_notes_map, [&](const auto& item){
        return item.first->getId() == user_id;
    });

    if (user == m_notes_map.end()) {
        std::cerr << "Error while adding new note from user" << std::endl;
        std::cerr << "There is not user with id: " << user_id << std::endl;
        return;
    }

    auto note = std::ranges::find_if(user->second, [&](const auto& item){
        return item->getId() == note_id;
    });

    if (note == user->second.end()) {
        std::cerr << "Error while updating note text with id - " << note_id << " for user with id - " << user_id << std::endl;
        return;
    }

    (*note)->setText(text);
}

std::pair<std::string, std::string> Repository::getNoteInfo(uint32_t note_id, uint32_t user_id) {
    auto user = std::ranges::find_if(m_notes_map, [&](const auto& item){
        return item.first->getId() == user_id;
    });

    if (user == m_notes_map.end()) {
        std::cerr << "Error while getting info about note with id: " << note_id;
        std::cerr << "from user with id: " << user_id;
        return {};
    }

    auto note = std::ranges::find_if(user->second, [&](const auto& item){
        return item->getId() == note_id;
    });

    if (note == user->second.end()) {
        std::cerr << "Error while updating note text with id - " << note_id << " for user with id - " << user_id << std::endl;
        return {};
    }

    return {(*note)->getTitle(), (*note)->getText()};
}

void Repository::shareNoteToAllUsers(uint32_t owner_id, uint32_t note_id) {
    auto note_info = getNoteInfo(note_id, owner_id);
    if (note_info.first.empty()) {
        std::cerr << "Error: Note not found for sharing. Owner id: " << owner_id
                  << ", Note id: " << note_id << std::endl;
        return;
    }

    const std::string& title = note_info.first;
    const std::string& text = note_info.second;

    std::cout << "Sharing note title: " << title << std::endl;
    std::cout << "Sharing note text: " << text << std::endl;

    // Проходим по всем пользователям кроме владельца
    for (auto& [user_ptr, notes_vector] : m_notes_map) {
        uint32_t current_user_id = user_ptr->getId();

        // Пропускаем владельца
        if (current_user_id == owner_id) {
            continue;
        }

        // Проверяем, нет ли уже такой заметки у пользователя
        bool note_already_exists = false;
        for (const auto& note_ptr : notes_vector) {
            if (note_ptr->getId() == note_id) {
                note_already_exists = true;
                break;
            }
//            if (note_ptr->getTitle() == title && note_ptr->getText() == text) {
//                note_already_exists = true;
//                break;
//            }
        }

        // Если заметки еще нет, добавляем ее
        if (!note_already_exists) {
            // Создаем новую заметку для текущего пользователя
//            const uint32_t new_note_id = ++m_notes_count;
            auto new_note = std::make_shared<Note>(note_id, title);
            new_note->setText(text);
            notes_vector.emplace_back(new_note);

            std::cout << "Shared note '" << title << "' to user with id: "
                      << current_user_id << std::endl;
        }
    }
}