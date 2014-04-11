#ifndef TOHIR_H
#define TOHIR_H
#include <QObject>

class TohIR : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<QString> temperatures READ readTemperatures NOTIFY temperaturesChanged())
    Q_PROPERTY(QString version READ readVersion NOTIFY versionChanged())
    Q_PROPERTY(QString minTemp READ readMinTemp NOTIFY minTempChanged())
    Q_PROPERTY(QString maxTemp READ readMaxTemp NOTIFY maxTempChanged())

public:
    explicit TohIR(QObject *parent = 0);
    ~TohIR();

    QList<QString> readTemperatures();
    Q_INVOKABLE void startScan();
    Q_INVOKABLE void saveScreenCapture();

    QString readVersion();
    QString readMinTemp();
    QString readMaxTemp();

//     Q_INVOKABLE void readInitParams();

signals:
    void temperaturesChanged();
    void versionChanged();
    void minTempChanged();
    void maxTempChanged();

private:
    int randInt(int low, int high);
//    QString m_var;
    QList<QString> m_temperatures;
};


#endif // TOHIR_H

