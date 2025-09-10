import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

ApplicationWindow {
    width: 640
    height: 480
    visible: true
    title: qsTr("Explainer DB editor")
    footer: Label{
        id: errorLabel
        Connections{
            target: categories
            function onErrorOccured(error){
                errorLabel.text=error
            }
        }
    }

    header: ToolBar{
        RowLayout{
            anchors.fill: parent
            ToolButton{
                text: "Open"
                onClicked: {
                    fileDialog.saveMode=false
                    fileDialog.open()
                }
            }
            ToolButton{
                text: "Save"
                onClicked: {
                    fileDialog.saveMode=true
                    fileDialog.open()
                }
            }
            TabBar{
                id: barId
                Layout.fillWidth: true
                width: parent.width
                TabButton{
                    text: "Categories"
                }
                TabButton{
                    text: "Words"
                }
                TabButton{
                    text: "Preview"
                }
            }
        }
    }

    StackLayout{
        anchors.fill: parent
        currentIndex: barId.currentIndex
        Item{
            Layout.fillHeight: true
            Layout.fillWidth: true
            ListView{
                id: catLV
                model: categories
                anchors.fill: parent
                delegate: Rectangle{
                    width: catLV.width
                    height: 200
                    color: "transparent"
                    border.width: 1
                    RowLayout{
                        anchors.fill: parent
                        Image {
                            source: image
                            width: height
                            height: parent.height
                            MouseArea{
                                anchors.fill: parent
                                anchors.margins: 10
                                hoverEnabled: true
                                RoundButton{
                                    anchors.left: parent.left
                                    anchors.bottom: parent.bottom
                                    text:"✏️"
                                    visible: parent.containsMouse
                                    onClicked: {
                                        imageLoaderDialog.index=index
                                        imageLoaderDialog.open()
                                    }
                                }
                            }
                        }
                        TextInput{
                            Layout.fillWidth: true
                            text: name
                        }
                    }
                }
            }

        }
        Item{
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
        Item{
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }

    FileDialog{
        id: imageLoaderDialog
        property int index
        fileMode: FileDialog.OpenFile
        onAccepted: categories.setImage(index, selectedFile)
    }

    FileDialog{
        id: fileDialog
        property bool saveMode: false
        fileMode: saveMode?FileDialog.SaveFile:FileDialog.OpenFile
        onAccepted: saveMode?categories.saveToFile(selectedFile):categories.filePath=selectedFile
    }

}
