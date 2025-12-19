#include "NotesManager.hpp"

#include <iostream>

NotesManager::NotesManager(NoteClient* client, QObject* parent)
        : QObject(parent),
          m_model(new NotesModel(this)),
          m_client(client),
          m_userId(0) {
    connect(m_client, &NoteClient::noteCreationSuccess, this, &NotesManager::onNoteCreationSuccess);
    connect(m_client, &NoteClient::noteCreationFailed, this, &NotesManager::onNoteCreationFailed);
    connect(m_client, &NoteClient::noteOpenSuccess, this, &NotesManager::onNoteOpenSuccess);
    connect(m_client, &NoteClient::noteOpenFailed, this, &NotesManager::onNoteOpenFailed);
    connect(m_client, &NoteClient::updateTextSuccess, this, &NotesManager::onUpdateTextSuccess);
    connect(m_client, &NoteClient::updateTextFailed, this, &NotesManager::onUpdateTextFailed);
    connect(m_client, &NoteClient::createSharedNote, this, &NotesManager::onShareNoteNotification);
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

void NotesManager::onNoteCreationSuccess(const QString& title, uint32_t noteId, uint32_t ownerId, uint32_t version) {
    m_model->addPersonalNote(static_cast<int>(noteId), title, ownerId, version);
}

void NotesManager::onNoteCreationFailed(const QString& reason) {
    qWarning() << "Note creation failed:" << reason;
}

void NotesManager::onNoteOpenSuccess(uint32_t note_id, uint32_t version, const QString& text) {
    emit noteOpened(note_id, version, text);
}

void NotesManager::onNoteOpenFailed(uint32_t note_id, const QString& reason) {
    qWarning() << "NotesManager::onNoteOpenFailed - ID:" << note_id
               << "Reason:" << reason;

    // Можно показать ошибку пользователю
    emit noteOpenError(static_cast<int>(note_id), reason);
}

void NotesManager::onUpdateTextSuccess(uint32_t note_id, uint32_t user_id) {

}

void NotesManager::onUpdateTextFailed(const QString& reason) {

}

void NotesManager::openNote(int noteId, int version) {
    m_client->sendOpenNoteRequest(noteId, version);
}

void NotesManager::updateNote(int noteId, const QString& text, int version) {
    m_client->sendUpdateTextRequest(noteId, text, version);
}

void NotesManager::shareNoteWithEveryone(int noteId, int version) {
    m_client->sendShareNoteRequest(noteId, version);
}

void NotesManager::onShareNoteNotification(uint32_t note_id, const QString& title, uint32_t owner_id, uint32_t version) {
    m_model->addSharedNote(note_id, title, owner_id, version);
}