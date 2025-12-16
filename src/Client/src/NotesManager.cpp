#include "NotesManager.hpp"

#include <iostream>

NotesManager::NotesManager(NoteClient* client, QObject* parent)
        : QObject(parent),
          m_model(new NotesModel(this)),
          m_client(client),
          m_userId(0) {
    connect(m_client, &NoteClient::noteCreationSuccess, this, &NotesManager::onNoteCreationSuccess);
    connect(m_client, &NoteClient::noteCreationFailed, this, &NotesManager::onNoteCreationFailed);
}

QAbstractListModel* NotesManager::model() const {
//    qDebug() << "NotesManager::model()" << m_model;
    return m_model;
}

void NotesManager::createNote(const QString& title) {
//    std::cout << "Entered NotesManager::createNote" << std::endl;
    if (title.isEmpty()) {
        emit onNoteCreationFailed("Заметка не может быть создана с пустым названием");
        return;
    }
    m_client->sendCreateNoteRequest(title);
}

void NotesManager::onNoteCreationSuccess(const QString& title, uint32_t noteId) {
    m_model->addNote(static_cast<int>(noteId), title);
}

void NotesManager::onNoteCreationFailed(const QString& reason) {
    qWarning() << "Note creation failed:" << reason;
}

void NotesManager::openNote(int noteId) {
//    std::cout << "NotesManager::openNote is called" << std::endl;
}

void NotesManager::updateNote(int noteId, const QString& text) {

}