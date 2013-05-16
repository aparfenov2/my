import QtQuick 2.0

Rectangle { // dialog
    id : dialog
    width: 140
    height: 80
    color: "#25ffffff"

    property variant suffixes
    property variant validator

    property string value: ""
    property string suffix: ""

    signal accepted

    Timer {
            interval: 10; running: true; repeat: false;
            onTriggered: {
                txtInout.focus = true
            }
    }

    Column {
        anchors.centerIn: parent
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            color: "white"
            text: "Введите знач.:"
            font.pointSize: 12
        }
        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            TextInput {
                id : txtInout
                anchors.verticalCenter: parent.verticalCenter
                color: "white"
                text: dialog.value
                font.pointSize: 12
                onTextChanged: dialog.value = txtInout.text
            }
            TextInput {
                id : sfxInout
                anchors.verticalCenter: parent.verticalCenter
                color: "white"
                text: dialog.suffix
                font.pointSize: 12
                onTextChanged: dialog.suffix = sfxInout.text
            }
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
