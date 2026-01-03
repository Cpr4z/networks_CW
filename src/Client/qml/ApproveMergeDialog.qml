import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: dialog
    modal: true
    focus: true
    width: 650
    height: 470

    x: (parent ? (parent.width - width) / 2 : 100)
    y: (parent ? (parent.height - height) / 2 : 100)


    title: "Подтверждение изменений"

    property int noteId: -1
    property int mergeAuthor: -1
    property int mergeVersion: 0

    property string originalText: ""
    property string mergedText: ""

    signal acceptMerge(int noteId, string text, int version, int merge_sender_id)
    signal rejectMerge(int noteId, string text, int version, int merge_sender_id)

    function reposition() {
        if (parent) {
            x = Math.max(0, (parent.width - width) / 2)
            y = Math.max(0, (parent.height - height) / 2)
        }
    }

    onVisibleChanged: {
        if (visible) {
            reposition()
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 10

        RowLayout {
            Layout.fillWidth: true

            Label {
                text: "От пользователя c id: " + mergeAuthor
                color: "#666"
            }

            Item { Layout.fillWidth: true }

            Label {
                text: "Версия: " + mergeVersion
                color: "#666"
                visible: mergeVersion !== 0
            }
        }

        Label {
            Layout.fillWidth: true
            text: "Ваш текущий текст (локально)"
            font.bold: true
        }

        TextArea {
            Layout.fillWidth: true
            Layout.preferredHeight: 150
            readOnly: true
            wrapMode: TextEdit.Wrap
            text: dialog.originalText
        }

        Label {
            Layout.fillWidth: true
            text: "Предложенные изменения (можно отредактировать перед принятием)"
            font.bold: true
        }

        TextArea {
            id: proposedArea
            Layout.fillWidth: true
            Layout.fillHeight: true
            wrapMode: TextEdit.Wrap
            text: dialog.mergedText

            onTextChanged: dialog.mergedText = text
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            Button {
                text: "Отклонить"
                onClicked: {
                    dialog.rejectMerge(dialog.noteId, dialog.originalText, dialog.mergeVersion, dialog.mergeAuthor)
                    dialog.close()
                }
            }

            Item { Layout.fillWidth: true }

            Button {
                text: "Принять"
                highlighted: true
                enabled: dialog.noteId !== -1
                onClicked: {
                    dialog.acceptMerge(dialog.noteId, dialog.mergedText, dialog.mergeVersion, dialog.mergeAuthor)
                    dialog.close()
                }
            }
        }
    }
}