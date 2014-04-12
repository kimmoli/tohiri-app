#ifndef TOHIR_H
#define TOHIR_H
#include <QObject>

class TohIR : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<QString> temperatures READ readTemperatures NOTIFY temperaturesChanged())
    Q_PROPERTY(QString version READ readVersion NOTIFY versionChanged())
    Q_PROPERTY(QString minTemp READ readMinTemp NOTIFY minTempChanged())
    Q_PROPERTY(QString avgTemp READ readAvgTemp NOTIFY avgTempChanged())
    Q_PROPERTY(QString maxTemp READ readMaxTemp NOTIFY maxTempChanged())
    Q_PROPERTY(int hotSpot READ readHotSpot NOTIFY hotSpotChanged())

    Q_PROPERTY(qreal gradientOpacity READ readGradientOpacity WRITE writeGradientOpacity(qreal) NOTIFY gradientOpacityChanged())
    Q_PROPERTY(int updateRate READ readUpdateRate WRITE writeUpdateRate() NOTIFY updateRateChanged())

public:
    explicit TohIR(QObject *parent = 0);
    ~TohIR();

    QList<QString> readTemperatures();
    Q_INVOKABLE void startScan();
    Q_INVOKABLE void saveScreenCapture();

    QString readVersion();
    QString readMinTemp();
    QString readAvgTemp();
    QString readMaxTemp();
    int readHotSpot();

    qreal readGradientOpacity();
    void writeGradientOpacity(qreal val);

    int readUpdateRate();
    void writeUpdateRate(int val);

    Q_INVOKABLE void readSettings();
    Q_INVOKABLE void saveSettings();

signals:
    void temperaturesChanged();
    void versionChanged();
    void minTempChanged();
    void avgTempChanged();
    void maxTempChanged();
    void hotSpotChanged();

    void gradientOpacityChanged();
    void updateRateChanged();

private:
    int randInt(int low, int high);
//    QString m_var;
    QList<QString> m_temperatures;

    int m_avg;
    int m_min;
    int m_max;
    int m_hotSpot;
    qreal m_gradientOpacity;
    int m_updateRate;
};


#endif // TOHIR_H

