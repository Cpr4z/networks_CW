#pragma once

#include <QAbstractListModel>
#include <QString>
#include <vector>

struct NoteItem {
    int noteId;
    QString title;
    bool isShared = false;
    int ownerId = 0;
    int version = 0;
};

class NotesModel : public QAbstractListModel {
Q_OBJECT

public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        TitleRole,
        IsSharedRole,
        OwnerIdRole,
        VersionRole,
    };

    Q_INVOKABLE QString getTitleById(int noteId) const;
    Q_INVOKABLE int personalNotesCount() const;
    Q_INVOKABLE int sharedNotesCount() const;
    Q_INVOKABLE bool isNoteShared(int noteId) const;


    explicit NotesModel(QObject* parent = nullptr);

    // QAbstractListModel interface
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    // API
    void addPersonalNote(int id, const QString& title, int ownerId, int version);
    void addSharedNote(int id, const QString& title, int ownerId, int version);
    const NoteItem& noteAt(int row) const;
//    void updateNoteVersion(uint32_t note_id, uint32_t version);

private:
    std::vector<NoteItem> m_notes;
};
