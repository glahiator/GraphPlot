#include "PLC_Connector.h"

PLC_Connector::PLC_Connector(quint16 _bind_port, QHostAddress _host, quint16 _send_port, QObject *parent)
    : QObject(parent), bind_port(_bind_port), host(_host), send_port(_send_port)
{
    socket = new QUdpSocket();
    socket->setSocketOption(QAbstractSocket::LowDelayOption, QVariant(1));
    socket->bind( bind_port, QUdpSocket::ShareAddress );
    if ( socket->open(QIODevice::ReadWrite) )
    {
        socket->readAll();
        connect( socket, &QUdpSocket::readyRead, this, &PLC_Connector::Receive );

    }
    else
    {
        qDebug() << "Sencor Connector socket not open";
    }
    timer = new QTimer();
    timer->stop();
    timer->setInterval(1000);
    connect( timer, &QTimer::timeout, this, &PLC_Connector::SendReceivedPack );

    timer->start(1000);
    previousTime = GetCurrentTime1();
}

PLC_Connector::~PLC_Connector()
{
    delete socket;
    delete timer;
}

void PLC_Connector::Receive()
{
    previousTime = GetCurrentTime1();

    right_vals = CylinderVals();
    left_vals = CylinderVals();
}

void PLC_Connector::SendReceivedPack()
{
    qint64 currentTime = GetCurrentTime1();
    if ( currentTime - previousTime > 500 )
    {
//        qDebug() << currentTime  <<  previousTime << GetCurrentTime1();
        return;
    }
    else
    {
//        qDebug() << currentTime  <<  previousTime << GetCurrentTime1() << currentTime - previousTime;

        PLC_Data data;
        data.right = right_vals;
        data.left = left_vals;
        emit plcDataReceive( data );
    }
}

