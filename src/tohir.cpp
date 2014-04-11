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



QList<QString> TohIR::readTemperatures()
{

    return m_temperatures;
}

void TohIR::startScan()
{
    int i;

    m_temperatures.clear();

    for (i=0 ; i<64 ; i++)
        m_temperatures.append(QString("#%1%2%3").arg(randInt(0, 255), 2, 16, QChar('0')).arg(randInt(0, 255), 2, 16, QChar('0')).arg(randInt(0, 255), 2, 16, QChar('0')).toUpper());

    emit temperaturesChanged();
}

int TohIR::randInt(int low, int high)
{
    // Random number between low and high
    return qrand() % ((high + 1) - low) + low;
}

QString TohIR::readVersion()
{
    return GITHASH;
}


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

//    notificationSend("Screenshot saved", ssFilename);

}
