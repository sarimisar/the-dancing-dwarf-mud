import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import QMudEnums 1.0

Item {
    id: page
    anchors.fill: parent

    ColumnLayout {
        spacing: 10
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        anchors.fill: parent
        anchors.margins: 20

        Rectangle {
            Layout.alignment: Qt.AlignHCenter
            Layout.fillHeight: true
            width: 500
            color: "#F8F6E9"
            radius: 10

            ListView {
                id: listViewCharacters
                model: charactersModel
                anchors.fill: parent

                delegate: Rectangle {
                    property variant myData: model

                    width: parent.width
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
                        text: "Livello " + level + ", " + chClass + " " + race
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: listViewCharacters.currentIndex = index
                    }
                }
            }
        }

        QMudButton {
            id: btnConnect
            width: 150
            text: "Connetti"
            enabled: listViewCharacters.currentIndex >= 0
            Layout.alignment: Qt.AlignHCenter

            KeyNavigation.tab: btnCreate

            onClicked: connectionHandler.cmdConnect(listViewCharacters.currentItem.myData.name)
        }

        QMudButton {
            id: btnCreate
            width: 150
            text: "Crea"
            Layout.alignment: Qt.AlignHCenter

            KeyNavigation.tab: btnConnect

            onClicked: {
            }
        }
    }
}
