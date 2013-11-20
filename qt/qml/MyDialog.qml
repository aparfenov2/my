import QtQuick 2.0

Rectangle { // dialog
    id : dialog
    width: 140
    height: 80
    color: "#25ffffff"

    signal accepted

    Column {
        anchors.centerIn: parent
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            color: "white"
            text: "ЗАПУСТИТЕ"
            font.pointSize: 12
        }
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            color: "white"
            text: "ИЗМЕРЕНИЕ"
            font.pointSize: 12
        }
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            color: "white"
            text: "G"
            font.pointSize: 12
        }
    }

//    MouseArea {
//        anchors.fill: parent
//        onClicked: {
//            parent.destroy();
//        }
//    }

    Keys.onReturnPressed: {
        accepted();
        dialog.destroy();
    }

}
