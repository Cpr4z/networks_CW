#include <QObject>

class ClientWindow : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString content READ content WRITE setContent NOTIFY contentChanged)
public:
    QString content() const { return m_content; }
    void setContent(const QString& c) { if (m_content != c) { m_content = c; emit contentChanged(); } }

public slots:
    void insertText(int pos, const QString& text);
    void deleteRange(int start, int end);
signals:
    void contentChanged();

private:
    QString m_content;
};
