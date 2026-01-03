#include "Note.hpp"

Note::Note(Id id, const std::string& title, Id owner_id) : m_note_data({id, title, {}, 0, owner_id}) {
}

Note::Note(Id id, const std::string& title, const std::string& text, Id version) : m_note_data({id, title, text, version, 0}) {

}

void Note::incrementVersion() {
    m_note_data.version++;
}


