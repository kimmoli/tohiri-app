
import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.tohiri.TohIR 1.0


Page
{
    property string version

    id: page

    SilicaFlickable
    {
        anchors.fill: parent

        contentHeight: column.height

        Column
        {
            id: column

            width: page.width
            spacing: Theme.paddingLarge
            PageHeader
            {
                title: "About tohiri"
            }
            Label
            {
                x: Theme.paddingLarge
                text: "TOH Infrared Imager"
                color: Theme.primaryColor
                font.pixelSize: Theme.fontSizeMedium
                font.bold: true
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Label
            {
                x: Theme.paddingLarge
                text: "(C) 2014 kimmoli"
                color: Theme.primaryColor
                font.pixelSize: Theme.fontSizeMedium
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Label
            {
                x: Theme.paddingLarge
                text: "Version: " + version
                color: Theme.primaryColor
                font.pixelSize: Theme.fontSizeMedium
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }
}


