import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.12

import "controls"

import QMudEnums 1.0

Item {
    id: page
    anchors.fill: parent

    Rectangle {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 9
        width: 500
        height: parent.height - btnConnect.height - 27
        color: "#F8F6E9"
        radius: 10

        ListView {
            id: listViewCharacters
            model: charactersModel
            anchors.top: parent.top
            clip: true
            anchors.fill: parent
            anchors.margins: 9
            flickableDirection: Flickable.VerticalFlick
            boundsBehavior: Flickable.StopAtBounds

            delegate: Rectangle {
                property variant myData: model

                width: listViewCharacters.width
                clip: true
                height: 80
                color: "transparent"

                Rectangle {
                    height: parent.height - 2
                    width: parent.width
                    anchors.top: parent.top
                    anchors.horizontalCenter: parent.horizontalCenter
                    color: index === listViewCharacters.currentIndex ? "#E6E4E7" : "transparent"
                    radius: 10
                }

                Rectangle {
                    height: 2
                    width: parent.width * 0.95
                    anchors.bottom: parent.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                    color: "#E6E4E7"
                }

                QMudLabel {
                    height: 60
                    width: parent.width
                    anchors.top: parent.top
                    //anchors.horizontalCenter: parent.horizontalCenter
                    font.pointSize: 32
                    text: name
                    leftPadding: 10
                }

                QMudLabel {
                    height: 20
                    width: parent.width
                    anchors.bottom: parent.bottom
                    //anchors.horizontalCenter: parent.horizontalCenter
                    font.pointSize: 12
                    leftPadding: 30
                    text: qsTr("Livello ") + level + ", " + chClass + " " + race
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: listViewCharacters.currentIndex = index
                }
            }

            ScrollBar.vertical: ScrollBar {}
        }
    }

    QMudButton {
        id: btnConnect
        width: 150
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 9
        x: (parent.width - 300 - 9) / 2
        text: qsTr("Connetti")
        enabled: listViewCharacters.currentIndex >= 0

        KeyNavigation.tab: btnCreate

        onClicked: connectionHandler.cmdConnect(listViewCharacters.currentItem.myData.name)
    }

    QMudButton {
        id: btnCreate
        width: 150
        anchors.bottom: parent.bottom
        anchors.left: btnConnect.right
        anchors.leftMargin: 9
        anchors.bottomMargin: 9
        text: "Crea"

        KeyNavigation.tab: btnConnect

        onClicked: chCreationDialog.open()
    }
}
