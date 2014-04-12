/*
  Copyright (C) 2013 Jolla Ltd.
  Contact: Thomas Perl <thomas.perl@jollamobile.com>
  All rights reserved.

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Jolla Ltd nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

import QtQuick 2.0
import Sailfish.Silica 1.0
import tohiri.TohIR 1.0
import QtMultimedia 5.0
import Sailfish.Media 1.0



Page
{
    id: page

    property bool pause: false
    property real gridOpacity: 0.5

    SilicaFlickable
    {
        anchors.fill: parent

        PullDownMenu
        {
            id: pdm
            MenuItem
            {
                text: "About"
                onClicked: pageStack.push(Qt.resolvedUrl("aboutPage.qml"))
            }

            MenuItem
            {
                text: "Save image"
                onClicked: saveTimer.start()
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
            height: 480
        }

        /* the 8x8 grig for temperature gradient */
        Grid
        {
            z: 2
            columns: 8
            anchors.centerIn: parent

            Repeater
            {
                id: gradient
                model: 64

                Rectangle
                {
                    width: 60
                    height: 60
                    opacity: gridOpacity
                    color: "#000000"
                }
            }

        }
        Row
        {
            id: mamLabels
            x: Theme.paddingLarge
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: videoPreview.bottom
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

    }

    TohIR
    {
        id: tohir

        onTemperaturesChanged:
        {
            var res = tohir.temperatures

            for (var i=0; i<64 ; i++)
            {
                var tmp = gradient.itemAt(i)
                tmp.color = res[i];
//                console.log(res[i])
            }


        }
    }

    Timer
    {
        interval: 250
        repeat: true
        running: applicationActive && page.status === PageStatus.Active &&
                 !pdm.active && !saveTimer.running && !pause
        onTriggered:
        {
            tohir.startScan()
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

}


