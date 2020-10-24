import QtQuick 2.12
import QtQuick.Controls 2.12

ProgressBar {
    id: control

    property color bgColor: "#88868686"
    property color color: "#EE3333"
    property string text: ""

    background: Rectangle {
        anchors.fill: parent
        color: control.bgColor
        radius: 5
    }

    contentItem: Item {
        width: control.width
        height: control.height
        anchors.verticalCenter: control.verticalCenter
        anchors.horizontalCenter: control.horizontalCenter

        Rectangle {
            width: control.visualPosition * (parent.width)
            height: control.height
            x: 0
            y: 0
            radius: 4
            color: control.color
        }

        QMudLabel {
            color: "#000000"
            text: control.text
            font.pointSize: 10

            horizontalAlignment: Text.AlignHCenter

            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
