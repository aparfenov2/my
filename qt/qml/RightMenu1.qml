import QtQuick 2.0
//import MyLib 1.0

Item {
    anchors.margins: 3
    Column {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        spacing: 3
        property int btnHeight: 40

        MyButton {
            id : izm_bt
            anchors.left: parent.left
            anchors.right: parent.right
            height: parent.btnHeight
            topText: "ИЗМЕРЕНИЕ"
            midText: "C"
            midFamily: "myvi_gly2"
            botText: "СТАРТ"
            KeyNavigation.down: dme_bt
            focus: true
        }


        MyButton {
            id : dme_bt
            QtObject {
                id : dme_vm
                property string midText: appModel.dme_channel + appModel.dme_suffixStr
//                Component.onCompleted: {
//                    appModel.dmeModelChanged.connect(function() {
//                        midText = appModel.dmeModel.channel + appModel.dmeModel.suffixStr;
//                    });
//                }
            }
            anchors.left: parent.left
            anchors.right: parent.right
            height: parent.btnHeight
            topText: "КАНАЛ DME"
            midText: dme_vm.midText
            botText: ""
            KeyNavigation.down: out_bt
            Keys.onReturnPressed: {
                var dlg = Qt.createComponent("EditorDlg.qml").createObject(rootView, {"x": 60, "y": 80, "z":1 });
                dlg.focus = true;
                dlg.value = dme_vm.midText
                dlg.suffixes = {0:"x", 1:"y"};
                dlg.validator = [1,128];

                dlg.accepted.connect(function() {
                    dme_bt.focus = true;
                    rightMenu.focus = true;
                    appModel.dme_channel = dlg.value;
                });
            }
        }
        MyButton {
            id : out_bt
            anchors.left: parent.left
            anchors.right: parent.right
            height: parent.btnHeight
            topText: "ВЫХОД"
            midText: "E"
            midFamily: "myvi_gly2"
            botText: "АНТЕННА"
            KeyNavigation.down: lev_bt
        }
        MyButton {
            id : lev_bt
            anchors.left: parent.left
            anchors.right: parent.right
            height: parent.btnHeight
            topText: "УРОВЕНЬ"
            midText: "+10"
            botText: "дБм"
            KeyNavigation.down: hip_bt
        }
        MyButton {
            id : hip_bt
            anchors.left: parent.left
            anchors.right: parent.right
            height: parent.btnHeight
            topText: "ЗАПУСК"
            midText: "ХИП"
            botText: "FFFFF"
            botFamily: "myvi_gly2"
        }
    }
    Text {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        text : "ГЛАВНОЕ"
        color : "white"
        font.pixelSize: 8
    }
}
