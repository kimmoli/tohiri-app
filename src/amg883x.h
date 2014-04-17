#ifndef amg883X_H
#define amg883X_H
#include <QObject>
#include <QString>
#include "driverBase.h"

namespace
{

const unsigned char amgThermistorRegister = 0x0e;
const unsigned char amgTemperatureRegister = 0x80;

}

class amg883x : public DriverBase
{
    Q_OBJECT
public:
    explicit amg883x(unsigned char amg_address);
    ~amg883x();

    void init();

    qreal getThermistor();
    QList<qreal> getTemperatureArray();



private:
    unsigned char amgAddress;
};

#endif // amg883X_H
