#include "tohir.h"
#include <QSettings>
#include <QCoreApplication>
#include <QTime>
#include <QtDBus/QtDBus>
#include <QDBusArgument>

#include "amg883x.h"


TohIR::TohIR(QObject *parent) :
    QObject(parent)
{

    // Create seed for the random
    // That is needed only once on application startup
    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());

    m_min = 100.0;
    m_max = -20.0;
    m_avg = 0.0;
    m_hotSpot = 31;

    readSettings();

    controlVdd(true);

    QThread::msleep(300);

    amg = new amg883x(0x68);
}

void TohIR::readSettings()
{
    QSettings s("kimmoli", "tohiri");

    s.beginGroup("View");
    m_gradientOpacity = s.value("gradientOpacity", "0.5").toReal();
    m_updateRate = s.value("updateRate", 500).toInt();
    m_granularity = s.value("granularity", "2.0").toReal();
    m_contrast = s.value("contrast", 1.0).toReal();
    s.endGroup();

    emit gradientOpacityChanged();
    emit updateRateChanged();
    emit granularityChanged();
    emit contrastChanged();
}

void TohIR::saveSettings()
{
    QSettings s("kimmoli", "tohiri");

    s.beginGroup("View");
    s.setValue("gradientOpacity", QString::number(m_gradientOpacity,'f',2) );
    s.setValue("updateRate", m_updateRate);
    s.setValue("granularity", m_granularity);
    s.setValue("contrast", m_contrast);
    s.endGroup();
}


TohIR::~TohIR()
{
    controlVdd(false);
}

int TohIR::randInt(int low, int high)
{
    // Random number between low and high
    return qrand() % ((high + 1) - low) + low;
}

/* Return git describe as string (see .pro file) */
QString TohIR::readVersion()
{
    return QString(APPVERSION);
}

/**/
qreal TohIR::readGradientOpacity()
{
    return m_gradientOpacity;
}

/**/
void TohIR::writeGradientOpacity(qreal val)
{
    m_gradientOpacity = val;

    emit gradientOpacityChanged();
}

int TohIR::readUpdateRate()
{
    return m_updateRate;
}

void TohIR::writeUpdateRate(int val)
{
    m_updateRate = val;

    emit updateRateChanged();
}

qreal TohIR::readGranularity()
{
    return m_granularity;
}

void TohIR::writeGranularity(qreal val)
{
    m_granularity = val;

    emit granularityChanged();
}

qreal TohIR::readContrast()
{
    return m_contrast;
}

void TohIR::writeContrast(qreal val)
{
    m_contrast = val;

    emit contrastChanged();
}


/* Return thermistor temperature */
QString TohIR::readThermistor()
{
    return QString("%1 °C").arg(QString::number(amg->getThermistor(), 'g', 3));
}

/* Start IR Scan function, emit changed after completed */
void TohIR::startScan()
{

//    printf("Thermistor %0.5f\n", amg->getThermistor());

    QList<qreal> res = amg->getTemperatureArray();

    int i;
    qreal thisMax = -40.0;

    m_min = 200.0;
    m_max = -40.0;

    m_avg = 0.0;

//     for (i=0 ; i < 64 ; i++)
//        printf("%0.2f%s", res.at(i), (( i%8 == 0 ) ? "\n" : " ") );

    /* Return color gradient array */

    for (i=0 ; i<64 ; i++)
    {
        /* Just use whole numbers */
        qreal tmp = res.at(i);

        if (tmp > thisMax)
        {
            thisMax = tmp;
            m_hotSpot = i;
        }

        if (tmp > m_max)
            m_max = tmp;

        if (tmp < m_min)
            m_min = tmp;

        m_avg = m_avg + tmp;

    }

    emit maxTempChanged();
    emit minTempChanged();
    emit hotSpotChanged();

    m_avg = m_avg/64;
    emit avgTempChanged();

    /* Get RGB values for each pixel */
    m_temperatures.clear();

    for (i=0 ; i<64 ; i++)
        m_temperatures.append(temperatureColor(res.at(i), m_min, m_max, m_avg));

    emit temperaturesChanged();
}

/* Return temperature color gradients as array */
QList<QString> TohIR::readTemperatures()
{
    return m_temperatures;
}

/* Return minimum, average and maximum temperature of last scan */
QString TohIR::readMinTemp()
{
    return QString("%1 °C").arg(QString::number(m_min, 'g', 3));
}

QString TohIR::readAvgTemp()
{
    return QString("%1 °C").arg(QString::number(m_avg, 'g', 3));
}

QString TohIR::readMaxTemp()
{
    return QString("%1 °C").arg(QString::number(m_max, 'g', 3));
}

int TohIR::readHotSpot()
{
    return m_hotSpot;
}

/* Call dbus method to save screencapture */
void TohIR::saveScreenCapture()
{
    QDate ssDate = QDate::currentDate();
    QTime ssTime = QTime::currentTime();

    QString ssFilename = QString("%8/tohiri-%1%2%3-%4%5%6-%7.png")
                    .arg((int) ssDate.day(),    2, 10, QLatin1Char('0'))
                    .arg((int) ssDate.month(),  2, 10, QLatin1Char('0'))
                    .arg((int) ssDate.year(),   2, 10, QLatin1Char('0'))
                    .arg((int) ssTime.hour(),   2, 10, QLatin1Char('0'))
                    .arg((int) ssTime.minute(), 2, 10, QLatin1Char('0'))
                    .arg((int) ssTime.second(), 2, 10, QLatin1Char('0'))
                    .arg((int) ssTime.msec(),   3, 10, QLatin1Char('0'))
                    .arg(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));


    QDBusMessage m = QDBusMessage::createMethodCall("org.nemomobile.lipstick",
                                                    "/org/nemomobile/lipstick/screenshot",
                                                    "",
                                                    "saveScreenshot" );

    QList<QVariant> args;
    args.append(ssFilename);
    m.setArguments(args);

    if (QDBusConnection::sessionBus().send(m))
        printf("Screenshot success to %s\n", qPrintable(ssFilename));
    else
        printf("Screenshot failed\n");
}


QString TohIR::temperatureColor(qreal temp, qreal min, qreal max, qreal avg)
{
    /* We have 61 different colors - for now */
    static const QString lookup[61] =
    {
        "#0500ff", "#0400ff", "#0300ff", "#0200ff", "#0100ff", "#0000ff",
        "#0002ff", "#0012ff", "#0022ff", "#0032ff", "#0044ff", "#0054ff",
        "#0064ff", "#0074ff", "#0084ff", "#0094ff", "#00a4ff", "#00b4ff",
        "#00c4ff", "#00d4ff", "#00e4ff", "#00fff4", "#00ffd0", "#00ffa8",
        "#00ff83", "#00ff5c", "#00ff36", "#00ff10", "#17ff00", "#3eff00",
        "#65ff00", "#8aff00", "#b0ff00", "#d7ff00", "#fdff00", "#FFfa00",
        "#FFf000", "#FFe600", "#FFdc00", "#FFd200", "#FFc800", "#FFbe00",
        "#FFb400", "#FFaa00", "#FFa000", "#FF9600", "#FF8c00", "#FF8200",
        "#FF7800", "#FF6e00", "#FF6400", "#FF5a00", "#FF5000", "#FF4600",
        "#FF3c00", "#FF3200", "#FF2800", "#FF1e00", "#FF1400", "#FF0a00",
        "#FF0000"
    };

    /* If true span is low, tweak it to around avg */
    if ((max - min) < 20.0)
    {
        max = ( ((avg + 10.0) > max) ? (avg + 10.0) : max );
        min = ( ((avg - 10.0) < min) ? (avg - 10.0) : min );
    }

    /* Adjust low end to 0, to get only positive numbers */
    qreal t = temp - min;

    /* span is 2x max or min difference to average, which is larger */
    qreal span = 2.0 * ((max - avg) > (avg - min) ? (max - avg) : (avg - min));

    /* Scale to 60 points */
    qreal x = (t * (60000.0/span))/1000.0;

    /* just to prevent segfaults, return error color (white) */
    if ( (x < 0.0) || (x > 60.0) )
        return "#FFFFFF";

    return lookup[static_cast<int>(x)]; /* Return corresponding RGB color */
}


void TohIR::controlVdd(bool state)
{
    int fd = open("/sys/devices/platform/reg-userspace-consumer.0/state", O_WRONLY);

    if (!(fd < 0))
    {
        if (write (fd, state ? "1" : "0", 1) != 1)
            qDebug() << "Failed to control VDD.";

        close(fd);
    }

    return;
}
