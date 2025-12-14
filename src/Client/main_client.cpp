#include "src/Client.hpp"

#include "src/AuthManager.hpp"
//#include "src/NotesModel.hpp"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

int main(int argc, char** argv) {
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    ClientPtr client = std::make_unique<NoteClient>();

    AuthManager auth(client.get());
//    NotesModel notes(client.get());

    engine.rootContext()->setContextProperty("authManager", &auth);
//    engine.rootContext()->setContextProperty("notesModel", &notes);

    engine.load(QUrl(QStringLiteral("qrc:/qml/LoginPage.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
