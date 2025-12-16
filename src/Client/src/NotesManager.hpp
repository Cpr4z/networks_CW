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
    Q_INVOKABLE void openNote(int noteId);
    Q_INVOKABLE void updateNote(int noteId, const QString& text);

private slots:

    void onNoteCreationSuccess(const QString& title, uint32_t noteId);

    void onNoteCreationFailed(const QString& reason);

signals:
    void noteOpened(int noteId, QString title, QString text);

private:
    NotesModel* m_model;
    NoteClient* m_client;
    uint32_t m_userId;
};