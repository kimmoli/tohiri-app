#ifndef DRIVERBASE_H
#define DRIVERBASE_H
#include <QString>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <QDebug>
#include <QObject>
#include <QByteArray>

class DriverBase : public QObject {

public:

    // write given bytes to specified address
    bool writeBytes(unsigned char address, char bytes[], int length);

    // read specified amount of bytes
    QByteArray readBytes(unsigned char address, int howManyBytesToRead);

    // first write the register from which to read and then read
    QByteArray writeThenRead(unsigned char address,
                             char registerToRead,
                             int howManyBytesToRead);

private:
    int openDeviceFile(const char *name);
    bool setSlaveAddress(int file, unsigned char address);
    bool i2cWrite(int file, char buffer[], int buffer_length);
    bool i2cRead(int file, char buffer[], int howManyBytesToRead);
};

#endif // DRIVERBASE_H
