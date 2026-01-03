import QtQuick
import QtQuick.Controls

ApplicationWindow {
    visible: true
    width: 400
    height: 360
    color: "#f5f5f5"
    title: "Authorization"

    property string mode: "login"

    Rectangle {
        anchors.centerIn: parent
        width: parent.width * 0.8
        color: "white"
        radius: 8
        border.color: "#d0d0d0"
        border.width: 1

        Column {
            anchors.centerIn: parent
            spacing: 14
            width: parent.width * 0.9

            Text {
                text: mode === "login" ? "Вход в систему" : "Регистрация"
                font.pixelSize: 22
                font.bold: true
                anchors.horizontalCenter: parent.horizontalCenter
                color: "#202020"
            }

            Row {
                spacing: 10
                width: parent.width
                height: 40

                Text {
                    text: "Login:"
                    font.pixelSize: 16
                    verticalAlignment: Text.AlignVCenter
                    width: 80
                    color: "#333"
                }

                TextField {
                    id: username
                    placeholderText: "Имя пользователя"
                    font.pixelSize: 14
                    width: parent.width - 100
                }
            }

            Row {
                spacing: 10
                width: parent.width
                height: 40

                Text {
                    text: "Password:"
                    font.pixelSize: 16
                    verticalAlignment: Text.AlignVCenter
                    width: 80
                    color: "#333"
                }

                TextField {
                    id: password
                    placeholderText: "Пароль"
                    echoMode: TextInput.Password
                    font.pixelSize: 14
                    width: parent.width - 100
                }
            }

            Button {
                text: mode === "login" ? "Войти" : "Зарегистрироваться"
                width: parent.width * 0.6
                height: 34
                anchors.horizontalCenter: parent.horizontalCenter

                font.pixelSize: 15
                font.bold: true

                background: Rectangle {
                    implicitWidth: 120
                    implicitHeight: 34
                    radius: 6
                    color: mode === "login" ? "#0078D4" : "#2E8B57"
                }

                contentItem: Text {
                    text: parent.text
                    anchors.centerIn: parent
                    color: "white"
                    font.pixelSize: parent.font.pixelSize
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                onClicked: {
                    if (mode === "login") {
                        console.log("Before calling login method")
                        authManager.login(username.text, password.text)
                    }
                    else
                        authManager.registerUser(username.text, password.text)
                }
            }

            Text {
                text: mode === "login"
                    ? "У меня нет аккаунта — <a href='#'>зарегистрироваться</a>"
                    : "У меня уже есть аккаунт — <a href='#'>войти</a>"
                textFormat: Text.RichText
                font.pixelSize: 14
                color: "#0078D4"
                horizontalAlignment: Text.AlignHCenter
                onLinkActivated: mode = mode === "login" ? "register" : "login"
            }

            Text {
                id: status
                color: "red"
                font.pixelSize: 14
                wrapMode: Text.Wrap
                horizontalAlignment: Text.AlignHCenter
            }

            Connections {
                target: authManager

                function onLoginSuccess(userId) {
                    var component = Qt.createComponent("qrc:/qml/NotesListPage.qml")
                    console.log("Login finished successfully")
                    if (component.status === Component.Ready) {
                        var window = component.createObject(null, { "userId": userId })
                        if (window === null) {
                            console.log("Ошибка создания NotesListPage:", component.errorString())
                        } else {
                            if (typeof root !== "undefined")
                                root.destroy()
                        }
                    } else if (component.status === Component.Error) {
                        console.log("Ошибка загрузки QML:", component.errorString())
                    }
                }
                function onLoginFailed(reason) {
                    status.color = "red"
                    status.text = reason
                }

                function onRegistrationSuccess(userId) {
                    console.log("Регситрация завершилась успешно")
                    status.color = "green"
                    status.text = "Регистрация прошла успешно! Теперь войдите."
                    mode = "login"
                }
                function onRegistrationFailed(reason) {
                    status.color = "red"
                    status.text = reason
                }
            }


        }
    }
}