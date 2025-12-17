#pragma once

#include <QAbstractListModel>
#include <QString>
#include <vector>

struct NoteItem {
    int noteId;
    QString title;
};

class NotesModel : public QAbstractListModel {
Q_OBJECT

public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        TitleRole
    };

    Q_INVOKABLE QString getTitleById(int noteId) const;
    explicit NotesModel(QObject* parent = nullptr);

    // QAbstractListModel interface
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    // API
    void addNote(int id, const QString& title);
    const NoteItem& noteAt(int row) const;

private:
    std::vector<NoteItem> m_notes;
};
