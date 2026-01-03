#include "src/Client.hpp"

#include "src/AuthManager.hpp"
#include "src/NotesManager.hpp"
//#include "src/NotesModel.hpp"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>

int main(int argc, char** argv) {
    QGuiApplication app(argc, argv);

    QQuickStyle::setStyle("Fusion");
    QQmlApplicationEngine engine;

    ClientPtr client = std::make_unique<NoteClient>();

    AuthManager auth(client.get());
    NotesManager notesManager(client.get());

    engine.rootContext()->setContextProperty("authManager", &auth);
    engine.rootContext()->setContextProperty("notesManager", &notesManager);

    engine.load(QUrl(QStringLiteral("qrc:/qml/LoginPage.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
