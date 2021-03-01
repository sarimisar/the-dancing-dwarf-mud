import QtQuick 2.0
import QtQuick.Controls 2.15
import QtQml.Models 2.15

Popup {
    id: dialog

    width: 612
    height: 359

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
            text: qsTr("Crea")
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0
            enabled: listViewCharacters.currentIndex >= 0

            KeyNavigation.tab: btnCancel

            onClicked: dialog.close()
        }
    }

    ComboBox {
        id: comboBox
        y: 72
        anchors.horizontalCenter: parent.horizontalCenter
        width: 288
        height: 40
        anchors.top: row.bottom
        anchors.horizontalCenterOffset: -153
        currentIndex: 0
        anchors.topMargin: 98

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
        id: textField
        x: 9
        y: 18
        width: 594
        height: 40
        placeholderText: qsTr("Nome del personaggio")
    }
}

