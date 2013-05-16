import QtQuick 2.0
import MyLib 1.0


Rectangle { // scaler
    width: 640
    height: 480
    transform: Scale { xScale: 2; yScale: 2}

    Item {
        id : appModel
        property string dme_channel: "50"
        property string dme_suffixStr: "Y"

    }

//    Timer {
//            interval: 1000; running: true; repeat: true;
//            onTriggered: {
//                appModel.refresh();
//            }
//    }

    Rectangle { // main
        id : rootView
        width : parent.width /2
        height : parent.height /2
        color: "#203E95"

//        MyDialog {
//            x : 60
//            y : 80
//            z : 1
//        }
        Efficiency {
            x : 20
            y : 30
            z : 1
        }
        Distance {
            x : 180
            y : 30
            z : 1
        }
        COCode {
            x : 210
            y : 100
            z : 1
        }
        Statistics {
            x : 10
            y : 100
            z : 1
        }

        Item { // leftBigColumn
            anchors.left: parent.left
            anchors.right: rightMenu.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.margins: 3

            TopMenu { // topMenu
                id : topMenu
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                height: 20
            }

            OscilImpl { // oscil
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: topMenu.bottom
                anchors.bottom: bottomMenu.top
            }

            BottomMenu { // bottomMenu
                id : bottomMenu
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom

                height: 20
            }
        }

        Loader {
            id : rightMenu
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width : 50
            sourceComponent : rightMenu1
            focus : true
        }

        Component {
            id : rightMenu1
            RightMenu1 {}
        }
    }
}
