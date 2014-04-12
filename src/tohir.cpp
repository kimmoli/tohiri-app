#include "tohir.h"
#include <QSettings>
#include <QCoreApplication>
#include <QTime>
#include <QtDBus/QtDBus>
#include <QDBusArgument>


TohIR::TohIR(QObject *parent) :
    QObject(parent)
{
//    m_var = "";

    // Create seed for the random
    // That is needed only once on application startup
    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());

    m_min = 255;
    m_max = -255;
    m_avg = 0;

}

//void TohIR::readInitParams()
//{
//    QSettings settings;
//    m_var = settings.value("var", "").toString();

//    emit varChanged();
//}

TohIR::~TohIR()
{
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

/* Start IR Scan function, emit changed after completed */
void TohIR::startScan()
{
    int i;

    m_temperatures.clear();

    /* Return jibberish color gradient array */

    for (i=0 ; i<64 ; i++)
        m_temperatures.append(QString("#%1%2%3").arg(randInt(0, 255), 2, 16, QChar('0')).arg(randInt(0, 255), 2, 16, QChar('0')).arg(randInt(0, 255), 2, 16, QChar('0')).toUpper());

    emit temperaturesChanged();

    i = randInt(-10, 100);
    if (i < m_min)
    {
        m_min = i;
        emit minTempChanged();
    }

    if (i > m_max)
    {
        m_max = i;
        emit maxTempChanged();
    }

    i = m_avg;
    m_avg = (m_min + m_max) / 2;

    if (m_avg != i)
    {
        emit avgTempChanged();
    }
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
