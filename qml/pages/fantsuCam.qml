
import QtQuick 2.0
import Sailfish.Silica 1.0
import tohiri.TohIR 1.0
import QtMultimedia 5.0
import Sailfish.Media 1.0

Page
{
    id: page

    SilicaFlickable
    {
        anchors.fill: parent

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
            title: "FantsuCam"
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

//        ShaderEffect
//        {
//            property variant source: ShaderEffectSource { sourceItem: videoPreview; hideSource: true }
//            anchors.fill: videoPreview
//            property real dividerValue: divSlider.value


//        fragmentShader: "
//uniform float dividerValue;
//uniform sampler2D source;
//uniform lowp float qt_Opacity;
//varying vec2 qt_TexCoord0;

//void main()
//{
//    vec2 uv = qt_TexCoord0.xy;
//    vec4 orig = texture2D(source, uv);
//    vec3 col = orig.rgb;
//    float y = 0.3 *col.r + 0.59 * col.g + 0.11 * col.b;
//    y = y < 0.3 ? 0.0 : (y < 0.6 ? 0.5 : 1.0);
//    if (y == 0.5)
//        col = vec3(0.0, 0.8, 0.0);
//    else if (y == 1.0)
//        col = vec3(0.0, 0.0, 0.8);
//    else
//        col = vec3(0.0, 0.0, 0.0);
//    if (uv.x < dividerValue)
//        gl_FragColor = qt_Opacity * vec4(col, 1.0);
//    else
//        gl_FragColor = qt_Opacity * orig;
//}"
//        }

        ShaderEffect
        {
            property variant source: ShaderEffectSource { sourceItem: videoPreview; hideSource: true }
            property real wiggleAmount: divSlider.value
            anchors.fill: videoPreview

            fragmentShader: "
                varying highp vec2 qt_TexCoord0;
                uniform sampler2D source;
                uniform highp float wiggleAmount;
                void main(void)
                {
                    highp vec2 wiggledTexCoord = qt_TexCoord0;
                    wiggledTexCoord.s += sin(4.0 * 3.141592653589 * wiggledTexCoord.t) * wiggleAmount;
                    gl_FragColor = texture2D(source, wiggledTexCoord.st);
                }
            "
        }



//        ShaderEffect
//        {
//            property variant source: ShaderEffectSource { sourceItem: videoPreview; hideSource: true }
//            anchors.fill: videoPreview
//            property real dividerValue: divSlider.value

//            property real threshold: 0.5
//            property real targetSize: 250 - (200 * threshold) // TODO: fix ...
//            property real resS: targetSize
//            property real resT: targetSize

//            // TODO
//            property real magTol: 0.3
//            property real quantize: 8.0
//            fragmentShader: "
//uniform float dividerValue;
//uniform float threshold;
//uniform float resS;
//uniform float resT;
//uniform float magTol;
//uniform float quantize;

//uniform sampler2D source;
//uniform lowp float qt_Opacity;
//varying vec2 qt_TexCoord0;

//void main()
//{
//    vec4 color = vec4(1.0, 0.0, 0.0, 1.1);
//    vec2 uv = qt_TexCoord0.xy;
//    if (uv.x < dividerValue) {
//        vec2 st = qt_TexCoord0.st;
//        vec3 rgb = texture2D(source, st).rgb;
//        vec2 stp0 = vec2(1.0/resS,  0.0);
//        vec2 st0p = vec2(0.0     ,  1.0/resT);
//        vec2 stpp = vec2(1.0/resS,  1.0/resT);
//        vec2 stpm = vec2(1.0/resS, -1.0/resT);
//        float i00 =   dot( texture2D(source, st).rgb, vec3(0.2125,0.7154,0.0721));
//        float im1m1 = dot( texture2D(source, st-stpp).rgb, vec3(0.2125,0.7154,0.0721));
//        float ip1p1 = dot( texture2D(source, st+stpp).rgb, vec3(0.2125,0.7154,0.0721));
//        float im1p1 = dot( texture2D(source, st-stpm).rgb, vec3(0.2125,0.7154,0.0721));
//        float ip1m1 = dot( texture2D(source, st+stpm).rgb, vec3(0.2125,0.7154,0.0721));
//        float im10 =  dot( texture2D(source, st-stp0).rgb, vec3(0.2125,0.7154,0.0721));
//        float ip10 =  dot( texture2D(source, st+stp0).rgb, vec3(0.2125,0.7154,0.0721));
//        float i0m1 =  dot( texture2D(source, st-st0p).rgb, vec3(0.2125,0.7154,0.0721));
//        float i0p1 =  dot( texture2D(source, st+st0p).rgb, vec3(0.2125,0.7154,0.0721));
//        float h = -1.*im1p1 - 2.*i0p1 - 1.*ip1p1  +  1.*im1m1 + 2.*i0m1 + 1.*ip1m1;
//        float v = -1.*im1m1 - 2.*im10 - 1.*im1p1  +  1.*ip1m1 + 2.*ip10 + 1.*ip1p1;
//        float mag = sqrt(h*h + v*v);
//        if (mag > magTol) {
//            color = vec4(0.0, 0.0, 0.0, 1.0);
//        }
//        else {
//            rgb.rgb *= quantize;
//            rgb.rgb += vec3(0.5, 0.5, 0.5);
//            ivec3 irgb = ivec3(rgb.rgb);
//            rgb.rgb = vec3(irgb) / quantize;
//            color = vec4(rgb, 1.0);
//        }
//    } else {
//        color = texture2D(source, uv);
//    }
//    gl_FragColor = qt_Opacity * color;
//}            "
//        }

        Slider
        {
            id: divSlider
            width: parent.width - 10
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: saveButton.top
            minimumValue: 0.0
            maximumValue: 0.1
            value: 0.05
            stepSize: 0.001
            visible: !saveTimer.running
        }

        Button
        {
            id: saveButton
            visible: !saveTimer.running
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            text: "save"
            onClicked: saveTimer.start()
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

        TohIR
        {
            id: tohir
        }


    }


}


