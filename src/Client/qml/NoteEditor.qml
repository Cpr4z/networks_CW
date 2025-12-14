import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

Item {
    id: editorRoot
    anchors.fill: parent
    property alias noteTitle: titleField.text
    signal saved(string title, string content)

    Rectangle {
        anchors.fill: parent
        color: "white"
        border.color: "#cccccc"

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 16
            spacing: 10

            // 🔹 Заголовок заметки (редактируемое поле)
            RowLayout {
                Layout.fillWidth: true
                spacing: 8

                Label {
                    text: "Название:"
                    font.pixelSize: 16
                }

                TextField {
                    id: titleField
                    Layout.fillWidth: true
                    placeholderText: "Введите название заметки..."
                    font.bold: true
                    font.pixelSize: 16
                }
            }

            // 🔹 Основная зона редактирования
            TextArea {
                id: textEdit
                Layout.fillWidth: true
                Layout.fillHeight: true
                placeholderText: "Введите текст заметки..."
                wrapMode: TextEdit.Wrap
            }

            // 🔹 Кнопка назад
            Button {
                text: "Назад"
                Layout.alignment: Qt.AlignHCenter
                onClicked: editorRoot.destroy()
            }
        }
    }

    // 🔹 Обработка Ctrl+S
    Shortcut {
        sequence: StandardKey.Save
        onActivated: {
            if (titleField.text.trim().length === 0)
                return

            console.log("Сохранение заметки:", titleField.text)
            // Меняем заголовок родительского окна
            var window = editorRoot.window
            if (window) {
                window.title = titleField.text
            }

            editorRoot.saved(titleField.text, textEdit.text)
        }
    }
}