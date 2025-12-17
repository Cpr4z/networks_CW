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

        // Информационный текст
        Text {
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
            text: list.count === 0 ? "У вас пока нет заметок" : "Мои заметки"
            font.pixelSize: 16
            font.bold: true
            visible: list.count === 0
        }

        // Список заметок
        ListView {
            id: list
            width: parent.width
            height: parent.height - 50  // Оставляем место для кнопки
            model: notesManager ? notesManager.model : null
            clip: true
            spacing: 5

            delegate: Rectangle {
                id: delegateItem
                width: list.width
                height: 40
                border.width: 1
                border.color: "lightgray"
                radius: 4

                Component.onCompleted: {
                    console.log("Created delegate for note id:", noteId)
                }

                Text {
                    anchors.centerIn: parent
                    text: title
                    font.pixelSize: 14
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: () => {
                        // console.log("Trying to open note with id", id);
                        console.log("Trying to open note with id", noteId);
                        notesManager.openNote(noteId);
                    }
                    // onClicked: notesManager.openNote(id)
                }
            }
        }

        // Кнопка создания заметки
        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Создать новую заметку"
            onClicked: createDialog.open()
        }
    }

    CreateNoteDialog {
        id: createDialog
        anchors.centerIn: parent

        onNoteAccepted: function(title) {
            notesManager.createNote(title)
        }
    }

    Connections {
        target: notesManager

        function onNoteOpened(noteId, text) {
            var title = notesManager.model.getTitleById(noteId);
            console.log("Opening editor for note:", noteId, "Title:", title);

            var component = Qt.createComponent("qrc:/qml/NoteEditor.qml")
            if (component.status === Component.Ready) {
                var editor = component.createObject(window.contentItem, {
                    noteId: noteId,
                    initialTitle: title,
                    initialText: text
                })
                console.log("Editor created:", editor)
            } else {
                console.error("Failed to load NoteEditor component:", component.errorString())
            }
        }

        function onNoteOpenError(noteId, error) {
            console.error("Failed to open note", noteId, ":", error)

            // Показываем сообщение об ошибке
            errorDialog.text = "Ошибка открытия заметки: " + error
            errorDialog.open()
        }
    }
}