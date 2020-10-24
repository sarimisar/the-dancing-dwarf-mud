import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.0

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
        radius: width / 2

        QMudMap {
            id: qmudminimap
            anchors.fill: parent
            anchors.margins: 2
            clip: true
            layer.enabled: true

            layer.effect: OpacityMask {
                maskSource: Item {
                    width: qmudminimap.width
                    height: qmudminimap.height
                    Rectangle {
                        anchors.centerIn: parent
                        width: qmudminimap.width
                        height: qmudminimap.height
                        radius: Math.min(width, height)
                    }
                }
            }

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

    // Player information
    QMudChInfo {
        id: playerInfo
        width: 250
        height: 100
        x: parent.width / 2.0 - width * 1.5
        y: parent.height - 200

        name: connectionHandler.playerName.toString().charAt(0).toUpperCase() +
              connectionHandler.playerName.toString().substring(1)

        valueHp: connectionHandler.playerHpCurrent / connectionHandler.playerHpMaximum
        textHp: connectionHandler.playerHpCurrent + " / " + connectionHandler.playerHpMaximum
        valueMp: connectionHandler.playerMpCurrent / connectionHandler.playerMpMaximum
        textMp: connectionHandler.playerMpCurrent + " / " + connectionHandler.playerMpMaximum
        valueAp: connectionHandler.playerApCurrent / connectionHandler.playerApMaximum
        textAp: connectionHandler.playerApCurrent + " / " + connectionHandler.playerApMaximum
    }

    // Target information
    QMudChInfo {
        id: targetInfo
        width: 250
        height: 100
        x: parent.width / 2.0 + width * 0.5
        y: parent.height - 200

        name: connectionHandler.playerName.toString().charAt(0).toUpperCase() +
              connectionHandler.playerName.toString().substring(1)

        valueHp: connectionHandler.playerHpCurrent / connectionHandler.playerHpMaximum
        textHp: connectionHandler.playerHpCurrent + " / " + connectionHandler.playerHpMaximum
        valueMp: connectionHandler.playerMpCurrent / connectionHandler.playerMpMaximum
        textMp: connectionHandler.playerMpCurrent + " / " + connectionHandler.playerMpMaximum
        valueAp: connectionHandler.playerApCurrent / connectionHandler.playerApMaximum
        textAp: connectionHandler.playerApCurrent + " / " + connectionHandler.playerApMaximum
    }

    // Experience bar
    QMudProgressBar {
        width: (parent.width - miniMap.width * 2) * 0.8
        height: 10
        anchors.horizontalCenter: parent.horizontalCenter
        y: 5
        bgColor: "darkgray"
        color: "darkmagenta"
        text: connectionHandler.playerExperience + " / " + connectionHandler.playerExperienceToLevelUp
        font.pointSize: 6
        value: connectionHandler.playerExperience / connectionHandler.playerExperienceToLevelUp
    }
}
