#pragma once
#include <QObject>
#include <QAbstractListModel>

#include "Client.hpp"
#include "NotesModel.hpp"


class NotesManager : public QObject {
Q_OBJECT
    Q_PROPERTY(QAbstractListModel* model READ model CONSTANT)

public:
    explicit NotesManager(NoteClient* client, QObject* parent = nullptr);

    QAbstractListModel* model() const;

    Q_INVOKABLE void createNote(const QString& title);
    Q_INVOKABLE void openNote(int noteId, int version);
    Q_INVOKABLE void updateNote(int noteId, const QString& text, int version);
    Q_INVOKABLE void shareNoteWithEveryone(int noteId, int version);
    Q_INVOKABLE void syncNote(int noteId);
    Q_INVOKABLE void ownerApprove(int noteId, const QString& merged_version);
    Q_INVOKABLE void serverApprove(int noteId);
    Q_INVOKABLE void ownerApproveResult(int noteId, int merge_sender_id, int merge_result, const QString& merged_text);
    Q_INVOKABLE void getNotes();

private slots:

    void onNoteCreationSuccess(const QString& title, uint32_t noteId, uint32_t ownerId, uint32_t version);
    void onNoteCreationFailed(const QString& reason);

    void onNoteOpenSuccess(uint32_t note_id, uint32_t version, const QString& text);
    void onNoteOpenFailed(uint32_t note_id, const QString& reason);

    void onUpdateTextSuccess(uint32_t note_id, uint32_t user_id, uint32_t version);
    void onUpdateTextFailed(uint8_t reason);

    void onShareNoteNotification(uint32_t note_id, const QString& title, uint32_t owner_id, uint32_t version);

    void onCreateSyncDialog(const QString& server_text);

    void onCreateOwnerApproveDialog(uint32_t note_id, uint32_t merge_sender_id, const QString& approve_version);

    void onGetNotes(const QMap<uint32_t, std::tuple<QString, bool, uint32_t, uint32_t>>& notes);

    void onApproveServerVersion(uint32_t note_id, const QString& server_version);

signals:
    void noteOpened(int note_id, int version, const QString& text);
    void noteOpenError(int noteId, const QString& error);
    void noteUpdateConflict();
    void createSyncDialog(const QString& server_text);
    void createOwnerApproveDialog(int note_id, int merge_sender_id, const QString& approve_version);
    void afterTextUpdated(int version);


private:
    NotesModel* m_model;
    NoteClient* m_client;
    uint32_t m_userId;
};