#include "PLC.h"

PLC::PLC(quint16 _bind_port, QHostAddress _host, quint16 _send_port, QObject *parent)
    : QObject(parent), bind_port(_bind_port), host(_host), send_port(_send_port)
{
    socket = new QUdpSocket();
    socket->setSocketOption(QAbstractSocket::LowDelayOption, QVariant(1));
    socket->bind( bind_port, QUdpSocket::ShareAddress );
    if ( socket->open(QIODevice::ReadWrite) )
    {
        socket->readAll();
        connect( socket, &QUdpSocket::readyRead, this, &PLC::Receive );
    }
    else
    {
        qDebug() << "PLC socket not open";
    }
    timer = new QTimer();
    timer->stop();
    timer->setInterval(1000);
    connect( timer, &QTimer::timeout, this, &PLC::SendReceivedPack );

    timer->start(1000);
    previousTime = GetCurrentTime1();
}

PLC::~PLC()
{
    delete socket;
    delete timer;
}

void PLC::Receive()
{
    previousTime = GetCurrentTime1();
}

void PLC::SendReceivedPack()
{
    qint64 currentTime = GetCurrentTime1();
    if ( currentTime - previousTime > 500 )
    {
        return;
    }
    else
    {
        plc_pack pack;
        emit ready_plc( pack );
    }
}

