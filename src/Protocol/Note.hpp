#pragma once

#include <vector>

#include "User.hpp"

struct NoteData {
    Id note_id;
    Id cursor_pos;
    std::string title;
    std::string text;
    Id version;
    Users current_users;
};

class Note {
public:
    explicit Note(Id id, const std::string& title);
    explicit Note(Id id, const std::string& title, const std::string& text);

    void addUser(const User& user);
    void removeUser(Id user_id);

    void sync();

    void incrementVersion();

    Id getId() const { return m_note_data.note_id; }
    Id getCursorPos() const { return m_note_data.cursor_pos; }
    std::string getTitle() const { return m_note_data.title; }
    std::string getText() const { return m_note_data.text; }
    void setText(const std::string& text) { m_note_data.text = text;}
    Users getCurrentUsers() const { return m_note_data.current_users; }
    void setVersion(Id version) { m_note_data.version = version; }
    Id getVersion() const { return m_note_data.version; }

private:
    NoteData m_note_data;
//    Id m_id = 0;
//    Id m_cursor_pos = 0;
//    Users m_current_users;
};

using NotePtr = std::shared_ptr<Note>;
using Notes = Vector<NotePtr>;
//using DocumentsMap = Map<Id, NotePtr>;
using NotesMap = Map<UserPtr, Notes>;
