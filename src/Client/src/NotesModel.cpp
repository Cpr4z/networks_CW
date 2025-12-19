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
        case IsSharedRole: return note.isShared;
        case OwnerIdRole: return note.ownerId;
        case VersionRole: return note.version;
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
            { TitleRole, "title" },
            { IsSharedRole, "isShared"},
            { OwnerIdRole, "ownerId"}
    };
}

void NotesModel::addPersonalNote(int id, const QString& title, int ownerId, int version) {
    const int row = m_notes.size();
    beginInsertRows(QModelIndex(), row, row);
    m_notes.push_back({ id, title, false, ownerId, version });
    endInsertRows();
}

void NotesModel::addSharedNote(int id, const QString& title, int ownerId, int version) {
    const int row = m_notes.size();
    beginInsertRows(QModelIndex(), row, row);
    m_notes.push_back({ id, title, true, ownerId, version });
    endInsertRows();
}

const NoteItem& NotesModel::noteAt(int row) const {
    return m_notes.at(row);
}

int NotesModel::personalNotesCount() const {
    return std::count_if(m_notes.begin(), m_notes.end(),
                         [](const NoteItem& note) { return !note.isShared; });
}

int NotesModel::sharedNotesCount() const {
    return std::count_if(m_notes.begin(), m_notes.end(),
                         [](const NoteItem& note) { return note.isShared; });
}

bool NotesModel::isNoteShared(int noteId) const {
    for (const auto& note : m_notes) {
        if (note.noteId == noteId) {
            return note.isShared;
        }
    }
    return false;
}
