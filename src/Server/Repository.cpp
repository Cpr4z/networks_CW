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
    m_notes_map[newUser];
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

std::pair<uint32_t, uint32_t> Repository::addNote(uint32_t user_id, const std::string& title) {
    const auto& user = std::ranges::find_if(m_notes_map, [&](const auto& item){
        return item.first->getId() == user_id;
    });

    if (user == m_notes_map.end()) {
        std::cerr << "Error while adding new note from user" << std::endl;
        std::cerr << "There is not user with id: " << user_id << std::endl;
        return {};
    }

    const uint32_t newNoteId = ++m_notes_count;

    const auto new_note = std::make_shared<Note>(newNoteId, title, user_id);
    new_note->incrementVersion();
    user->second.emplace_back(new_note);

    return {newNoteId, new_note->getVersion()};
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

    (*note)->incrementVersion();
    (*note)->setText(text);
}

std::tuple<std::string, std::string, uint32_t>  Repository::getNoteInfo(uint32_t note_id, uint32_t user_id) {
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

    return {(*note)->getTitle(), (*note)->getText(), (*note)->getVersion()};
}

void Repository::shareNoteToAllUsers(uint32_t owner_id, uint32_t note_id) {
    const auto& [title, text, version] = getNoteInfo(note_id, owner_id);
    if (title.empty()) {
        std::cerr << "Error: Note not found for sharing. Owner id: " << owner_id
                  << ", Note id: " << note_id << std::endl;
        return;
    }

    std::cout << "Sharing note title: " << title << std::endl;
    std::cout << "Sharing note text: " << text << std::endl;

    for (auto& [user_ptr, notes_vector] : m_notes_map) {
        uint32_t current_user_id = user_ptr->getId();

        if (current_user_id == owner_id) {
            continue;
        }

        bool note_already_exists = false;
        for (const auto& note_ptr : notes_vector) {
            if (note_ptr->getId() == note_id) {
                note_ptr->makeShared();
                note_already_exists = true;
                break;
            }
        }

        if (!note_already_exists) {
            notes_vector.emplace_back(std::make_shared<Note>(note_id, title, text, version));

            std::cout << "Shared note '" << title << "' to user with id: "
                      << current_user_id << std::endl;
        }
    }
}

bool Repository::isContainsConflict(uint32_t user_id, uint32_t note_id, uint32_t version) {
    auto user = std::ranges::find_if(m_notes_map, [&](const auto& item){
        return item.first->getId() == user_id;
    });

    if (user == m_notes_map.end()) {
        std::cerr << "Error while checking conflicts about note with id: " << note_id << std::endl;
        std::cerr << "with version: " << version << std::endl;
        std::cerr << "from user with id: " << user_id << std::endl;
        return false;
    }

    auto note = std::ranges::find_if(user->second, [&](const auto& item){
        return item->getId() == note_id;
    });

    if (note == user->second.end()) {
        std::cerr << "Error while updating note text with id - " << note_id << " for user with id - " << user_id << std::endl;
        return false;
    }

    return (*note)->getVersion() != version;
}

uint32_t Repository::getNoteVersion(uint32_t user_id, uint32_t note_id) {
    auto user = std::ranges::find_if(m_notes_map, [&](const auto& item){
        return item.first->getId() == user_id;
    });

    if (user == m_notes_map.end()) {
        std::cerr << "Error while checking conflicts about note with id: " << note_id << std::endl;
        std::cerr << "from user with id: " << user_id << std::endl;
        return 0;
    }

    auto note = std::ranges::find_if(user->second, [&](const auto& item){
        return item->getId() == note_id;
    });

    if (note == user->second.end()) {
        std::cerr << "Error while updating note text with id - " << note_id << " for user with id - " << user_id << std::endl;
        return 0;
    }
    return (*note)->getVersion();
}

void Repository::updateVersionForSharedNotes(uint32_t sender_id, uint32_t note_id) {
    auto user = std::ranges::find_if(m_notes_map, [&](const auto& item){
        return item.first->getId() == sender_id;
    });

    if (user == m_notes_map.end()) {
        std::cerr << "Repository::updateVersionForSharedNotes 1 error" << std::endl;
        return;
    }

    auto note = std::ranges::find_if(user->second, [&](const auto& item){
        return item->getId() == note_id;
    });

    if (note == user->second.end()) {
        std::cerr << "Repository::updateVersionForSharedNotes 2 error" << std::endl;
        return;
    }

    Id note_version = (*note)->getVersion();
    const std::string& text = (*note)->getText();

    for (auto& [note_user, notes] : m_notes_map) {
        if (note_user->getId() == sender_id) {
            continue;
        }

        for (auto& user_note : notes) {
            if (user_note->getId() == note_id) {
                std::cout << "Note with id " << note_id << " for user " << note_user->getId() << " was updated to version " << note_version << std::endl;
                user_note->setText(text);
                user_note->setVersion(note_version);
            }
        }
    }
}

std::tuple<std::string, std::string, uint32_t> Repository::getNoteInfoToSync(uint32_t note_id, uint32_t sync_user_id) {
    for (const auto& [user, notes] : m_notes_map) {
        if (user->getId() == sync_user_id) {
            continue;
        }

        for (const auto& note : notes) {
            if (note->getId() == note_id) {
                return {note->getTitle(), note->getText(), note->getVersion()};
            }
        }
    }
    return {};
}

uint32_t Repository::getOwnerId(uint32_t note_id) {
    for (const auto& [user, notes] : m_notes_map) {
        for (const auto& note : notes) {
            if (note->getId() != note_id) {
                continue;
            }
            if (note->getOwnerId() != 0) {
                return note->getOwnerId();
            }
        }
    }
    return 0;
}

std::map<uint32_t, std::tuple<std::string, bool, uint32_t, uint32_t>> Repository::getNotesForUser(uint32_t user_id) {
    std::map<uint32_t, std::tuple<std::string, bool, uint32_t, uint32_t>> result;
    const auto user = std::ranges::find_if(m_notes_map, [&](const auto& item){
        return item.first->getId() == user_id;
    });
    for (const auto& note : user->second) {
        result[note->getId()] = std::make_tuple(note->getTitle(), note->isShared(), note->getVersion(), note->getOwnerId());
    }
    return result;
}

std::string Repository::getOwnerTextVersion(uint32_t note_id) {
    uint32_t owner_id = getOwnerId(note_id);
    const auto user = std::ranges::find_if(m_notes_map, [&](const auto& item){
        return item.first->getId() == owner_id;
    });
    const auto note = std::ranges::find_if(user->second, [&](const auto& item){
        return item->getId() == note_id;
    });

    return (*note)->getText();
}