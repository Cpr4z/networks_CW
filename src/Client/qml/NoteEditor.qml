import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

Item {
    id: editorRoot
    anchors.fill: parent

    property int noteId
    property string initialTitle
    property string initialText

    Rectangle {
        anchors.fill: parent
        color: "white"

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 16

            TextField {
                id: titleField
                text: initialTitle
                font.pixelSize: 16
                font.bold: true
            }

            TextArea {
                id: textEdit
                text: initialText
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            Button {
                text: "Назад"
                onClicked: editorRoot.destroy()
            }
        }
    }

    Shortcut {
        sequence: StandardKey.Save
        onActivated: {
            notesManager.updateNote(
                noteId,
                titleField.text,
                textEdit.text
            )
        }
    }
}