#include "NotesManager.hpp"

#include <iostream>

NotesManager::NotesManager(NoteClient* client, QObject* parent)
        : QObject(parent),
          m_model(new NotesModel(this)),
          m_client(client),
          m_userId(0) {
    connect(m_client, &NoteClient::noteCreationSuccess, this, &NotesManager::onNoteCreationSuccess);
    connect(m_client, &NoteClient::noteCreationFailes, this, &NotesManager::onNoteCreationFailed);
}

QAbstractListModel* NotesManager::model() {
    return m_model;
}

void NotesManager::createNote(const QString& title) {
    std::cout << "Entered NotesManager::createNote" << std::endl;
    if (title.isEmpty()) {
        emit onNoteCreationFailed("Заметка не может быть создана с пустым названием");
    }
    m_client->sendCreateNoteRequest(title);
}

void NotesManager::openNote(int noteId) {

}

void NotesManager::updateNote(int noteId, const QString& text) {

}