#include "NotesModel.hpp"

NotesModel::NotesModel(QObject* parent)
        : QAbstractListModel(parent) {}

int NotesModel::rowCount(const QModelIndex&) const {
    return static_cast<int>(m_notes.size());
}

QVariant NotesModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= rowCount())
        return {};

    const auto& note = m_notes[index.row()];

    switch (role) {
        case IdRole: return note.id;
        case TitleRole: return note.title;
        default: return {};
    }
}

QHash<int, QByteArray> NotesModel::roleNames() const {
    return {
            { IdRole, "id" },
            { TitleRole, "title" }
    };
}

void NotesModel::addNote(int id, const QString& title) {
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_notes.push_back({ id, title });
    endInsertRows();
}

const NoteItem& NotesModel::noteAt(int row) const {
    return m_notes.at(row);
}
