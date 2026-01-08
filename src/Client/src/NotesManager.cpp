#include "NotesManager.hpp"

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
    connect(m_client, &NoteClient::createSyncDialog, this, &NotesManager::onCreateSyncDialog);
    connect(m_client, &NoteClient::createOwnerApproveDialog, this, &NotesManager::onCreateOwnerApproveDialog);
    connect(m_client, &NoteClient::getNotes, this, &NotesManager::onGetNotes);
    connect(m_client, &NoteClient::approveServerVersion, this, &NotesManager::onApproveServerVersion);
    connect(m_client, &NoteClient::updateTextMerged, this, &NotesManager::onUpdateTextMerged);
    connect(m_client, &NoteClient::autoMergedText, this, &NotesManager::onAutoMergedText);
}

QAbstractListModel* NotesManager::model() const {
    return m_model;
}

void NotesManager::createNote(const QString& title) {
    if (title.isEmpty()) {
        emit onNoteCreationFailed("Заметка не может быть создана с пустым названием");
        return;
    }
    m_client->sendCreateNoteRequest(title);
}

void NotesManager::syncNote(int noteId, const QString& base_version, const QString& local_version) {
    m_client->sendSyncNoteRequest(noteId, base_version, local_version);
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
    emit noteOpenError(static_cast<int>(note_id), reason);
}

void NotesManager::onUpdateTextSuccess(uint32_t note_id, uint32_t user_id, uint32_t version) {
    emit afterTextUpdated(static_cast<int>(version));
}

void NotesManager::onUpdateTextFailed(uint8_t reason) {
    if (static_cast<int>(reason) == 1) {
        emit noteUpdateConflict();
    }
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

void NotesManager::onCreateSyncDialog(const QString& server_text) {
    emit createSyncDialog(server_text);
}

void NotesManager::onCreateOwnerApproveDialog(uint32_t note_id, uint32_t merge_sender_id, const QString& approve_text) {
    emit createOwnerApproveDialog(note_id, merge_sender_id, approve_text);
}

void NotesManager::onGetNotes(const QMap<uint32_t, std::tuple<QString, bool, uint32_t, uint32_t>>& notes) {
    for (const auto& [note_id, note_info] : notes.asKeyValueRange()) {
        const auto& [title, is_shared, version, owner_id] = note_info;
        if (is_shared) {
            m_model->addSharedNote(note_id, title, owner_id, version);
        } else {
            m_model->addPersonalNote(note_id, title, owner_id, version);
        }
    }
}

void NotesManager::onApproveServerVersion(uint32_t note_id, uint32_t version, const QString& server_version) {
    emit serverVersionAccepted(note_id, version, server_version);
}

void NotesManager::onUpdateTextMerged(uint32_t note_id, uint32_t version, const QString& merged_version) {
    emit updateTextMerged(note_id, version, merged_version);
}

void NotesManager::onAutoMergedText(uint32_t note_id, uint32_t version, const QString& auto_merged_version) {
    emit autoMergedText(note_id, version, auto_merged_version);
}

void NotesManager::ownerApprove(int noteId, const QString& merged_version) {
    m_client->sendApproveMergeRequest(noteId, 0, merged_version);
}

void NotesManager::serverApprove(int noteId) {
    m_client->sendApproveMergeRequest(noteId, 1, "");
}

void NotesManager::getNotes() {
    m_client->sendGetNotesRequest();
}

void NotesManager::approveMerge(int noteId, const QString& approved_merge, int version, int merge_sender_id) {
    m_client->sendOwnerApproveMergeResponse(noteId, version, merge_sender_id, 0, approved_merge);
}

void NotesManager::rejectMerge(int noteId, const QString& owner_version, int version, int merge_sender_id) {
    m_client->sendOwnerApproveMergeResponse(noteId, version, merge_sender_id, 1, owner_version);
}