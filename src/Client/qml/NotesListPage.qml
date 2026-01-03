import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: window
    visible: true
    width: 600
    height: 500
    title: "Мои заметки"

    property bool showOnlyPersonal: false
    property bool showOnlyShared: false
    property int currentFilter: 0
    property int userId: -1

    Component.onCompleted: {
        if (notesManager && userId > 0) {
            notesManager.getNotes();
        } else {
            console.warn("Cannot load notes: notesManager =", notesManager, ", userId =", window.userId);
        }
    }

    function shouldShowItem(isShared) {
        if (currentFilter === 0) return true;
        if (currentFilter === 1) return !isShared;
        if (currentFilter === 2) return isShared;
        return true;
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 10

        RowLayout {
            Layout.fillWidth: true

            Text {
                text: "📝 Заметки"
                font.pixelSize: 18
                font.bold: true
            }

            Item { Layout.fillWidth: true }

            Row {
                spacing: 15

                Text {
                    text: "Личные: " + (notesManager ? notesManager.model.personalNotesCount() : 0)
                    color: currentFilter === 1 ? "blue" : "gray"
                }

                Text {
                    text: "Общие: " + (notesManager ? notesManager.model.sharedNotesCount() : 0)
                    color: currentFilter === 2 ? "blue" : "gray"
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 5

            ButtonGroup {
                id: filterGroup
                buttons: filterRow.children
            }

            Row {
                id: filterRow
                spacing: 5

                RadioButton {
                    text: "Все"
                    checked: true
                    onCheckedChanged: if (checked) currentFilter = 0

                    background: Rectangle {
                        color: parent.checked ? "#e0e0ff" : "transparent"
                        border.width: 1
                        border.color: parent.checked ? "blue" : "lightgray"
                        radius: 4
                    }
                }

                RadioButton {
                    text: "Личные"
                    onCheckedChanged: if (checked) currentFilter = 1

                    background: Rectangle {
                        color: parent.checked ? "#e0e0ff" : "transparent"
                        border.width: 1
                        border.color: parent.checked ? "blue" : "lightgray"
                        radius: 4
                    }
                }

                RadioButton {
                    text: "Общие"
                    onCheckedChanged: if (checked) currentFilter = 2

                    background: Rectangle {
                        color: parent.checked ? "#e0e0ff" : "transparent"
                        border.width: 1
                        border.color: parent.checked ? "blue" : "lightgray"
                        radius: 4
                    }
                }
            }

            Item { Layout.fillWidth: true }

            Button {
                text: "🔄 Общие"
                visible: currentFilter !== 1
                onClicked: notesManager.loadSharedNotes()

                ToolTip.visible: hovered
                ToolTip.text: "Загрузить общие заметки"

                background: Rectangle {
                    color: parent.hovered ? "#d0e0ff" : "#e8f0ff"
                    border.width: 1
                    border.color: "lightblue"
                    radius: 4
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: "lightgray"
        }

        ListView {
            id: list
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: notesManager ? notesManager.model : null
            clip: true
            spacing: 5

            delegate: Loader {
                width: list.width
                height: shouldShowItem(isShared) ? 50 : 0
                active: shouldShowItem(isShared)

                sourceComponent: noteDelegate

                property var modelData: model

                property int noteId: model.noteId
                property bool isShared: model.isShared
                property string title: model.title
                property int ownerId: model.ownerId
                property int version: model.version
            }

            Component {
                id: noteDelegate

                Rectangle {
                    id: noteItem
                    width: list.width
                    height: 50
                    border.width: 1
                    border.color: isShared ? "lightblue" : "lightgray"
                    radius: 4

                    color: {
                        if (isShared) {
                            return ownerId === 0 ? "#f0f8ff" : "#fff8f0"; // Голубой если моя, бежевый если чужая
                        }
                        return "white";
                    }

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 10

                        Text {
                            text: isShared ? "👥" : "📝"
                            font.pixelSize: 16
                        }

                        Text {
                            Layout.fillWidth: true
                            text: title
                            font.pixelSize: 14
                            font.bold: true
                            elide: Text.ElideRight
                            color: isShared ? "blue" : "black"
                        }

                        Text {
                            text: {
                                if (!isShared) return "";
                                if (ownerId === 0) return "Вы поделились";
                                return "ID:" + ownerId;
                            }
                            font.pixelSize: 11
                            color: "gray"
                            visible: isShared
                        }

                        Button {
                            text: "Открыть"
                            onClicked: notesManager.openNote(noteId, version)

                            background: Rectangle {
                                color: parent.hovered ?
                                    (isShared ? "#d0e0ff" : "#e0e0e0") :
                                    "transparent"
                                border.width: 1
                                border.color: isShared ? "lightblue" : "lightgray"
                                radius: 3
                            }
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.LeftButton | Qt.RightButton

                        onClicked: () => {
                            notesManager.openNote(noteId, version);
                        }

                        onPressAndHold: {
                            if (isShared) {
                                if (ownerId === 0) {
                                    sharedByMeMenu.popup();
                                } else {
                                    sharedWithMeMenu.popup();
                                }
                            } else {
                                personalNoteMenu.popup();
                            }
                        }
                    }

                    Menu {
                        id: personalNoteMenu

                        MenuItem {
                            text: "Поделиться со всеми"
                            onTriggered: {
                                notesManager.shareNoteWithEveryone(noteId, version);
                            }
                        }

                        MenuSeparator {}

                        MenuItem {
                            text: "Удалить"
                        }
                    }

                    Menu {
                        id: sharedByMeMenu

                        MenuItem {
                            text: "Отменить расшаривание"
                        }

                        MenuSeparator {}

                        MenuItem {
                            text: "Копировать ссылку"
                        }
                    }

                    Menu {
                        id: sharedWithMeMenu

                        MenuItem {
                            text: "Сделать копию"
                        }

                        MenuItem {
                            text: "Отписаться"
                        }
                    }
                }
            }

            Label {
                anchors.centerIn: parent
                text: {
                    if (!notesManager || !notesManager.model) return "Нет данных";
                    if (currentFilter === 0 && list.count === 0) return "Нет заметок";
                    if (currentFilter === 1 && notesManager.model.personalNotesCount === 0)
                        return "Нет личных заметок";
                    if (currentFilter === 2 && notesManager.model.sharedNotesCount === 0)
                        return "Нет общих заметок";
                    return "";
                }
                font.pixelSize: 16
                color: "gray"
                visible: text !== ""
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            Button {
                text: "➕ Создать заметку"
                onClicked: createDialog.open()

                background: Rectangle {
                    color: parent.hovered ? "#e0ffe0" : "#f0fff0"
                    border.width: 1
                    border.color: "lightgreen"
                    radius: 4
                }
            }

            Item { Layout.fillWidth: true }

            Text {
                text: {
                    var total = list.count;
                    var shown = 0;
                    for (var i = 0; i < total; i++) {
                        if (shouldShowItem(notesManager.model.isNoteShared(i))) {
                            shown++;
                        }
                    }
                    return "Показано: " + shown + " из " + total;
                }
                font.pixelSize: 12
                color: "gray"
                visible: currentFilter !== 0 && list.count > 0
            }
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

        function onNoteOpened(noteId, version, text) {
            var title = notesManager.model.getTitleById(noteId);
            var isShared = notesManager.model.isNoteShared(noteId);

            var component = Qt.createComponent("qrc:/qml/NoteEditor.qml")
            if (component.status === Component.Ready) {
                var editor = component.createObject(window.contentItem, {
                    noteId: noteId,
                    initialTitle: title,
                    initialText: text,
                    isNoteShared: isShared,
                    localVersion: version
                })
            } else {
                console.error("Failed to load NoteEditor:", component.errorString())
            }
        }
    }
}