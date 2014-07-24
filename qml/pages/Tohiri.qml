
import QtQuick 2.0
import Sailfish.Silica 1.0
import tohiri.TohIR 1.0
import QtMultimedia 5.0
import Sailfish.Media 1.0
import "."

Page
{
    id: page

    SilicaFlickable
    {
        anchors.fill: parent

        PullDownMenu
        {
            id: pdm

            visible: !saveTimer.running

            MenuItem
            {
                text: "About"
                onClicked: pageStack.push(Qt.resolvedUrl("aboutPage.qml"),
                                          { "version": tohir.version } )
            }
            MenuItem
            {
                text: "Settings"
                onClicked:
                {
                    var dialog = pageStack.push(Qt.resolvedUrl("SettingsDialog.qml"),
                                          { "gradientOpacity": tohir.gradientOpacity,
                                            "updateRate": tohir.updateRate } )
                    dialog.accepted.connect( function()
                    {
                        tohir.gradientOpacity = dialog.gradientOpacity
                        tohir.updateRate = dialog.updateRate
                        tohir.saveSettings()
                    } )
                }

            }
            MenuItem
            {
                text: "Save image"
                onClicked:
                {
                    saveTimer.start()
                }
            }
        }

        Camera
        {
            id: camera
            flash.mode: Camera.FlashOff
            captureMode: Camera.CaptureStillImage
            focus.focusMode: Camera.FocusContinuous
            onError:
            {
                console.error("error: " + camera.errorString);
            }
        }

        PageHeader
        {
            anchors.top: parent.top
            id: cameraHeader
            title: "TOH Infrared Imager"
        }

        GStreamerVideoOutput
        {
            id: videoPreview
            z: 1
            anchors.centerIn: parent
            source: camera
            width: 480
            //height: 480
        }

        Canvas
        {
            id: grad
            width: 8
            height: 8

            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: videoPreview.top

            onPaint:
            {
                var ctx = getContext('2d')

                var temps = tohir.temperatures

                var x=0;
                var y=0;

                for (var i=0; i<64 ; i++)
                {
                    ctx.beginPath();
                    ctx.lineWidth="1";
                    ctx.strokeStyle=temps[i]
                    ctx.rect(x,y,1,1);
                    ctx.stroke();
                    x=x+1;
                    if (x==8)
                    {
                        x=0;
                        y=y+1;
                    }
                }
            }
        }

        ShaderEffect
        {
            anchors.fill: videoPreview

            property variant videoSource: ShaderEffectSource { sourceItem: videoPreview; hideSource: true }
            property variant gradientSource: ShaderEffectSource { sourceItem: grad; hideSource: true }

            property real op : tohir.gradientOpacity

            fragmentShader:
                "
                varying highp vec2 qt_TexCoord0;

                uniform sampler2D videoSource;
                uniform sampler2D gradientSource;
                uniform highp float op;

                void main()
                {
                        gl_FragColor = texture2D(videoSource, qt_TexCoord0) + op * (texture2D(gradientSource, qt_TexCoord0) - texture2D(videoSource, qt_TexCoord0));
                }
                "
        }

        Rectangle
        {
            id: gridPlaceHolder
            width: 480
            height: 480
            color: "transparent"
            anchors.centerIn: parent
        }

//        /* the 8x8 grig for temperature gradient */
//        Grid
//        {
//            id: tempGrid
//            z: 2
//            columns: 8
//            anchors.centerIn: parent

//            Repeater
//            {
//                id: gradient
//                model: 64

//                Rectangle
//                {
//                    width: 60
//                    height: 60
//                    opacity: tohir.gradientOpacity
//                    color: "#000000"
//                    property string lab: ""
//                    Label
//                    {
//                        anchors.centerIn: parent
//                        text: parent.lab
//                        color: Theme.primaryColor
//                        font.pixelSize: Theme.fontSizeLarge
//                        font.bold: true
//                        horizontalAlignment: Text.AlignHCenter
//                        opacity: 1.0
//                    }
//                }
//            }
//        }

        Rectangle
        {
            id: mamBackground
            z: 2
            color: Theme.secondaryHighlightColor
            width: 480
            height: mamLabels.height + mamValues.height + 10
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: gridPlaceHolder.bottom //videoPreview.bottom
        }

        Row
        {
            id: mamLabels
            z: 3
            x: Theme.paddingLarge
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: mamBackground.top //videoPreview.bottom

            Label
            {
                width: page.width/3
                text: "min"
                color: Theme.secondaryColor
                font.pixelSize: Theme.fontSizeSmall
                horizontalAlignment: Text.AlignHCenter
            }
            Label
            {
                width: page.width/3
                text: "avg"
                color: Theme.secondaryColor
                font.pixelSize: Theme.fontSizeSmall
                horizontalAlignment: Text.AlignHCenter
            }
            Label
            {
                width: page.width/3
                text: "max"
                color: Theme.secondaryColor
                font.pixelSize: Theme.fontSizeSmall
                horizontalAlignment: Text.AlignHCenter
            }
        }
        Row
        {
            id: mamValues
            z: 3
            anchors.top: mamLabels.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            Label
            {
                width: page.width/3
                text: tohir.minTemp
                color: Theme.primaryColor
                font.pixelSize: Theme.fontSizeSmall
                horizontalAlignment: Text.AlignHCenter
            }
            Label
            {
                width: page.width/3
                text: tohir.avgTemp
                color: Theme.primaryColor
                font.pixelSize: Theme.fontSizeSmall
                horizontalAlignment: Text.AlignHCenter
            }
            Label
            {
                width: page.width/3
                text: tohir.maxTemp
                color: Theme.primaryColor
                font.pixelSize: Theme.fontSizeSmall
                horizontalAlignment: Text.AlignHCenter
            }
        }

        Timer
        {
            id: saveTimer
            interval: 500
            repeat: false
            running: false
            onTriggered:
            {
                tohir.saveScreenCapture()
            }
        }

        Timer
        {
            interval: tohir.updateRate
            repeat: true
            running: applicationActive && page.status === PageStatus.Active &&
                     !pdm.active && !saveTimer.running
            triggeredOnStart: true
            onTriggered:
            {
                tohir.startScan()
            }
        }
        Label
        {
            id: thermistorTemperature
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: mamBackground.bottom
            text: "---"
        }
        Label
        {
            id: currentDateTime
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: thermistorTemperature.bottom
            anchors.topMargin: 10
            text: "---"
        }

    }

    TohIR
    {
        id: tohir

        onTemperaturesChanged:
        {
            var res = tohir.temperatures
            var hotspot = tohir.hotSpot

            grad.requestPaint()

//            for (var i=0; i<64 ; i++)
//            {
//                var tmp = gradient.itemAt(i)
//                tmp.color = res[i];
//                if (i === hotspot)
//                    tmp.lab = "+"
//                else
//                    tmp.lab = ""
//            }
        }
    }

    Timer
    {
        id: thermistorReader
        interval: 1000
        repeat: true
        running: applicationActive && page.status === PageStatus.Active
        onTriggered:
        {
            thermistorTemperature.text = tohir.readThermistor()
            currentDateTime.text = Qt.formatDateTime(new Date(), "hh:mm:ss - dd.MM.yyyy")
        }
    }


}


