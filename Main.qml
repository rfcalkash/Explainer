import QtQuick
import QtQuick.Controls

Window {
    width: 500
    height: 1000
    visible: true
    title: qsTr("Объяснялка")
    StackView{
        anchors.fill: parent
        initialItem: GridView{
            anchors.fill: parent
            cellWidth: width/2
            cellHeight: cellWidth
        }
    }
}
