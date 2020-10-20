import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

FocusScope {
    id: focusScope
    height: 40

    property alias font: control.font
    property alias text: control.text

    signal clicked

    RoundButton {
        id: control
        radius: 10
        anchors.fill: parent

        font.pointSize: 12
        font.family: "Cooper Black"

        focus: true
        activeFocusOnTab: true
        focusPolicy: Qt.StrongFocus

        onClicked: focusScope.clicked()
        Keys.onReturnPressed: focusScope.clicked()
        Keys.onEnterPressed: focusScope.clicked()

        contentItem: Text {
            text: control.text
            font: control.font
            opacity: enabled ? 1.0 : 0.3
            color: control.down ? Qt.lighter("#F4F2E5", 1.5) : (control.hovered ? Qt.lighter("#F4F2E5", 1.3) : "#F4F2E5")
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }

        background: Rectangle {
            implicitWidth: 100
            implicitHeight: 40
            radius: control.radius
            opacity: enabled ? 1 : 0.3
            border.width: (control.visualFocus || control.activeFocus) ? 2 : 0
            border.color: "#50AA50"
            color: control.down ? Qt.lighter("#757064", 1.5) : (control.hovered ? Qt.lighter("#757064", 1.3) : "#757064")
        }
    }
}
