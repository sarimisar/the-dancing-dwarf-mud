import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.12

Item {
    id: root

    required property string colorKey
    property url icon

    signal clicked()

    MouseArea {
        id: mouseArea

        anchors.fill: root
        anchors.centerIn: parent

        drag.target: tile

        onReleased: parent = tile.Drag.target !== null ? tile.Drag.target : root
        onClicked: root.clicked()

        Rectangle {
            id: tile

            anchors.fill: mouseArea
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter

            color: "#AAFFFFFF"

            Drag.keys: [ root.colorKey ]
            Drag.active: mouseArea.drag.active
            Drag.hotSpot.x: root.width / 2
            Drag.hotSpot.y: 0

            Image {
                id: internalImage
                anchors.fill: parent
                horizontalAlignment: Image.AlignHCenter
                verticalAlignment: Image.AlignVCenter
                fillMode: Image.PreserveAspectFit
                source: root.icon
                smooth: true
            }

            states: State {
                when: mouseArea.drag.active
                ParentChange { target: tile; parent: root }
                AnchorChanges { target: tile; anchors.verticalCenter: undefined; anchors.horizontalCenter: undefined }
            }
        }
    }
}
