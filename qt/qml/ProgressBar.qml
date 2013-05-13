import QtQuick 2.0

Item {
    id: progressbar

    property int minimum: 0
    property int maximum: 100
    property int value: 0
    property color color: "#77B753"

    width: 250; height: 23
    clip: true

    Rectangle {
        id: border
        anchors.fill: parent
        anchors.bottomMargin: 1
        anchors.rightMargin: 1
        color: "transparent"
        border.width: 1
        border.color: parent.color
    }

    Rectangle {
        id: highlight
        property int widthDest: ( ( progressbar.width * ( value - minimum ) ) / ( maximum - minimum ) - 4 )
        width: highlight.widthDest

        Behavior on width {
            SmoothedAnimation {
                velocity: 1200
            }
        }

        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
            margins: 2
        }
        color: parent.color
    }
}
