
import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.tohiri.TohIR 1.0
import QtMultimedia 5.0
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
                                            "updateRate": tohir.updateRate,
                                            "granularity": tohir.granularity,
                                            "contrast": tohir.contrast } )
                    dialog.accepted.connect( function()
                    {
                        tohir.gradientOpacity = dialog.gradientOpacity
                        tohir.updateRate = dialog.updateRate
                        tohir.granularity = dialog.granularity
                        tohir.contrast = dialog.contrast
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

        VideoOutput
        {
            id: videoPreview
            source: camera
            width: 480
            anchors.centerIn: parent
        }

        Canvas
        {
            id: grad

            width: (8 * tohir.granularity).toFixed(0)
            height: (8 * tohir.granularity).toFixed(0)

            property real granu: (8 * tohir.granularity).toFixed(0)/8

            anchors.centerIn: parent

            onPaint:
            {
                var ctx = getContext('2d')

                var temps = tohir.temperatures

                var x = 0.0;
                var y = 0.0;

                for (var i=0; i<64 ; i++)
                {
                    ctx.beginPath()
                    ctx.lineWidth = 1.0
                    ctx.strokeStyle = temps[i]
                    ctx.fillStyle = temps[i]
                    ctx.fillRect(x, y, granu, granu)
                    ctx.stroke()

                    x = x + granu
                    if (x >= width)
                    {
                        x = 0.0
                        y = y + granu
                    }
                }
            }
        }

        ShaderEffect
        {
            id: shader

            anchors.centerIn: parent
            anchors.verticalCenterOffset: -60
            height: 640
            width: 480

            property variant videoSource: ShaderEffectSource { sourceItem: videoPreview; hideSource: true }
            property variant gradientSource: ShaderEffectSource { sourceItem: grad; hideSource: true }

            property real op : tohir.gradientOpacity
            property real contrast : tohir.contrast

            smooth: false /* afaik this sets GL_NEAREST  (true sets GL_LINEAR) */

            fragmentShader:
                "
                varying highp vec2 qt_TexCoord0;

                uniform sampler2D videoSource;
                uniform sampler2D gradientSource;
                uniform highp float op;
                uniform highp float contrast;

                void main()
                {
                        highp vec4 gradient = texture2D(gradientSource, qt_TexCoord0);
                        gradient.rgb /= gradient.a;
                        gradient.rgb = (gradient.rgb - 0.5) * contrast + 0.5;
                        gradient.rgb *= gradient.a;
                        gl_FragColor = texture2D(videoSource, qt_TexCoord0) + op * (gradient - texture2D(videoSource, qt_TexCoord0));
                }
                "
        }

        Rectangle
        {
            id: mamBackground
            z: 2
            color: Theme.secondaryHighlightColor
            width: 480
            height: mamLabels.height + mamValues.height + 10
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: shader.bottom
        }

        Row
        {
            id: mamLabels
            z: 3
            x: Theme.paddingLarge
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: mamBackground.top

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

        Rectangle
        {
            id: hotspotMarker
            /*
              Corner of videopreview is at 30,100  (-8)
            */
            x: 25
            y: 95
            width: 10
            height: 10
            rotation: 45
            color: "white"
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

            hotspotMarker.x = 55 + ((60 * hotspot) % 480)
            hotspotMarker.y = 135 + (80 * ((hotspot/8)|0))
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


