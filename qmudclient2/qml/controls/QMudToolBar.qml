import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.12
import QtQml.Models 2.15

Grid {
    id: bar

    property alias count: bar.columns
    property int buttonWidth: height
    property int buttonHeight: height

    anchors.margins: 5
    spacing: 5
    rows: 1

    Repeater {
        model: bar.columns * bar.rows
        delegate: DropArea {
            id: dragTarget

            property string colorKey: "action"
            property alias dropProxy: dragTarget

            width: bar.buttonWidth
            height: bar.buttonHeight
            keys: [ colorKey ]

            Rectangle {
                id: dropRectangle

                anchors.fill: parent
                color: "transparent"
                border.color: "white"
                opacity: 0.5

                states: [
                    State {
                        when: dragTarget.containsDrag
                        PropertyChanges {
                            target: dropRectangle
                            color: "red"
                        }
                    },

                    State {
                        when: !dragTarget.containsDrag
                        PropertyChanges {
                            target: dropRectangle
                            color: "transparent"
                        }
                    }
                ]
            }
        }
    }
}
