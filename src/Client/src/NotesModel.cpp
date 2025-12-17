#include "NotesModel.hpp"

#include <iostream>

NotesModel::NotesModel(QObject* parent)
        : QAbstractListModel(parent) {
}

int NotesModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid())
        return 0;

    return static_cast<int>(m_notes.size());
}

QVariant NotesModel::data(const QModelIndex& index, int role) const {
    const auto& note = m_notes[index.row()];

    switch (role) {
        case IdRole: return note.noteId;
        case TitleRole: return note.title;
        default: return {};
    }
}

QString NotesModel::getTitleById(int noteId) const {
    for (const auto& note : m_notes) {
        std::cout << "Title of note with id "<< noteId << note.title.toStdString()   << std::endl;
        if (note.noteId == noteId) {
            return note.title;
        }
    }
    return QString();
}

QHash<int, QByteArray> NotesModel::roleNames() const {
    return {
            { IdRole, "noteId" },
            { TitleRole, "title" }
    };
}

void NotesModel::addNote(int id, const QString& title) {
    const int row = m_notes.size();
    beginInsertRows(QModelIndex(), row, row);
    m_notes.push_back({ id, title });
//    std::cout << m_notes.size() << " " << this <<  std::endl;
    endInsertRows();
}

const NoteItem& NotesModel::noteAt(int row) const {
    return m_notes.at(row);
}
