import QtQuick 2.0

Rectangle {
    width: 100
    height: 62
    border.width: 1
    border.color: "white"
    radius: 4
    color: focus ? "292929" : "#00000000"

    property int topSize: 8
    property int midSize: 12
    property int botSize: 8

    property string topText: "ИЗМЕРЕНИЕ"
    property string midText: "gly"
    property string botText: "СТАРТ"

    property string topFamily: "Courier"
    property string midFamily: "Courier"
    property string botFamily: "Courier"

    property int textMargin: 3
    property color textColor: "white"


    Text {
        id : topLabel
        anchors.top: parent.top
        anchors.topMargin: parent.textMargin
        anchors.horizontalCenter: parent.horizontalCenter
        text: parent.topText
        font.pixelSize: parent.topSize
        font.family: parent.topFamily
        color: parent.textColor
    }
    Text {
        id : glyLabel
        anchors.centerIn: parent
        text: parent.midText
        color: parent.textColor
        font.pixelSize: parent.midSize
        font.family: parent.midFamily
    }
    Text {
        anchors.bottom: parent.bottom
        anchors.bottomMargin: parent.textMargin
        anchors.horizontalCenter: parent.horizontalCenter
        text: parent.botText
        color: parent.textColor
        font.pixelSize: parent.botSize
        font.family: parent.botFamily
    }
}
