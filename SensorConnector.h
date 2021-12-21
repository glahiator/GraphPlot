#ifndef SENSORCONNECTOR_H
#define SENSORCONNECTOR_H

#include <QObject>
#include <QUdpSocket>
#include <QString>
#include <QTimer>

#include "Utilites.h"

struct SensorPack {
    QVector<quint16> adc1_data;
    QVector<quint16> adc3_data;
    QVector<quint16> adc1_filtered_data;
    QVector<quint16> adc3_filtered_data;
    quint32 millisec;
};

class SensorConnector : public QObject
{
    Q_OBJECT
public:
    explicit SensorConnector(quint16 _bind_port, QHostAddress _host, quint16 _send_port = 8888, QObject *parent = nullptr);
    ~SensorConnector();
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

#endif // SENSORCONNECTOR_H
