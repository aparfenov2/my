import QtQuick 2.0

Row { // topMenu
    property int labWidth: 40

    ProgressBar {
        width: parent.labWidth
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        value: 70
    }
    MyLabel {
        width: parent.labWidth
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        topText: "КАНАЛ"
        botText: "50X"
    }
    MyLabel {
        width: parent.labWidth
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        topText: "F ПРД"
        botText: "1074 МГЦ"
    }
    MyLabel {
        width: parent.labWidth
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        topText: "gly"
        botText: "12,0 МКС"
    }
    MyLabel {
        width: parent.labWidth
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        topText: "F ПРМ"
        botText: "1011 МГЦ"
    }
    MyLabel {
        width: parent.labWidth
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        topText: "gly"
        botText: "12,0 МКС"
    }
    MyLabel {
        width: parent.labWidth
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        topText: "ГОТОВ"
        botText: ""
    }
}
