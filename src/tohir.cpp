#include "tohir.h"
#include <QSettings>
#include <QCoreApplication>
#include <QTime>


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
