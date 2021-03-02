import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.12

FocusScope {
    id: focusScope
    height: 40

    property alias font: control.font
    property alias model: control.model
    property alias currentIndex: control.currentIndex

    signal clicked

    ComboBox {
        id: control
        anchors.fill: parent

        font.pointSize: 12
        font.family: "Cooper Black"

        focus: true
        activeFocusOnTab: true
        focusPolicy: Qt.StrongFocus

        Keys.onReturnPressed: focusScope.clicked()
        Keys.onEnterPressed: focusScope.clicked()

        contentItem: Text {
            text: control.currentText
            font: control.font
            opacity: enabled ? 1.0 : 0.3
            color: "#F4F2E5"
            padding: 9
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }

        background: Rectangle {
            implicitWidth: 120
            implicitHeight: 40
            color: "#757064"
            border.width: 0
            radius: 5
        }

        popup: Popup {
            y: control.height + 1
            width: control.width
            implicitHeight: contentItem.implicitHeight
            padding: 1

            contentItem: ListView {
                clip: true
                implicitHeight: contentHeight
                model: control.popup.visible ? control.delegateModel : null
                currentIndex: control.highlightedIndex

                ScrollIndicator.vertical: ScrollIndicator { }
            }

            background: Rectangle {
                border.width: 2
                color: "#757064"
                border.color: "#50AA50"
            }
        }

        delegate: ItemDelegate {
            width: control.width

            contentItem: Text {
                text: modelData
                color: "#F4F2E5"
                font: control.font
                elide: Text.ElideRight
                verticalAlignment: Text.AlignVCenter
            }

            background: Rectangle {
                border.width: 0
                color: control.highlightedIndex === index ? Qt.lighter("#757064", 1.5) : "#757064"
            }
        }
    }
}
