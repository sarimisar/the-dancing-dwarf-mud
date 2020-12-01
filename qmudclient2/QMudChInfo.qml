import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Item {
    id: control
    width: 184
    height: 65

    property alias valueHp: progressBarHp.value
    property alias valueMp: progressBarMp.value
    property alias valueAp: progressBarAp.value

    property alias textHp: progressBarHp.text
    property alias textMp: progressBarMp.text
    property alias textAp: progressBarAp.text

    property alias name: chName.text

    property var qmudId: 999999999999

    Column
    {
        spacing: 3
        QMudLabel {
            id: chName
            height: 25
            leftPadding: 5
            width: control.width
            verticalAlignment: Text.AlignVCenter
            color: "#FFFFFF"

            background: Rectangle {
                color: "#BB343434"
                border.color: "#BBA4A4A4"
                radius: 5
            }
        }

        Rectangle {
            id: rectData
            height: control.height - chName.height - 3
            width: control.width
            color: "#BB343434"
            radius: 5

            ColumnLayout {
                id: layout
                spacing: 2
                width: rectData.width - 4
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter

                Item {
                    Layout.fillHeight: true
                }

                QMudProgressBar {
                    id: progressBarHp
                    height: (rectData.height - layout.spacing * 4) / 8 * 3
                    width: rectData.width - 4
                    color: "#22FF22"
                    Layout.fillWidth: false
                }

                QMudProgressBar {
                    id: progressBarMp
                    height: (rectData.height - layout.spacing * 4) / 8 * 3
                    width: rectData.width - 4
                    color: "steelblue"
                    Layout.fillWidth: false
                }

                QMudProgressBar {
                    id: progressBarAp
                    height: rectData.height - progressBarHp.height - progressBarMp.height - layout.spacing * 4 - 1
                    width: rectData.width - 4
                    color: "#FFEE00"
                    Layout.fillWidth: false
                }

                Item {
                    Layout.fillHeight: true
                }
            }
        }
    }

    MouseArea {
        anchors.fill: parent

        onClicked: {
            if (qmudId !== 999999999999)
                connectionHandler.cmdTarget(qmudId)
        }
    }
}
