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
            console.debug("Premuto")
            console.debug(event.key)

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

}
