import QtQuick 2.15
import QtQuick.Controls 2.15

FocusScope
{
    height: 30

    property alias text: entry.text
    property alias echoMode: entry.echoMode
    property alias placeholderText: entry.placeholderText

    TextField {
        id: entry
        anchors.fill: parent
        verticalAlignment: TextInput.AlignVCenter
        padding: 5
        color: "#757064"
        font.pointSize: 12
        font.family: "Cooper Black"
        focus: true
    }
}
