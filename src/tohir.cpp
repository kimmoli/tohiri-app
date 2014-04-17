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

    m_min = 100;
    m_max = -20;
    m_avg = 0;
    m_hotSpot = 31;

    readSettings();

    controlVdd(true);

    QThread::msleep(100);

    amg = new amg883x(0x68);
}

void TohIR::readSettings()
{
    QSettings s("kimmoli", "tohiri");

    s.beginGroup("View");
    m_gradientOpacity = s.value("gradientOpacity", "0.5").toReal();
    m_updateRate = s.value("updateRate", 500).toInt();
    s.endGroup();

    emit gradientOpacityChanged();
    emit updateRateChanged();
}

void TohIR::saveSettings()
{
    QSettings s("kimmoli", "tohiri");

    s.beginGroup("View");
    s.setValue("gradientOpacity", QString::number(m_gradientOpacity,'f',2) );
    s.setValue("updateRate", m_updateRate);
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
    return GITHASH;
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
    qDebug() << "m_updateRate read "  << m_updateRate;
    return m_updateRate;
}

void TohIR::writeUpdateRate(int val)
{
    m_updateRate = val;
    qDebug() << "m_updateRate write "  << m_updateRate;

    emit updateRateChanged();
}

/* Start IR Scan function, emit changed after completed */
void TohIR::startScan()
{

    printf("Thermistor %0.5f\n", amg->getThermistor());

    QList<qreal> res = amg->getTemperatureArray();

    int i;
    int thisMax = -20;

    m_min = 100;
    m_max = -20;

    m_temperatures.clear();
    m_avg = 0;

    for (i=0 ; i < 64 ; i++)
        printf("%0.2f%s", res.at(i), (( i%8 == 0 ) ? "\n" : " ") );

    /* Return color gradient array */

    for (i=0 ; i<64 ; i++)
    {
        /* Just use whole numbers */
        int tmp = static_cast<int>(res.at(i));

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

        m_temperatures.append(temperatureColor(tmp));
    }

    emit maxTempChanged();
    emit minTempChanged();

    /* Average is average of last scan */
    m_avg = m_avg/64;
    emit avgTempChanged();

    emit temperaturesChanged();
    emit hotSpotChanged();
}

/* Return temperature color gradients as array */
QList<QString> TohIR::readTemperatures()
{
    return m_temperatures;
}

/* Return minimum, average and maximum temperature of last scan */
QString TohIR::readMinTemp()
{
    return QString("%1 °C").arg(m_min);
}

QString TohIR::readAvgTemp()
{
    return QString("%1 °C").arg(m_avg);
}

QString TohIR::readMaxTemp()
{
    return QString("%1 °C").arg(m_max);
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

    QString ssFilename = QString("/home/nemo/Pictures/tohiri-%1%2%3-%4%5%6-%7.png")
                    .arg((int) ssDate.day(),    2, 10, QLatin1Char('0'))
                    .arg((int) ssDate.month(),  2, 10, QLatin1Char('0'))
                    .arg((int) ssDate.year(),   2, 10, QLatin1Char('0'))
                    .arg((int) ssTime.hour(),   2, 10, QLatin1Char('0'))
                    .arg((int) ssTime.minute(), 2, 10, QLatin1Char('0'))
                    .arg((int) ssTime.second(), 2, 10, QLatin1Char('0'))
                    .arg((int) ssTime.msec(),   3, 10, QLatin1Char('0'));


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


QString TohIR::temperatureColor(int temp)
{
    /* We have 61 different colors - for now */
    static const QString lookup[61] =
    {
        "#FF0000",
        "#FF0a00",
        "#FF1400",
        "#FF1e00",
        "#FF2800",
        "#FF3200",
        "#FF3c00",
        "#FF4600",
        "#FF5000",
        "#FF5a00",
        "#FF6400",
        "#FF6e00",
        "#FF7800",
        "#FF8200",
        "#FF8c00",
        "#FF9600",
        "#FFa000",
        "#FFaa00",
        "#FFb400",
        "#FFbe00",
        "#FFc800",
        "#FFd200",
        "#FFdc00",
        "#FFe600",
        "#FFf000",
        "#FFfa00",
        "#fdff00",
        "#d7ff00",
        "#b0ff00",
        "#8aff00",
        "#65ff00",
        "#3eff00",
        "#17ff00",
        "#00ff10",
        "#00ff36",
        "#00ff5c",
        "#00ff83",
        "#00ffa8",
        "#00ffd0",
        "#00fff4",
        "#00e4ff",
        "#00d4ff",
        "#00c4ff",
        "#00b4ff",
        "#00a4ff",
        "#0094ff",
        "#0084ff",
        "#0074ff",
        "#0064ff",
        "#0054ff",
        "#0044ff",
        "#0032ff",
        "#0022ff",
        "#0012ff",
        "#0002ff",
        "#0000ff",
        "#0100ff",
        "#0200ff",
        "#0300ff",
        "#0400ff",
        "#0500ff"
    };

    if (temp < -20)
        temp = -20;

    if (temp > 100)
        temp = 100;

    return lookup[60-((temp+20)/2)];
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
