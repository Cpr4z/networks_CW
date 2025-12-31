import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: approveMergeDialog
    modal: true
    focus: true
    width: 600
    height: 500
    title: "Подтверждение слияния изменений"

    // 🔹 Входные данные
    property int noteId: -1
    property string noteTitle: ""
    property string mergedText: ""
    property string originalText: ""
    property string mergeAuthor: ""
    property int mergeVersion: 0

    signal acceptMerge(int noteId, string text, int version)
    signal rejectMerge(int noteId)
    signal requestChanges(int noteId, string feedback)

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 8

        // --- Заголовок ---
        Text {
            text: "Запрос на объединение изменений"
            font.pixelSize: 16
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }

        Text {
            text: "Заметка: " + noteTitle
            font.pixelSize: 14
            color: "#666"
            Layout.alignment: Qt.AlignHCenter
        }

        Text {
            text: "Автор слияния: " + mergeAuthor
            font.pixelSize: 12
            color: "#999"
            Layout.alignment: Qt.AlignHCenter
        }

        // --- Панель с вкладками ---
        TabBar {
            id: tabBar
            Layout.fillWidth: true

            TabButton {
                text: "Исходный текст"
            }
            TabButton {
                text: "Предложенное слияние"
            }
            TabButton {
                text: "Сравнение"
            }
        }

        StackLayout {
            id: stackLayout
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: tabBar.currentIndex

            // Вкладка 1: Исходный текст
            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true

                TextArea {
                    readOnly: true
                    text: originalText
                    wrapMode: TextEdit.Wrap
                    font.pixelSize: 12
                    background: Rectangle {
                        color: "#f9f9f9"
                        border.color: "#ddd"
                        border.width: 1
                        radius: 3
                    }
                }
            }

            // Вкладка 2: Предложенное слияние
            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true

                TextArea {
                    id: mergedTextArea
                    text: mergedText
                    wrapMode: TextEdit.Wrap
                    font.pixelSize: 12
                    background: Rectangle {
                        color: "#f0fff0"
                        border.color: "#4CAF50"
                        border.width: 2
                        radius: 3
                    }

                    // Позволяем владельцу редактировать перед принятием
                    onTextChanged: {
                        approveMergeDialog.mergedText = text
                    }
                }
            }

            // Вкладка 3: Сравнение
            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true

                Flickable {
                    contentWidth: width
                    contentHeight: diffText.height

                    Text {
                        id: diffText
                        width: parent.width
                        text: generateDiff(originalText, mergedText)
                        wrapMode: TextEdit.Wrap
                        font.pixelSize: 12
                        font.family: "monospace"

                        // Подсветка изменений
                        textFormat: Text.RichText
                    }
                }
            }
        }

        // --- Комментарий владельца ---
        ColumnLayout {
            Layout.fillWidth: true
            visible: requestChangesCheckBox.checked

            Label {
                text: "Комментарий к изменениям:"
                font.pixelSize: 12
            }

            TextField {
                id: feedbackField
                Layout.fillWidth: true
                placeholderText: "Опишите, что нужно изменить..."
            }
        }

        // --- Опции ---
        RowLayout {
            Layout.fillWidth: true

            CheckBox {
                id: requestChangesCheckBox
                text: "Запросить изменения"
                font.pixelSize: 12
            }

            Item { Layout.fillWidth: true }

            Text {
                text: "Версия: v" + mergeVersion
                font.pixelSize: 12
                color: "gray"
            }
        }

        // --- Кнопки действий ---
        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            Button {
                text: "Отклонить"
                flat: true
                onClicked: {
                    approveMergeDialog.rejectMerge(noteId)
                    approveMergeDialog.close()
                }
                background: Rectangle {
                    color: parent.down ? "#ffcccc" : "transparent"
                    border.color: "#ff6666"
                    border.width: 1
                    radius: 3
                }
            }

            Item { Layout.fillWidth: true }

            Button {
                text: "Запросить изменения"
                visible: requestChangesCheckBox.checked
                onClicked: {
                    approveMergeDialog.requestChanges(noteId, feedbackField.text)
                    approveMergeDialog.close()
                }
                background: Rectangle {
                    color: parent.down ? "#ffe6cc" : "#ffcc99"
                    radius: 3
                }
            }

            Button {
                text: "Принять"
                highlighted: true
                onClicked: {
                    approveMergeDialog.acceptMerge(noteId, mergedText, mergeVersion)
                    approveMergeDialog.close()
                }
                background: Rectangle {
                    color: parent.down ? "#2E7D32" : "#4CAF50"
                    radius: 3
                }
            }
        }
    }

    // Функция для генерации простого diff (упрощенная версия)
    function generateDiff(original, merged) {
        var result = ""
        var originalLines = original.split('\n')
        var mergedLines = merged.split('\n')

        // Простой алгоритм сравнения строк
        for (var i = 0; i < Math.max(originalLines.length, mergedLines.length); i++) {
            var origLine = i < originalLines.length ? originalLines[i] : ""
            var mergedLine = i < mergedLines.length ? mergedLines[i] : ""

            if (origLine !== mergedLine) {
                result += "<span style='color:red'>- " + escapeHtml(origLine) + "</span><br/>"
                result += "<span style='color:green'>+ " + escapeHtml(mergedLine) + "</span><br/>"
            } else {
                result += "  " + escapeHtml(origLine) + "<br/>"
            }
        }

        return result
    }

    function escapeHtml(text) {
        return text
            .replace(/&/g, "&amp;")
            .replace(/</g, "&lt;")
            .replace(/>/g, "&gt;")
            .replace(/"/g, "&quot;")
            .replace(/'/g, "&#039;")
            .replace(/ /g, "&nbsp;")
    }
}