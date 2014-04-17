#include "amg883x.h"

amg883x::amg883x(unsigned char amg_address)
{
    amgAddress = amg_address;

    init();
}

amg883x::~amg883x()
{

}

void amg883x::init()
{

}

qreal amg883x::getThermistor()
{
    QByteArray res = writeThenRead(amgAddress, amgThermistorRegister, 2);

    qreal temp = (double)(res.at(0) | ( res.at(1) << 8 )) / 16.0;

    return temp;
}

QList<qreal> amg883x::getTemperatureArray()
{
    QList<qreal> temp;
    int i;

    QByteArray res = writeThenRead(amgAddress, amgTemperatureRegister, 128);

    temp.clear();

    for ( i = 0 ; i < 64; i++ )
    {
        /* sign extend */
        qint16 x = 0xFFF & ((res.at(i*2) | (res.at((i*2)+1) << 8 ) ));
        qint16 r = (x ^ 0x800) - 0x800;

        temp.append( (qreal)r / 4.0 );
    }

    return temp;
}
