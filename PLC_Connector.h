#ifndef PLC_CONNECTOR_H
#define PLC_CONNECTOR_H

#include <QObject>
#include <QUdpSocket>
#include <QString>
#include <QTimer>
#include <QVariant>

#include "Utilites.h"
#include "CalculateCylinder.h"

struct PLC_Data {
    CylinderVals right;
    CylinderVals left;
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

    CylinderVals right_vals;
    CylinderVals left_vals;

signals:
    void plcDataReceive(PLC_Data pack);
};

#endif // PLC_CONNECTOR_H
