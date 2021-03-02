import QtQuick 2.0
import QtQuick.Controls 2.15
import QtQml.Models 2.15

Popup {
    id: dialog

    width: 650
    height: 450

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

            onClicked: dialog.close()
        }

        QMudButton {
            id: btnOk
            width: 150
            enabled: chName.text.trim().length >= 3
            text: qsTr("Crea")
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0

            KeyNavigation.tab: btnCancel

            onClicked: dialog.close()
        }
    }

    ComboBox {
        id: chRace
        height: 40
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: chName.bottom
        font.family: "Cooper Black"
        font.pointSize: 12
        flat: true
        anchors.rightMargin: 315
        anchors.leftMargin: 9
        currentIndex: 0
        anchors.topMargin: 14

        model: ListModel {
            id: cbItems
            ListElement { text: qsTr("Umano") }
            ListElement { text: qsTr("Nano") }
            ListElement { text: qsTr("Gnomo") }
            ListElement { text: qsTr("Mezz'orco") }
            ListElement { text: qsTr("Elfo alto") }
            ListElement { text: qsTr("Mezz'elfo") }
        }
    }

    TextField {
        id: chName
        height: 40
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        font.pointSize: 12
        font.family: "Cooper Black"
        anchors.topMargin: 18
        anchors.leftMargin: 9
        anchors.rightMargin: 9
        placeholderText: qsTr("Nome del personaggio")
    }

    Rectangle {
        x: 64
        y: 170
        width: 170
        height: 158
        color: "#757064"


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
    }

    Image {
        id: image1
        x: 240
        y: 170
        width: 170
        height: 158
        source: "qrc:/images/images/classes/mage.svg"
        fillMode: Image.PreserveAspectFit
    }

    Image {
        id: image2
        x: 416
        y: 170
        width: 170
        height: 158
        source: "qrc:/images/images/classes/barbarian.svg"
        fillMode: Image.PreserveAspectFit
    }
}

