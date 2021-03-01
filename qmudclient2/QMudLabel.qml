import QtQuick 2.15
import QtQuick.Controls 2.15

Label {
    color: "#757064"
    font.pointSize: 12
    font.family: "Cooper Black"

    function blink() {
        blinkAnimation.restart()
    }

    NumberAnimation on opacity {
        id: blinkAnimation
        from: 0.0
        to: 1.0
        duration: 250
    }
}
