import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

ApplicationWindow {
    id: noteWindow
    visible: true
    width: 800
    height: 600
    title: initialTitle ? initialTitle : "Новая заметка"

    property int noteId: -1
    property string initialTitle: ""
    property string initialText: ""
    property bool isNoteShared: false
    property int localVersion: 0  // Локальная версия заметки
    property int serverVersion: 0 // Версия на сервере
    property bool conflictDetected: false // Флаг конфликта

    // Флаг изменений для предупреждения при закрытии
    property bool hasUnsavedChanges: false

    // Сравниваем текущий текст с исходным
    function checkForChanges() {
        hasUnsavedChanges = (titleField.text !== initialTitle || textArea.text !== initialText)
    }

    // Сохраняем заметку
    function saveNote() {
        if (noteId === -1) return

        // if (localVersion < serverVersion) {
        //     console.log(localVersion);
        //     console.log(serverVersion);
        //     conflictDetected = true
        //     conflictDialog.localContent = textArea.text
        //     conflictDialog.serverVersion = serverVersion
        //     conflictDialog.open()
        //     return
        // }

        notesManager.updateNote(noteId, textArea.text, localVersion)
        hasUnsavedChanges = false

        // Обновляем заголовок окна
        noteWindow.title = titleField.text
    }

    header: ToolBar {
        RowLayout {
            anchors.fill: parent

            // Кнопка назад/закрыть
            ToolButton {
                text: "←"
                font.pixelSize: 18
                onClicked: {
                    if (hasUnsavedChanges) {
                        unsavedChangesDialog.open()
                    } else {
                        noteWindow.close()
                    }
                }

                ToolTip.visible: hovered
                ToolTip.text: "Назад к списку заметок"
            }

            // Название заметки (редактируемое)
            TextField {
                id: titleField
                Layout.fillWidth: true
                text: initialTitle
                font.pixelSize: 18
                font.bold: true
                placeholderText: "Введите название заметки..."
                maximumLength: 100

                onTextChanged: {
                    checkForChanges()
                    // Обновляем заголовок окна в реальном времени
                    noteWindow.title = text ? text : "Без названия"
                }
            }

            // Кнопка сохранения
            ToolButton {
                id: saveButton
                text: "💾"
                font.pixelSize: 18
                enabled: hasUnsavedChanges
                onClicked: saveNote()

                ToolTip.visible: hovered
                ToolTip.text: "Сохранить заметку"

                contentItem: Text {
                    text: parent.text
                    font.pixelSize: 18
                    color: parent.enabled ? "black" : "gray"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                background: Rectangle {
                    color: "transparent"
                    border.width: hasUnsavedChanges ? 1 : 0
                    border.color: "green"
                    radius: 3
                }
            }

            // Кнопка дополнительных действий
            ToolButton {
                text: "⋮"
                font.pixelSize: 18
                onClicked: contextMenu.open()

                Menu {
                    id: contextMenu
                    y: parent.height

                    MenuItem {
                        text: "Удалить заметку"
                        onTriggered: deleteDialog.open()
                    }

                    MenuSeparator {}

                    MenuItem {
                        text: isNoteShared ? "👥 Расшарена" : "👤 Поделиться со всеми"
                        enabled: !isNoteShared  // Неактивна если уже расшарена
                        onTriggered: {
                            shareConfirmDialog.open();
                        }
                    }

                    // MenuItem {
                    //     text: "Поделиться заметкой..."
                    //     onTriggered: shareDialog.open()  // ← Открываем диалог расшаривания
                    // }
                }
            }
        }
    }

    // Основная область с текстом
    ScrollView {
        anchors.fill: parent
        anchors.margins: 16

        TextArea {
            id: textArea
            width: parent.width
            placeholderText: "Начните вводить текст заметки..."
            text: initialText
            wrapMode: TextEdit.Wrap
            font.pixelSize: 14
            selectByMouse: true
            persistentSelection: true

            onTextChanged: checkForChanges()
        }
    }

    // Статус бар внизу
    footer: ToolBar {
        height: 30

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 16
            anchors.rightMargin: 16

            // Индикатор сохранения
            Text {
                id: statusText
                text: hasUnsavedChanges ? "● Не сохранено" : "✓ Сохранено"
                color: hasUnsavedChanges ? "orange" : "green"
                font.pixelSize: 12
            }

            // Счетчик символов
            Text {
                text: textArea.text.length + " символов"
                font.pixelSize: 12
                color: "gray"
            }

            Item { Layout.fillWidth: true }

            // Дата последнего изменения
            Text {
                text: "Изменено: только что"
                font.pixelSize: 12
                color: "gray"
            }
        }
    }

    // Диалог при закрытии с несохраненными изменениями
    Dialog {
        id: unsavedChangesDialog
        anchors.centerIn: parent
        title: "Несохраненные изменения"
        standardButtons: Dialog.Save | Dialog.Discard | Dialog.Cancel

        Label {
            text: "У вас есть несохраненные изменения. Хотите сохранить их перед закрытием?"
            wrapMode: Text.WordWrap
        }

        onAccepted: {
            saveNote()
            noteWindow.close()
        }

        onDiscarded: {
            noteWindow.close()
        }
    }

    // Диалог подтверждения удаления
    Dialog {
        id: deleteDialog
        title: "Удалить заметку"
        standardButtons: Dialog.Ok | Dialog.Cancel

        Label {
            text: "Вы уверены, что хотите удалить эту заметку?\nДействие нельзя будет отменить."
            wrapMode: Text.WordWrap
        }

        onAccepted: {
            // TODO: Добавить метод удаления заметки
            console.log("Deleting note:", noteId)
            noteWindow.close()
        }
    }

    Dialog {
        id: shareConfirmDialog
        anchors.centerIn: parent
        title: "Поделиться заметкой"
        standardButtons: Dialog.Ok | Dialog.Cancel

        Label {
            width: parent.width
            wrapMode: Text.WordWrap
            text: "Заметка <b>\"" + titleField.text + "\"</b> станет доступна всем пользователям."
        }

        onAccepted: {
            console.log("Sharing note:", noteId);
            notesManager.shareNoteWithEveryone(noteId, localVersion);
        }
    }

    ConflictDialog {
        id: conflictDialog
        anchors.centerIn: parent
        noteId: noteWindow.noteId
        noteTitle: titleField.text

        onAcceptServer: {
            // Пользователь выбрал принять серверную версию
            notesManager.updateNote(noteId, conflictDialog.serverContent, serverVersion)
            conflictDetected = false
            localVersion = serverVersion
            textArea.text = conflictDialog.serverContent
            hasUnsavedChanges = false
        }

        // onOverwriteServer: {
        //     // Пользователь выбрал перезаписать сервер
        //     conflictDetected = false
        //     localVersion++ // Увеличиваем версию
        //     notesManager.forceUpdateNote(noteId, content, localVersion)
        //     hasUnsavedChanges = false
        // }

        onMergeManually: {
            // Пользователь выбрал ручное слияние
            conflictDetected = false
            localVersion++ // Увеличиваем версию
            textArea.text = content // Текст после ручного редактирования
            notesManager.ownerApprove(noteId, content);
            // notesManager.forceUpdateNote(noteId, content, localVersion)
            hasUnsavedChanges = false
        }
    }

    Popup {
        id: shareSuccessPopup
        anchors.centerIn: parent
        width: 300
        height: 120
        modal: true

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 20

            RowLayout {
                Layout.fillWidth: true

                Text {
                    text: "✓"
                    font.pixelSize: 24
                    color: "green"
                }

                Label {
                    Layout.fillWidth: true
                    text: "Заметка расшарена!"
                    font.bold: true
                }
            }

            Label {
                Layout.fillWidth: true
                text: "Теперь все пользователи могут видеть и редактировать эту заметку."
                wrapMode: Text.WordWrap
                color: "gray"
            }

            Button {
                Layout.alignment: Qt.AlignHCenter
                text: "OK"
                onClicked: shareSuccessPopup.close()
            }
        }
    }

    Connections {
        target: notesManager

        function onNoteUpdateConflict() {
            notesManager.syncNote(noteId);
        }

        function onCreateSyncDialog(server_text) {
            conflictDialog.localContent = textArea.text;
            conflictDialog.serverContent = server_text;
            conflictDialog.open()
        }
    }

    // Connections {
        // target: notesManager

        // Когда приходит обновление от сервера
        // onServerVersionChanged: function(noteId, content, version, sender) {
        //     if (noteId !== noteWindow.noteId) return
        //
        //     serverVersion = version
        //
        //     if (!conflictDetected) {
        //         if (localVersion < serverVersion && hasUnsavedChanges) {
        //             // Активный конфликт - показываем диалог
        //             conflictDialog.localContent = textArea.text
        //             conflictDialog.serverContent = content
        //             conflictDialog.serverVersion = version
        //             conflictDialog.open()
        //         } else if (localVersion < serverVersion) {
        //             // Просто обновляем, если нет локальных изменений
        //             textArea.text = content
        //             localVersion = serverVersion
        //         }
        //     }
        // }
    // }

    // Горячие клавиши - ИСПРАВЛЕНО
    Shortcut {
        sequences: [StandardKey.Save, "Ctrl+S"]
        onActivated: saveNote()
    }

    Shortcut {
        sequence: "Esc"
        onActivated: {
            if (hasUnsavedChanges) {
                unsavedChangesDialog.open()
            } else {
                noteWindow.close()
            }
        }
    }

    // При открытии фокусируемся на тексте
    Component.onCompleted: {
        textArea.forceActiveFocus()
        textArea.cursorPosition = textArea.text.length
    }

    // Предупреждение при закрытии окна
    onClosing: {
        if (hasUnsavedChanges) {
            close.accepted = false
            unsavedChangesDialog.open()
        }
    }
}