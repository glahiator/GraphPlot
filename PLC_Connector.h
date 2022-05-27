#ifndef PLC_CONNECTOR_H
#define PLC_CONNECTOR_H

#include <QObject>
#include <QUdpSocket>
#include <QString>
#include <QTimer>
#include <QVariant>
//#include "snap7.h"

#ifdef OS_WINDOWS
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
#endif


#include "Utilites.h"

struct PLCvals {
    PLCvals() {
        pA = 0.0;
        pB = 0.0;
        sY = 0.0;
        fY = 0.0;
        fS = 0.0;
        VS = 0.0;
    }
    double pA; // Давление в поршневой полости  цилиндра
    double pB; // Давление в штоковой полости  цилиндра
    double sY; // Задание на проп. клапан  цилиндра
    double fY; // Обратная связь положения золотника проп. клапана  цилиндра
    double fS; // Обратная связь положения штока  цилиндра
    double VS; // Обратная связь скорости штока  цилиндра (найти производную от fS)

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
//    TS7Client *Client;

private:
    quint16 bind_port;
    QHostAddress host;
    quint16 send_port;
    QUdpSocket * socket;
    qint64 previousTime;

signals:
//    void plcDataReceive(PLC_Data pack);
};

#endif // PLC_CONNECTOR_H
