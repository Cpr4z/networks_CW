import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: window
    visible: true
    width: 600
    height: 500
    title: "Мои заметки"

    // Свойства для управления отображением
    property bool showOnlyPersonal: false
    property bool showOnlyShared: false
    property int currentFilter: 0  // 0: все, 1: личные, 2: общие

    // Функция для проверки, должен ли элемент отображаться
    function shouldShowItem(isShared) {
        if (currentFilter === 0) return true;  // все
        if (currentFilter === 1) return !isShared;  // только личные
        if (currentFilter === 2) return isShared;   // только общие
        return true;
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 10

        // Заголовок и статистика
        RowLayout {
            Layout.fillWidth: true

            Text {
                text: "📝 Заметки"
                font.pixelSize: 18
                font.bold: true
            }

            Item { Layout.fillWidth: true }

            // Статистика
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

        // Фильтры
        RowLayout {
            Layout.fillWidth: true
            spacing: 5

            // Кнопки фильтров
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

            // Кнопка загрузки общих заметок
            Button {
                text: "🔄 Общие"
                visible: currentFilter !== 1  // Не показываем когда фильтр "Личные"
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

        // Разделитель
        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: "lightgray"
        }

        // Список заметок
        ListView {
            id: list
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: notesManager ? notesManager.model : null
            clip: true
            spacing: 5

            // Делегат с условным отображением
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

                    // Разный фон в зависимости от типа
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

                        // Иконка типа заметки
                        Text {
                            text: isShared ? "👥" : "📝"
                            font.pixelSize: 16
                        }

                        // Заголовок
                        Text {
                            Layout.fillWidth: true
                            text: title
                            font.pixelSize: 14
                            font.bold: true
                            elide: Text.ElideRight
                            color: isShared ? "blue" : "black"
                        }

                        // Индикатор владельца для общих заметок
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

                        // Кнопка открытия
                        Button {
                            text: "Открыть"
                            onClicked: notesManager.openNote(noteId)

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

                    // Контекстное меню по правому клику или долгому нажатию
                    MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.LeftButton | Qt.RightButton

                        onClicked: () => {
                            // console.log("Trying to open note with id", id);
                            console.log("Trying to open note with id", noteId);
                            notesManager.openNote(noteId);
                        }

                        onPressAndHold: {
                            if (isShared) {
                                if (ownerId === 0) {
                                    // Это моя расшаренная заметка
                                    sharedByMeMenu.popup();
                                } else {
                                    // Это чужая расшаренная заметка
                                    sharedWithMeMenu.popup();
                                }
                            } else {
                                // Личная заметка
                                personalNoteMenu.popup();
                            }
                        }
                    }

                    // Меню для личных заметок
                    Menu {
                        id: personalNoteMenu

                        MenuItem {
                            text: "Поделиться со всеми"
                            onTriggered: {
                                console.log("Sharing note:", noteId);
                                notesManager.shareNoteWithEveryone(noteId);
                            }
                        }

                        MenuSeparator {}

                        MenuItem {
                            text: "Удалить"
                            onTriggered: console.log("Delete:", noteId)
                        }
                    }

                    // Меню для заметок, которыми поделился я
                    Menu {
                        id: sharedByMeMenu

                        MenuItem {
                            text: "Отменить расшаривание"
                            onTriggered: console.log("Unshare:", noteId)
                        }

                        MenuSeparator {}

                        MenuItem {
                            text: "Копировать ссылку"
                            onTriggered: console.log("Copy link for:", noteId)
                        }
                    }

                    // Меню для заметок, которыми поделились со мной
                    Menu {
                        id: sharedWithMeMenu

                        MenuItem {
                            text: "Сделать копию"
                            onTriggered: console.log("Make copy of:", noteId)
                        }

                        MenuItem {
                            text: "Отписаться"
                            onTriggered: console.log("Unsubscribe from:", noteId)
                        }
                    }
                }
            }

            // Сообщение если нет заметок по выбранному фильтру
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

        // Нижняя панель с кнопками
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

            // Информация о фильтре
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

    // Connections
    Connections {
        target: notesManager

        function onNoteOpened(noteId, text) {
            var title = notesManager.model.getTitleById(noteId);
            var isShared = notesManager.model.isNoteShared(noteId);
            // console.log("Opening note:", noteId, "Title:", title, "Shared:", notesManager.model.isSharedByIndex?.(noteId));

            var component = Qt.createComponent("qrc:/qml/NoteEditor.qml")
            if (component.status === Component.Ready) {
                var editor = component.createObject(window.contentItem, {
                    noteId: noteId,
                    initialTitle: title,
                    initialText: text,
                    isNoteShared: isShared
                })
                console.log("Editor created:", editor)
            } else {
                console.error("Failed to load NoteEditor:", component.errorString())
            }
        }

        // function onNoteShared(noteId) {
        //     console.log("Note shared successfully:", noteId);
        //     showToast("Заметка расшарена!");
        // }
        //
        // function onSharedNotesLoaded() {
        //     console.log("Shared notes loaded");
        //     showToast("Общие заметки загружены");
        // }
    }

    // function showToast(message) {
    //     toastText.text = message;
    //     toastPopup.open();
    // }
    //
    // // Toast-уведомление
    // Popup {
    //     id: toastPopup
    //     anchors.bottom: parent.bottom
    //     anchors.horizontalCenter: parent.horizontalCenter
    //     width: 300
    //     height: 60
    //     opacity: 0.9
    //
    //     background: Rectangle {
    //         color: "#333"
    //         radius: 4
    //     }
    //
    //     Text {
    //         id: toastText
    //         anchors.centerIn: parent
    //         color: "white"
    //         font.pixelSize: 14
    //     }
    //
    //     Timer {
    //         interval: 2000
    //         running: toastPopup.visible
    //         onTriggered: toastPopup.close()
    //     }
    // }
}