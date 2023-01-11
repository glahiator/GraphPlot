#ifndef PLC_H
#define PLC_H

#include <QObject>
#include <QUdpSocket>
#include <QString>
#include <QTimer>
#include <QVariant>

#include "Utilites.h"
//#include "snap7.h"

struct cyldr_desc {
    cyldr_desc() { pA = 0.0; pB = 0.0; sY = 0.0; fY = 0.0; fS = 0.0; VS = 0.0; }
    double pA; // Давление в поршневой полости  цилиндра
    double pB; // Давление в штоковой полости  цилиндра
    double sY; // Задание на проп. клапан  цилиндра
    double fY; // Обратная связь положения золотника проп. клапана  цилиндра
    double fS; // Обратная связь положения штока  цилиндра
    double VS; // Обратная связь скорости штока  цилиндра (найти производную от fS)
};
struct pump_desc {
    pump_desc() { pP = 0.0; fD = 0.0; tfD = 0.0; tfdS = 0.0; CAN = 0.0; }
    double pP;
    double fD;
    double tfD;
    double tfdS;
    double CAN;
};
struct cyldr_vals {
    cyldr_desc left;
    cyldr_desc right;
};
struct pump_vals {
    pump_desc left;
    pump_desc right;
};
struct plc_pack {
    cyldr_vals cylinders;
    pump_vals pumps;
};

class PLC : public QObject
{
    Q_OBJECT
public:
    explicit PLC(quint16 _bind_port, QHostAddress _host, quint16 _send_port = 8888, QObject *parent = nullptr);
    ~PLC();

private:
    void Receive();
    void SendReceivedPack();
    quint16 bind_port;
    QHostAddress host;
    quint16 send_port;
    QUdpSocket * socket;
    qint64 previousTime;
    cyldr_vals cylinder;
    pump_vals pump;
    QTimer * timer;
//    TS7Client *Client;

signals:
    void ready_plc( plc_pack pack );
};

#endif // PLC_H
