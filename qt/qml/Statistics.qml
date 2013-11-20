import QtQuick 2.0

Column {
    Text {
        color: "white"
        text: "СТАТИСТИКА:"
        font.pixelSize: 6
    }
    Row {
        Text {
            color: "white"
            text: "M"
            font.pixelSize: 12
        }
        Column {
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                color: "white"
                text: "1500 М"
                font.pixelSize: 6
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
                font.pixelSize: 6
            }
        }
    }
    Text {
        color: "white"
        text: "s = 0,00"
        font.pixelSize: 6
    }
    Text {
        color: "white"
        text: "СКО = 0,00"
        font.pixelSize: 6
    }
}
