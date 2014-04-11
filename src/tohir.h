#ifndef TOHIR_H
#define TOHIR_H
#include <QObject>

class TohIR : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<QString> temperatures READ readTemperatures NOTIFY temperaturesChanged())

public:
    explicit TohIR(QObject *parent = 0);
    ~TohIR();

    QList<QString> readTemperatures();
    Q_INVOKABLE void startScan();
    Q_INVOKABLE void saveScreenCapture();

//     Q_INVOKABLE void readInitParams();

signals:
    void temperaturesChanged();

private:
    int randInt(int low, int high);
//    QString m_var;
    QList<QString> m_temperatures;
};


#endif // TOHIR_H

