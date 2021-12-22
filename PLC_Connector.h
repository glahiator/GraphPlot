#ifndef PLC_CONNECTOR_H
#define PLC_CONNECTOR_H

#include <QObject>
#include <QUdpSocket>
#include <QString>
#include <QTimer>
#include <QVariant>

#include "Utilites.h"

struct PLC_Pack {
    QVector<quint16> adc1_data;
    QVector<quint16> adc3_data;
    QVector<quint16> adc1_filtered_data;
    QVector<quint16> adc3_filtered_data;
    quint32 millisec;
};

class PLC_Connector : public QObject
{
    Q_OBJECT
public:
    explicit PLC_Connector(quint16 _bind_port, QHostAddress _host, quint16 _send_port = 8888, QObject *parent = nullptr);
    ~PLC_Connector();
    void Receive();
    void SendReceivedPack();

    QTimer * timer;

private:
    quint16 bind_port;
    QHostAddress host;
    quint16 send_port;
    QUdpSocket * socket;
    qint64 previousTime;

signals:
    void plcPackReceive(PLC_Pack pack);
};

#endif // PLC_CONNECTOR_H
