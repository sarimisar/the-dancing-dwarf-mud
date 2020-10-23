import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import qmudmapquickitem.qml 1.0

Item {
    onFocusChanged: button.focus = true

    Button {
        id: button
        anchors.fill: parent
        focus: true
        Keys.onPressed: {
            if (event.key === Qt.Key_8)
                connectionHandler.cmdMoveNorth();
            else if (event.key === Qt.Key_2)
                connectionHandler.cmdMoveSouth();
            else if (event.key === Qt.Key_6)
                connectionHandler.cmdMoveEast();
            else if (event.key === Qt.Key_4)
                connectionHandler.cmdMoveWeast();
            else if (event.key === Qt.Key_7 || event.key === Qt.Key_9)
                connectionHandler.cmdMoveUp();
            else if (event.key === Qt.Key_1 || event.key === Qt.Key_3)
                connectionHandler.cmdMoveDown();
        }
    }

    // Main Map
    QMudMap {
        anchors.fill: parent

        tailSizePx: 80
        zoomFactor: 1.0
        background: "#333333"

        mapTailSize: "50x50"
        mapCenterPoint: connectionHandler.playerPosition
        mapId: connectionHandler.playerMapId
        npcs: connectionHandler.npcs
    }

    // Mini Map
    Rectangle {
        id: miniMap
        width: 250
        height: 250
        x: parent.width - width - 15
        y: 15
        border.color: "#FFFFFF"
        border.width: 2

        QMudMap {
            anchors.fill: parent
            anchors.margins: 2
            clip: true

            miniMap: true

            tailSizePx: 80
            zoomFactor: 0.05
            background: "#333333"

            mapTailSize: "50x50"
            mapCenterPoint: connectionHandler.playerPosition
            mapId: connectionHandler.playerMapId
            npcs: connectionHandler.npcs
        }
    }

    QMudChInfo {
        id: playerInfo
        width: 250
        height: 100
        x: parent.width / 2.0 - width * 1.5
        y: parent.height - 200

        valueHp: 0.5//connectionHandler.playerHpCurrent / connectionHandler.playerHpMaximum
        valueMp: 0.3//connectionHandler.playerHpCurrent / connectionHandler.playerHpMaximum
        valueAp: connectionHandler.playerHpCurrent / connectionHandler.playerHpMaximum
    }
}
