import QtQuick
import QtQuick.Controls

ApplicationWindow {
    visible: true
    width: 600
    height: 400
    title: "Мои заметки"

    property string userId

    Column {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 10

        ListView {
            id: list
            model: notesModel
            delegate: Rectangle {
                width: parent.width
                height: 40
                border.width: 1
                Text {
                    anchors.centerIn: parent
                    text: model.title
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: notesModel.openNote(model.id)
                }
            }
        }

        Button {
            text: "Создать новую заметку"
            onClicked: {
                console.log("Создание новой заметки")

                var component = Qt.createComponent("qrc:/qml/NoteEditor.qml")
                if (component.status === Component.Ready) {
                    var editor = component.createObject(parent) // Важно: parent = текущий ApplicationWindow
                    if (editor === null) {
                        console.log("Ошибка создания NoteEditor:", component.errorString())
                    } else {
                        // Можно скрыть текущий контент
                        list.visible = false
                        visible = false
                    }
                } else if (component.status === Component.Error) {
                    console.log("Ошибка загрузки NoteEditor:", component.errorString())
                }
            }
        }

        Text {
            visible: list.count === 0
            text: "У вас пока нет заметок"
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}