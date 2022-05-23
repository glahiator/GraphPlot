#ifndef SENSOR_H
#define SENSOR_H

#include <QObject>
#include <QUdpSocket>
#include <QString>
#include <QTimer>
#include <QVariant>

#include "Utilites.h"

struct SensorPack {
    QVector<quint16> adc1_data;
    QVector<quint16> adc3_data;
    QVector<quint16> adc1_filtered_data;
    QVector<quint16> adc3_filtered_data;
    quint32 millisec;
};

struct SensorVals {
    SensorVals() { fT_R = 0.0; tfT_R = 0.0; fT_L = 0.0; tfT_L = 0.0; }
    double fT_L; // Объемный расход из блока левого цилиндра
    double fT_R; // Объемный расход из блока правого цилиндра
    double tfT_L; // Температура расхода из блока левого цилиндра
    double tfT_R; // Температура расхода из блока правого цилиндра
};

class Sensor : public QObject
{
    Q_OBJECT
public:
    explicit Sensor(quint16 _bind_port, QHostAddress _host, quint16 _send_port = 8888, QObject *parent = nullptr);
    ~Sensor();
    void Receive();
    void SendReceivedPack();

signals:
    void sensorPackReceive(SensorPack pack);

private:
    quint16 bind_port;
    QHostAddress host;
    quint16 send_port;
    QUdpSocket * socket;
    QTimer * timer;

    QVector<quint16> adc1_data;
    QVector<quint16> adc3_data;
    QVector<quint16> adc1_filtered_data;
    QVector<quint16> adc3_filtered_data;

    quint32 packNumber;
    quint32 millisec;
    qint64 previousTime;
};

#endif // SENSOR_H
