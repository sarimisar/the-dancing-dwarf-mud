import QtQuick 2.0
import QtQuick.Controls 2.15
import QtQml.Models 2.15

import "controls"

import QMudEnums 1.0


Popup {
    // Animazione messaggi di errore e info
    Connections {
        target: connectionHandler
        onLastMessageChange: function onFoo() {
            labelErrorMessage.blink()
            labelInfoMessage.blink()

            if (connectionHandler.lastInfoMessage !== "" &&
                connectionHandler.lastErrorMessage === "") {
                connectionHandler.clearMessages();
                dialog.close()
            }
        }
    }

    id: dialog

    width: 650
    height: 500

    property var currentClass: CLASS_WARRIOR

    Row {
        id: bottomRow
        y: 430
        height: 50
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        layoutDirection: Qt.RightToLeft
        anchors.rightMargin: 9
        anchors.leftMargin: 9
        anchors.bottomMargin: 9
        spacing: 9

        QMudButton {
            id: btnCancel
            width: 150
            text: qsTr("Annulla")
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0

            KeyNavigation.tab: btnOk

            onClicked: { connectionHandler.clearMessages(); dialog.close() }
        }

        QMudButton {
            id: btnOk
            width: 150
            enabled: chName.text.trim().length >= 3
            text: qsTr("Crea")
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0

            KeyNavigation.tab: btnCancel

            onClicked: {
                connectionHandler.clearMessages()
                connectionHandler.cmdChCreate(chName.text.trim(), dialog.currentClass, chRace.currentIndex, chSex.currentIndex === 0 ? SEX_MALE : SEX_FEMALE)
                //dialog.close()
            }
        }
    }

    QMudComboBox {
        id: chRace
        height: 40
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: chName.bottom
        anchors.rightMargin: 316
        anchors.leftMargin: 9
        currentIndex: 0
        anchors.topMargin: 14

        model: ListModel {
            ListElement { text: qsTr("Umano") }
            ListElement { text: qsTr("Nano") }
            ListElement { text: qsTr("Gnomo") }
            ListElement { text: qsTr("Mezz'orco") }
            ListElement { text: qsTr("Elfo alto") }
            ListElement { text: qsTr("Mezz'elfo") }
        }
    }

    QMudTextEntry {
        id: chName
        height: 40
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.topMargin: 18
        anchors.leftMargin: 9
        anchors.rightMargin: 9
        placeholderText: qsTr("Nome del personaggio")
    }

    Rectangle {
        id: classWarrior
        x: 64
        y: 170
        width: 170
        height: 158
        color: dialog.currentClass === CLASS_WARRIOR ? "#757064" : "#00000000"
        radius: 5

        Image {
            id: image
            anchors.fill: parent
            anchors.topMargin: 2
            anchors.leftMargin: 2
            anchors.rightMargin: 2
            anchors.bottomMargin: 2

            source: "qrc:/images/images/classes/warrior.svg"
            fillMode: Image.PreserveAspectFit
        }

        MouseArea {
            anchors.fill: parent

            onClicked: dialog.currentClass = CLASS_WARRIOR
        }
    }

    Rectangle {
        x: 240
        y: 170
        width: 170
        height: 158
        color: dialog.currentClass === CLASS_MAGE ? "#757064" : "#00000000"
        radius: 5

        Image {
            id: image1
            anchors.fill: parent
            anchors.topMargin: 2
            anchors.leftMargin: 2
            anchors.rightMargin: 2
            anchors.bottomMargin: 2

            source: "qrc:/images/images/classes/mage.svg"
            fillMode: Image.PreserveAspectFit
        }

        MouseArea {
            anchors.fill: parent

            onClicked: dialog.currentClass = CLASS_MAGE
        }
    }

    Rectangle {
        x: 416
        y: 170
        width: 170
        height: 158
        color: dialog.currentClass === CLASS_BARBARIAN ? "#757064" : "#00000000"
        radius: 5

        Image {
            id: image2

            anchors.fill: parent
            anchors.topMargin: 2
            anchors.leftMargin: 2
            anchors.rightMargin: 2
            anchors.bottomMargin: 2
            source: "qrc:/images/images/classes/barbarian.svg"
            fillMode: Image.PreserveAspectFit
        }

        MouseArea {
            anchors.fill: parent

            onClicked: dialog.currentClass = CLASS_BARBARIAN
        }
    }

    QMudLabel {
        id: labelErrorMessage
        x: 12
        width: 632
        anchors.top: classWarrior.bottom
        anchors.topMargin: 14
        text: connectionHandler.lastErrorMessage
        height: 20
        color: "#DD5555"
    }

    QMudLabel {
        id: labelInfoMessage
        x: 10
        width: 634
        anchors.top: labelErrorMessage.bottom
        anchors.topMargin: 6
        text: connectionHandler.lastInfoMessage
        height: 20
        color: "#55DD55"
    }

    QMudComboBox {
        id: chSex
        height: 40
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: chRace.bottom
        model: ListModel {
            ListElement { text: qsTr("Maschio") }
            ListElement { text: qsTr("Femmina") }
        }
        anchors.leftMargin: 8
        anchors.topMargin: 6
        currentIndex: 0
        anchors.rightMargin: 316
    }
}

