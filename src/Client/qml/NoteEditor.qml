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

    // Флаг изменений для предупреждения при закрытии
    property bool hasUnsavedChanges: false

    // Сравниваем текущий текст с исходным
    function checkForChanges() {
        hasUnsavedChanges = (titleField.text !== initialTitle || textArea.text !== initialText)
    }

    // Сохраняем заметку
    function saveNote() {
        if (noteId === -1) return

        notesManager.updateNote(noteId, textArea.text)
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
                        text: "Экспортировать..."
                    }

                    MenuItem {
                        text: "Поделиться..."
                    }
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