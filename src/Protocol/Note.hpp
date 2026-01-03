#pragma once

#include <vector>

#include "User.hpp"

struct NoteData {
    Id note_id;
    std::string title;
    std::string text;
    Id version;
    Id owner_id;
    bool is_shared = false;
};

class Note {
public:
    explicit Note(Id id, const std::string& title, Id owner_id);
    explicit Note(Id id, const std::string& title, const std::string& text, Id version);

    void incrementVersion();

    Id getId() const { return m_note_data.note_id; }
    Id getOwnerId() const { return m_note_data.owner_id; }
    std::string getTitle() const { return m_note_data.title; }
    std::string getText() const { return m_note_data.text; }
    void setText(const std::string& text) { m_note_data.text = text; }
    void setVersion(Id version) { m_note_data.version = version; }
    Id getVersion() const { return m_note_data.version; }
    bool isShared() const { return m_note_data.is_shared; }
    void makeShared() { m_note_data.is_shared = true; }

private:
    NoteData m_note_data;
};

using NotePtr = std::shared_ptr<Note>;
using Notes = Vector<NotePtr>;
using NotesMap = Map<UserPtr, Notes>;
