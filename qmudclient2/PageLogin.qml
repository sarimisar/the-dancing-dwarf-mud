import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Item {
    // Animazione messaggi di errore e info
    Connections {
        target: connectionHandler
        onLastMessageChange: {
            labelErrorMessage.blink()
            labelInfoMessage.blink()
        }
    }

    ColumnLayout {
        spacing: 10
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        Item {
            Layout.fillHeight: true
        }

        QMudLabel {
            text: "Username"
            Layout.fillHeight: false
            Layout.fillWidth: false
            Layout.alignment: Qt.AlignHCenter
        }

        QMudTextEntry {
            id: windConnectionUsername
            width: 300
            Layout.alignment: Qt.AlignHCenter

            focus: true
            KeyNavigation.tab: windConnectionPassword
        }

        QMudLabel {
            text: "Password"
            Layout.fillHeight: false
            Layout.fillWidth: false
            Layout.alignment: Qt.AlignHCenter
        }

        QMudTextEntry {
            id: windConnectionPassword
            width: 300
            echoMode: TextInput.Password
            Layout.alignment: Qt.AlignHCenter

            KeyNavigation.tab: btnLogin
        }

        QMudButton {
            id: btnLogin
            width: 150
            text: "Login"
            enabled: windConnectionUsername.text.length > 0 &&
                     windConnectionPassword.text.length >= 3
            Layout.alignment: Qt.AlignHCenter

            KeyNavigation.tab: windConnectionUsername

            onClicked: {
                connectionHandler.clearMessages()

                if (connectionHandler.connectToHost())
                    connectionHandler.cmdLogin(windConnectionUsername.text, windConnectionPassword.text)
            }
        }

        Text {
            width: 150
            text: '<html><style type="text/css"></style><a href="registrati">Registrati</a></html>'
            Layout.alignment: Qt.AlignHCenter
            opacity: enabled ? 1.0 : 0.3
            enabled: windConnectionUsername.text.length > 0 &&
                     windConnectionPassword.text.length >= 3

            onLinkActivated: {
                connectionHandler.clearMessages()

                if (connectionHandler.connectToHost())
                    connectionHandler.cmdSignin(windConnectionUsername.text, windConnectionPassword.text)
            }
        }

        QMudLabel {
            id: labelErrorMessage
            Layout.alignment: Qt.AlignHCenter
            text: connectionHandler.lastErrorMessage
            height: 20
            color: "#DD5555"
        }

        QMudLabel {
            id: labelInfoMessage
            Layout.alignment: Qt.AlignHCenter
            text: connectionHandler.lastInfoMessage
            height: 20
            color: "#55DD55"
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
