import QtQuick 2.14
import QtQuick.Controls 2.14

Switch {
    id: control

    bottomPadding: 5

    implicitWidth: indicator.implicitWidth + 15
    implicitHeight: scto.dp(43)

    property var textColor_: "#f0f0f0"
    property var textPressedColor_: "#cc9922"
    property var activeColor: "#ffffff"
    property var mainColor: "#BEC0C1"

    property var indicatorColor_: "#34aadc"

    property var showTopBorders_ : false
    property var showBottomBorders_ : false
    property var showCenterBorders_ : false
    property var textMargin_: scto.dp(40)

    property var soundOn_: true

    background: Rectangle{
        color: "#454647"
        border.color: "#404142"
        border.width: 1
        radius: 10
        opacity: 1.0
    }

     indicator: Rectangle {
         implicitWidth: 40
         implicitHeight: 14
         x: (control.width - width)/2
         y: 5
         radius: 13
         color: control.checked ? indicatorColor_ : "transparent"
         border.color: "#cccccc"

         Rectangle {
             x: control.checked ? parent.width - width : 0
             width: 14
             height: 14
             radius: 7
             color: control.down ? "#cccccc" : "#ffffff"
             border.color: control.checked ? (control.down ? "#17a81a" : "#21be2b") : "#999999"
         }
     }

     contentItem: Text {
          anchors {
             bottom: control.bottom
             horizontalCenter: indicator.horizontalCenter
             top: indicator.bottom
             margins: 3
          }

          text: control.text
          font: control.font
          opacity: enabled ? 1.0 : 0.6
          color: textColor_
          elide: Text.ElideNone
          verticalAlignment: Text.AlignBottom
          horizontalAlignment: Text.AlignHCenter
      }


     Rectangle{
         visible: showBottomBorders_
         width: 1
         height: scto.dp(15)
         color: control.pressed ? activeColor : mainColor
         anchors.left: parent.left
         anchors.bottom: parent.bottom
     }

     Rectangle{
         visible: showBottomBorders_
         height: 1
         width: scto.dp(15)
         color: control.pressed ? activeColor : mainColor
         anchors.left: parent.left
         anchors.bottom: parent.bottom
     }

     Rectangle{
         visible: showCenterBorders_
         height: 1
         width: scto.dp(30)
         color: control.pressed ? activeColor : mainColor
         anchors.horizontalCenter: parent.horizontalCenter
         anchors.bottom: parent.bottom
     }

     Rectangle{
         visible: showBottomBorders_
         width: 1
         height: scto.dp(15)
         color: control.pressed ? activeColor : mainColor
         anchors.right: parent.right
         anchors.bottom: parent.bottom
     }

     Rectangle{
         visible: showBottomBorders_
         height: 1
         width: scto.dp(15)
         color: control.pressed ? activeColor : mainColor
         anchors.right: parent.right
         anchors.bottom: parent.bottom
     }


     //TOP
     Rectangle{
         visible: showTopBorders_
         width: 1
         height: scto.dp(15)
         color: control.pressed ? activeColor : mainColor
         anchors.left: parent.left
         anchors.top: parent.top
     }

     Rectangle{
         visible: showTopBorders_
         height: 1
         width: scto.dp(15)
         color: control.pressed ? activeColor : mainColor
         anchors.left: parent.left
         anchors.top: parent.top
     }

     Rectangle{
         visible: showCenterBorders_
         height: 1
         width: scto.dp(30)
         color: control.pressed ? activeColor : mainColor
         anchors.horizontalCenter: parent.horizontalCenter
         anchors.top: parent.top
     }

     Rectangle{
         visible: showTopBorders_
         width: 1
         height: scto.dp(15)
         color: control.pressed ? activeColor : mainColor
         anchors.right: parent.right
         anchors.top: parent.top
     }

     Rectangle{
         visible: showTopBorders_
         height: 1
         width: scto.dp(15)
         color: control.pressed ? activeColor : mainColor
         anchors.right: parent.right
         anchors.top: parent.top
     }

     onToggled:{

         if(soundOn_ && !player.muted)
         {
             console.log("SWITCH toggled")

             player.source = "qrc:/sounds/switch01.wav"
             player.volume = 1.0
             player.play()
         }
     }
}
