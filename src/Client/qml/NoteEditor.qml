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
    property int localVersion: 0
    property int serverVersion: 0
    property bool conflictDetected: false

    property bool hasUnsavedChanges: false

    property Timer changeTimer: Timer {
        interval: 300
        onTriggered: checkForChanges()
    }

    function checkForChanges() {
        hasUnsavedChanges = (titleField.text !== initialTitle || textArea.text !== initialText)
    }

    function saveNote() {
        if (noteId === -1) return
        notesManager.updateNote(noteId, textArea.text, localVersion)
        hasUnsavedChanges = false

        noteWindow.title = titleField.text
    }

    header: ToolBar {
        RowLayout {
            anchors.fill: parent

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

            TextField {
                id: titleField
                Layout.fillWidth: true
                text: initialTitle
                font.pixelSize: 18
                font.bold: true
                placeholderText: "Введите название заметки..."
                maximumLength: 100

                onTextChanged: {
                    changeTimer.restart()
                }

                // onTextChanged: {
                //     checkForChanges()
                //     noteWindow.title = text ? text : "Без названия"
                // }
            }

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
                        enabled: !isNoteShared
                        onTriggered: {
                            shareConfirmDialog.open();
                        }
                    }
                }
            }
        }
    }

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

            // property Timer changeTimer: Timer {
            //     interval: 300
            //     onTriggered: checkForChanges()
            // }

            onTextChanged: {
                changeTimer.restart()
            }

            // onTextChanged: checkForChanges()
        }
    }

    footer: ToolBar {
        height: 30

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 16
            anchors.rightMargin: 16

            Text {
                id: statusText
                text: hasUnsavedChanges ? "● Не сохранено" : "✓ Сохранено"
                color: hasUnsavedChanges ? "orange" : "green"
                font.pixelSize: 12
            }

            Text {
                text: textArea.text.length + " символов"
                font.pixelSize: 12
                color: "gray"
            }

            Item { Layout.fillWidth: true }

            Text {
                text: "Изменено: только что"
                font.pixelSize: 12
                color: "gray"
            }
        }
    }

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
            notesManager.serverApprove(conflictDialog.noteId)
            // conflictDetected = false
            // localVersion = serverVersion
            // textArea.text = conflictDialog.serverContent
            // hasUnsavedChanges = false
        }

        onMergeManually: {
            // Пользователь выбрал ручное слияние
            conflictDetected = false
            localVersion++ // Увеличиваем версию
            textArea.text = localContent // Текст после ручного редактирования
            notesManager.ownerApprove(noteId, localContent);
            // notesManager.forceUpdateNote(noteId, content, localVersion)
            hasUnsavedChanges = false
        }
    }

    ApproveMergeDialog {
        id: approveMergeDialog
        anchors.centerIn: parent

        onAcceptMerge: function(noteId, text, version) {
            // Владелец принял слияние
            console.log("Owner accepted merge for note:", noteId)
            notesManager.approveMerge(noteId, text, version)

            // Обновляем локальный текст и версию
            textArea.text = text
            localVersion = version
            serverVersion = version
            hasUnsavedChanges = false
        }

        onRejectMerge: function(noteId) {
            console.log("Owner rejected merge for note:", noteId)
            notesManager.rejectMerge(noteId)
        }

        onRequestChanges: function(noteId, feedback) {
            console.log("Owner requested changes for note:", noteId, "Feedback:", feedback)
            notesManager.requestMergeChanges(noteId, feedback)
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

        function onAfterTextUpdated(version) {
            noteWindow.localVersion = version;
        }

        function onServerVersionAccepted(noteId, version, server_text) {
            if (noteId !== noteWindow.noteId)
                return

            textArea.text = server_text
            noteWindow.initialText = server_text
            noteWindow.localVersion = version
            noteWindow.serverVersion = version
            noteWindow.hasUnsavedChanges = false
            noteWindow.conflictDetected = false

            conflictDialog.close()
        }

        function onCreateOwnerApproveDialog(noteId, merge_sender_id, approve_text) {

        }

        // function onCreateAcceptMergeDialog(suggested_text) {
        //
        // }
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

    Component.onCompleted: {
        textArea.forceActiveFocus()
        textArea.cursorPosition = textArea.text.length
    }

    onClosing: {
        if (hasUnsavedChanges) {
            close.accepted = false
            unsavedChangesDialog.open()
        }
    }
}