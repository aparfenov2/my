import QtQuick 2.0

Row { // bottomMenu
    property int labWidth: 35

    MyLabel {
        width: parent.labWidth
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        topText: "10%"
        botText: ""
    }
    MyLabel {
        width: parent.labWidth
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        topText: "10,5 мкс"
        botText: "100 м"
    }
    MyLabel {
        width: parent.labWidth
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        topText: "ЗАПРОС"
        botText: "+10 дБм"
    }
    MyLabel {
        width: parent.labWidth
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        topText: "ОТВЕТ"
        botText: "-76 дБм"
    }
    MyLabel {
        width: parent.labWidth
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        topText: "КСВН"
        botText: "1,63"
    }
    MyLabel {
        width: parent.labWidth
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        topText: "П"
        botText: "1300"
    }
    MyLabel {
        width: parent.labWidth
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        topText: "ХИП"
        botText: "ИМП.ПАР/С"
    }
}
