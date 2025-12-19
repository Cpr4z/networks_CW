import QtQuick
import QtQuick.Controls

Dialog {
    id: dialog
    modal: true
    title: "Новая заметка"

    signal noteAccepted(string title)

    Column {
        spacing: 10
        padding: 16

        TextField {
            id: titleField
            placeholderText: "Название заметки"
        }

        Row {
            spacing: 10

            Button {
                text: "Отмена"
                onClicked: dialog.close()
            }

            Button {
                text: "OK"
                onClicked: {
                    if (titleField.text.length > 0) {
                        dialog.noteAccepted(titleField.text)
                        dialog.close()
                    }
                }
            }
        }
    }
}