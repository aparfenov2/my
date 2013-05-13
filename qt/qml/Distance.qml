import QtQuick 2.0

Column {
    Text {
        anchors.horizontalCenter: parent.horizontalCenter
        color: "white"
        text: "ДАЛЬНОСТЬ"
        font.pixelSize: 6
    }
    Text {
        anchors.horizontalCenter: parent.horizontalCenter
        color: "white"
        text: "1500 М"
        font.pixelSize: 16
    }
    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        height : 1
        color: "white"
    }

    Text {
        anchors.horizontalCenter: parent.horizontalCenter
        color: "white"
        text: "10,00 МКС"
        font.pixelSize: 10
    }
}
