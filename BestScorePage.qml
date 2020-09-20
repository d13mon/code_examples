import QtQuick 2.14
import QtQuick.Controls 2.14

ScrollView {
    id: control

    clip: true

    focus: true

    anchors.fill: parent;

    ScrollBar.vertical: ScrollBar {
        policy: ScrollBar.AlwaysOff
    }

    ScrollBar.horizontal: ScrollBar {
        policy: ScrollBar.AlwaysOff
    }  

    property var lineColor_: "#fff"
    property var lineWidth_: 1

    property var tableRowHeight_ : scto.dp(40)

    function updateScores() {
        longScores.updateScores()
        shortScores.updateScores()
    }

    function close(){
         control.visible = false
    }   

    function scrollContentToStart()
    {
        longScores.scrollContentToStart()
        shortScores.scrollContentToStart()
    }

    signal drawerClicked()

    Connections{
        target: langTest
    }

    Item {
        id: content
        anchors.fill: parent;      

        MainFrame {
            id: mainFrame          

            ContentFrame{             

                Header{
                    id: header

                    height: window.headerHeight_

                    BackButton{
                        id: backButton
                        Keys.forwardTo: [keys]
                        onClicked: control.close()
                    }

                    ResultsTableText{
                        id: headerValue

                        text: qsTr("Your Best Results")

                        height: scto.dp(35)

                        anchors {
                            top: parent.top
                            left: parent.left
                            right: parent.right
                            leftMargin: 0
                            topMargin: 9
                        }

                        horizontalAlignment: Text.AlignHCenter

                        font.bold: true
                        font.pixelSize: textHSizePix_

                        background: Rectangle{
                            color:  "transparent"
                        }                       
                    }

                    DrawerOpenButton {
                        id: drawerMenuIcon

                        anchors {
                            top: parent.top
                            right: parent.right
                        }

                        onClicked: control.drawerClicked()

                        Keys.forwardTo: [keys]
                    }

                }//Header

                SwipeView {
                    id: modesView

                    currentIndex: 0

                    anchors {
                       left: parent.left
                       right: parent.right
                       top: header.bottom
                       bottom: parent.bottom

                       margins: 0
                    }

                    Item {
                        id: firstPage

                        ScoresFrame{
                            id: longScores
                            anchors {
                               fill: parent
                            }

                            testModeIndex_: 2

                            caption_: "Long"
                            logoSource_: "qrc:///images//test_mode_long01.svg"

                            Keys.forwardTo: [keys]
                        }
                    }
                    Item {
                        id: secondPage

                        ScoresFrame{
                            id: shortScores
                            anchors {
                               fill: parent
                            }

                            testModeIndex_: 1

                            caption_: "Short"
                            logoSource_: "qrc:///images//test_mode_short01.svg"

                            Keys.forwardTo: [keys]
                        }
                    }

                     Keys.forwardTo: [keys]
                }

                ModePageIndicator {
                    id: indicator

                    count: modesView.count
                    currentIndex: modesView.currentIndex

                    anchors.bottom: modesView.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                }


            }//ContentFrame
        }//MainFrame
    }//Content   

    Item {
        id: keys
        anchors.fill: parent
        focus: true

        Keys.onBackPressed: { control.close(); event.accepted = true}
        Keys.onLeftPressed: { console.log("Key: LEFT"); event.accepted = true}
        Keys.onRightPressed: { console.log("Key: RIGHT"); event.accepted = true}
    }

    onVisibleChanged: {
        if(visible)
            control.focus = true

        scrollContentToStart()
    }
}
