/*
The MIT License (MIT)

Copyright (c) 2014 Kimmo Lindholm

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
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
    /* Enable moving averaging */

    char buf[2] = {0};

    buf[0] = amgReservedRegister1F;
    buf[1] = 0x50;
    writeBytes(amgAddress, buf, 2);

    buf[1] = 0x45;
    writeBytes(amgAddress, buf, 2);

    buf[1] = 0x57;
    writeBytes(amgAddress, buf, 2);

    buf[0] = amgAverageRegister;
    buf[1] = 0x20; /* MAMOD=1 */
    writeBytes(amgAddress, buf, 2);

    buf[0] = amgReservedRegister1F;
    buf[1] = 0x00;
    writeBytes(amgAddress, buf, 2);

    QByteArray res = writeThenRead(amgAddress, amgAverageRegister, 1);

    qDebug() << "AVE" << QString::number( res.at(0), 16 );

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
