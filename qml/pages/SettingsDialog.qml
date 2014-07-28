
import QtQuick 2.0
import Sailfish.Silica 1.0


Dialog
{
    property real gradientOpacity
    property real granularity
    property int updateRate
    property real contrast

    id: settings

    canAccept: true

    onDone:
    {
        if (result === DialogResult.Accepted)
        {
            gradientOpacity = opacitySlider.value.toFixed(1)
            granularity = granularitySlider.value.toFixed(1)
            updateRate = rateSlider.value
            contrast = contrastSlider.value
        }
    }

    Column
    {
        id: column

        width: parent.width
        spacing: Theme.paddingLarge
        DialogHeader
        {
            acceptText: "Save"
        }
        Slider
        {
            id: opacitySlider
            width: parent.width-10
            label: "Gradient opacity"
            anchors.horizontalCenter: parent.Center
            minimumValue: 0.0
            maximumValue: 1.0
            value: gradientOpacity.toFixed(1)
            stepSize: 0.1
            valueText: value.toFixed(1) * 100 + " %"
        }
        Slider
        {
            id: granularitySlider
            width: parent.width-10
            label: "Gradient granularity"
            anchors.horizontalCenter: parent.Center
            minimumValue: 1.0
            maximumValue: 5.0
            value: granularity
            stepSize: 0.1
            valueText: value.toFixed(1)
        }
        Slider
        {
            id: contrastSlider
            width: parent.width-10
            label: "Gradient contrast"
            anchors.horizontalCenter: parent.Center
            minimumValue: 1.0
            maximumValue: 2.0
            value: contrast
            stepSize: 0.1
            valueText: value.toFixed(1)
        }

        Slider
        {
            id: rateSlider
            width: parent.width-10
            label: "Update interval"
            anchors.horizontalCenter: parent.Center
            minimumValue: 100
            maximumValue: 2000
            value: updateRate
            stepSize: 100
            valueText: value + " ms"
        }
    }
}
