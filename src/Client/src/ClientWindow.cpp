#include "ClientWindow.hpp"
#include <QDebug>

void ClientWindow::insertText(int pos, const QString& text) {
    if (pos < 0 || pos > m_content.size()) {
        qWarning() << "Invalid insert position:" << pos;
        return;
    }

    m_content.insert(pos, text);
    emit contentChanged();
}

void ClientWindow::deleteRange(int start, int end) {
    if (start < 0 || end > m_content.size() || start >= end) {
        qWarning() << "Invalid delete range:" << start << end;
        return;
    }

    m_content.remove(start, end - start);
    emit contentChanged();
}
