import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: dialog
    modal: true
    focus: true
    width: 700
    height: 450
    title: "Конфликт изменений"

    property int noteId
    property string noteTitle
    property string localContent
    property string serverContent
    property int serverVersion

    signal acceptServer(int noteId)
    signal mergeManually(int noteId, string content)

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        Text {
            text: "Заметка была изменена другим пользователем"
            font.pixelSize: 18
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }

        Text {
            text: "Название: " + noteTitle
            font.pixelSize: 14
            color: "#666"
            Layout.alignment: Qt.AlignHCenter
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 10

            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true

                Label {
                    text: "Версия сервера"
                    font.bold: true
                }

                TextArea {
                    readOnly: true
                    text: serverContent
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true

                Label {
                    text: "Ваша версия"
                    font.bold: true
                }

                TextArea {
                    id: editableArea
                    text: localContent
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 10
            Layout.alignment: Qt.AlignRight

            Button {
                text: "Принять серверную"
                onClicked: {
                    dialog.acceptServer(noteId)
                    dialog.close()
                }
            }

            Button {
                text: "Сохранить объединённую"
                highlighted: true
                onClicked: {
                    dialog.mergeManually(noteId, editableArea.text)
                    dialog.close()
                }
            }
        }
    }
}