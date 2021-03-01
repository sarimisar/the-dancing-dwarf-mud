import QtQuick 2.15
import QtQuick.Controls 2.15

FocusScope
{
    height: 30
    //focus: true

    property alias radius: rectangle.radius
    property alias text: entry.text
    property alias echoMode: entry.echoMode

    Rectangle {
        id: rectangle
        color: "#F4F2E5"
        anchors.fill: parent

        radius: 10

        TextInput {
            id: entry
            anchors.fill: parent
            verticalAlignment: TextInput.AlignVCenter
            padding: 5
            color: "#757064"
            font.pointSize: 10
            font.family: "Cooper Black"
            focus: true
        }
    }
}
