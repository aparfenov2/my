import QtQuick 2.0

Rectangle {
    width: 100
    height: 62
    color: "#00000000"

    property int topSize: 8
    property int botSize: 8

    property string topText: "КАНАЛ"
    property string botText: "50X"

    property int textMargin: 1
    property color textColor: "white"


    Text {
        id : topLabel
        anchors.top: parent.top
        anchors.topMargin: parent.textMargin
        anchors.horizontalCenter: parent.horizontalCenter
        text: parent.topText
        font.pixelSize: parent.topSize
        color: parent.textColor
    }
    Text {
        anchors.bottom: parent.bottom
        anchors.bottomMargin: parent.textMargin
        anchors.horizontalCenter: parent.horizontalCenter
        text: parent.botText
        font.pixelSize: parent.botSize
        color: parent.textColor
    }
}
