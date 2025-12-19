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

private slots:

    void onNoteCreationSuccess(const QString& title, uint32_t noteId, uint32_t ownerId, uint32_t version);
    void onNoteCreationFailed(const QString& reason);

    void onNoteOpenSuccess(uint32_t note_id, uint32_t version, const QString& text);
    void onNoteOpenFailed(uint32_t note_id, const QString& reason);

    void onUpdateTextSuccess(uint32_t note_id, uint32_t user_id);
    void onUpdateTextFailed(const QString& reason);

    void onShareNoteNotification(uint32_t note_id, const QString& title, uint32_t owner_id, uint32_t version);

signals:
    void noteOpened(int note_id, uint32_t  version, const QString& text);
    void noteOpenError(int noteId, const QString& error);

private:
    NotesModel* m_model;
    NoteClient* m_client;
    uint32_t m_userId;
};