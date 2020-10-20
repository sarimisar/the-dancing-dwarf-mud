import QtQuick 2.12
import QtQuick.Window 2.12

import QMudEnums 1.0

Window {
    visible: true
    width: 800
    height: 600
    title: qsTr("The Dancing Dwarf - v 0.1")

    Item {
        id: stack
        anchors.fill: parent

        PageLogin           { anchors.fill: parent; visible: !connectionHandler.isConnected || connectionHandler.currentPage === PageType.LOGIN }
        PageCharactersList  { anchors.fill: parent; visible: connectionHandler.isConnected && connectionHandler.currentPage === PageType.CHARACTERS_LIST }
        PageInGame          { anchors.fill: parent; visible: connectionHandler.isConnected && connectionHandler.currentPage === PageType.IN_GAME }
    }
}
