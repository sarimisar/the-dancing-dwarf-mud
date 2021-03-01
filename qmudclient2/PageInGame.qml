import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.0
import QtQml 2.12

import qmudmapquickitem.qml 1.0


Item {
    property var shadowRadius: 10.0
    property var shadowSamples: 12

    onFocusChanged: button.focus = true

    Connections {
        target: connectionHandler
        onPlayerPositionChange: mainMap.setSelectedRoom(connectionHandler.playerPosition)
    }

    Button {
        id: button
        anchors.fill: parent
        focus: true
        focusPolicy: Qt.StrongFocus
        Keys.onPressed: {
            if (event.key === Qt.Key_8 || event.key === Qt.Key_W)
                connectionHandler.cmdMoveNorth();
            else if (event.key === Qt.Key_2 || event.key === Qt.Key_S)
                connectionHandler.cmdMoveSouth();
            else if (event.key === Qt.Key_6 || event.key === Qt.Key_D)
                connectionHandler.cmdMoveEast();
            else if (event.key === Qt.Key_4 || event.key === Qt.Key_A)
                connectionHandler.cmdMoveWeast();
            else if (event.key === Qt.Key_7 || event.key === Qt.Key_9 || event.key === Qt.Key_E)
                connectionHandler.cmdMoveUp();
            else if (event.key === Qt.Key_1 || event.key === Qt.Key_3 || event.key === Qt.Key_Q)
                connectionHandler.cmdMoveDown();
        }
    }

    // Main Map
    QMudMap {
        id: mainMap
        anchors.fill: parent

        tailSizePx: 80
        zoomFactor: 1.0
        background: "#878372"

        mapTailSize: "50x50"
        mapCenterPoint: connectionHandler.playerPosition
        mapId: connectionHandler.playerMapId
        npcs: connectionHandler.npcs

        onFocusChanged: button.focus = true

        property var roomContentObjs: []
        property var roomContentObjsShadows: []
        property var lastRoomId: [0, 0]

        Component {
            id: roomNpcInfo

            Item {
                id: roomNpcInfoControl
                width: 184
                height: 25

                property string name
                property var qmudId: 999999999999

                Column
                {
                    spacing: 3
                    QMudLabel {
                        height: 25
                        leftPadding: 5
                        width: roomNpcInfoControl.width
                        verticalAlignment: Text.AlignVCenter
                        color: "#FFFFFF"
                        text: roomNpcInfoControl.name

                        background: Rectangle {
                            color: qmudId === connectionHandler.selectedPcId ? "steelblue" : "#BB343434"
                            border.color: "#BBA4A4A4"
                            radius: 5
                        }
                    }
                }

                MouseArea {
                    anchors.fill: parent

                    onClicked: {
                        if (roomNpcInfoControl.qmudId !== 999999999999)
                            connectionHandler.cmdTarget(qmudId)
                    }
                }
            }
        }

        Component {
            id: roomNpcInfoShadow

            DropShadow {

                horizontalOffset: 0
                verticalOffset: 0
                radius: shadowRadius
                samples: shadowSamples
                color: "#80000000"
                source: roomNpcInfoControl
            }
        }

        function updateSelectedRoomInfo(roomId) {
            var index = 0

            for (index = 0; index < roomContentObjs.length; ++index)
                roomContentObjs[index].destroy()

            roomContentObjs = []

            for (index = 0; index < roomContentObjsShadows.length; ++index)
                roomContentObjsShadows[index].destroy()

            roomContentObjsShadows = []

            var yPos = mainMap.mapFromRoomPos(roomId).y - mainMap.tailSizePx * mainMap.zoomFactor - 30
            var xPos = mainMap.mapFromRoomPos(roomId).x

            for (index = 0; index < npcs.length; ++index)
            {
                var npcData = npcs[index].split(",");
                if (roomId.x === parseInt(npcData[1]) &&
                    roomId.y === parseInt(npcData[2]))
                {
                    roomContentObjs.push(roomNpcInfo.createObject(this,
                                                         {
                                                             id: "roomContentObj" + index.toString(),
                                                             x: xPos - 184 / 2,
                                                             y: yPos,
                                                             name: npcData[4],
                                                             qmudId: parseInt(npcData[0])
                                                         }))

                    roomContentObjsShadows.push(roomNpcInfoShadow.createObject(this,
                                                         {
                                                             source: roomContentObjs[roomContentObjs.length - 1],
                                                             x: xPos - 184 / 2,
                                                             y: yPos,
                                                             width: roomContentObjs[roomContentObjs.length - 1].width,
                                                             height: roomContentObjs[roomContentObjs.length - 1].height
                                                         }))


                    yPos -= 30
                }
            }
        }

        onSelectedRoomChanged: { updateSelectedRoomInfo(roomId); lastRoomId = roomId }
        onNpcsChanged: updateSelectedRoomInfo(lastRoomId)
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

        onFocusChanged: button.focus = true

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
            background: "#777362"

            mapTailSize: "50x50"
            mapCenterPoint: connectionHandler.playerPosition
            mapId: connectionHandler.playerMapId
            npcs: connectionHandler.npcs

            onFocusChanged: button.focus = true
        }
    }

    DropShadow {
        anchors.fill: miniMap
        horizontalOffset: 0
        verticalOffset: 0
        radius: shadowRadius
        samples: shadowSamples
        color: "#80000000"
        source: miniMap
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

        onFocusChanged: button.focus = true
    }

    DropShadow {
        anchors.fill: playerInfo
        horizontalOffset: 0
        verticalOffset: 0
        radius: shadowRadius
        samples: shadowSamples
        color: "#80000000"
        source: playerInfo
    }

    // Target information
    QMudChInfo {
        id: targetInfo
        width: 250
        height: 100
        x: parent.width / 2.0 + width * 0.5
        y: parent.height - 200
        visible: connectionHandler.selectedPcName !== ""

        name: connectionHandler.selectedPcName.toString().charAt(0).toUpperCase() +
              connectionHandler.selectedPcName.toString().substring(1)

        valueHp: connectionHandler.selectedPcHpCurrent / connectionHandler.selectedPcHpMaximum
        textHp: connectionHandler.selectedPcHpCurrent + " / " + connectionHandler.selectedPcHpMaximum
        valueMp: connectionHandler.selectedPcMpCurrent / connectionHandler.selectedPcMpMaximum
        textMp: connectionHandler.selectedPcMpCurrent + " / " + connectionHandler.selectedPcMpMaximum
        valueAp: connectionHandler.selectedPcApCurrent / connectionHandler.selectedPcApMaximum
        textAp: connectionHandler.selectedPcApCurrent + " / " + connectionHandler.selectedPcApMaximum

        onFocusChanged: button.focus = true
    }

    DropShadow {
        visible: targetInfo.visible
        anchors.fill: targetInfo
        horizontalOffset: 0
        verticalOffset: 0
        radius: shadowRadius
        samples: shadowSamples
        color: "#80000000"
        source: targetInfo
    }

    // Experience bar
    QMudProgressBar {
        id: experienceBar
        width: (parent.width - miniMap.width * 2) * 0.8
        height: 10
        anchors.horizontalCenter: parent.horizontalCenter
        y: 5
        bgColor: "darkgray"
        color: "darkmagenta"
        text: connectionHandler.playerExperience + " / " + connectionHandler.playerExperienceToLevelUp
        font.pointSize: 6
        value: connectionHandler.playerExperience / connectionHandler.playerExperienceToLevelUp

        onFocusChanged: button.focus = true
    }

    DropShadow {
        anchors.fill: experienceBar
        horizontalOffset: 0
        verticalOffset: 0
        radius: shadowRadius
        samples: shadowSamples
        color: "#80000000"
        source: experienceBar
    }
}
