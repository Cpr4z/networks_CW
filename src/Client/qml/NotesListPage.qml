import QtQuick
import QtQuick.Controls

ApplicationWindow {
    id: window
    visible: true
    width: 600
    height: 400
    title: "Мои заметки"

    Column {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 10

        ListView {
            id: list
            model: notesManager.model

            delegate: Rectangle {
                width: parent.width
                height: 40
                border.width: 1

                Text {
                    anchors.centerIn: parent
                    text: title
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: notesManager.openNote(id)
                }
            }
        }

        Button {
            text: "Создать новую заметку"
            onClicked: createDialog.open()
        }

        Text {
            visible: list.count === 0
            text: "У вас пока нет заметок"
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }

    CreateNoteDialog {
        id: createDialog
        parent: window.contentItem
        anchors.centerIn: parent
    }

    Connections {
        target: notesManager
        function onNoteOpened(noteId, title, text) {
            var component = Qt.createComponent("qrc:/qml/NoteEditor.qml")
            if (component.status === Component.Ready) {
                var editor = component.createObject(window, {
                    noteId: noteId,
                    initialTitle: title,
                    initialText: text
                })
            }
        }
    }
}