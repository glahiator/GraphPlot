#include "SensorConnector.h"

SensorConnector::SensorConnector( quint16 _bind_port, QHostAddress _host, quint16 _send_port, QObject *parent)
: QObject(parent), bind_port(_bind_port), host(_host), send_port(_send_port)
{
    socket = new QUdpSocket();
    socket->setSocketOption(QAbstractSocket::LowDelayOption, QVariant(1));
    socket->bind( bind_port, QUdpSocket::ShareAddress );
    if ( socket->open(QIODevice::ReadWrite) )
    {
        socket->readAll();
        connect( socket, &QUdpSocket::readyRead, this, &SensorConnector::Receive );

    }
    else
    {
        qDebug() << "Sencor Connector socket not open";
    }
    packNumber = 0;
    millisec = 0;

    timer = new QTimer();
    timer->stop();
    timer->setInterval(1000);
    connect( timer, &QTimer::timeout, this, &SensorConnector::SendReceivedPack );
    adc1_data.resize(10);
    adc3_data.resize(10);
    adc1_filtered_data.resize(10);
    adc3_filtered_data.resize(10);
    timer->start(1000);
    previousTime = GetCurrentTime1();
}

SensorConnector::~SensorConnector()
{
    delete socket;
    delete timer;
}

void SensorConnector::Receive()
{
    previousTime = GetCurrentTime1();

    QByteArray datagram;
//    qDebug() << "HERE";

    if (socket->hasPendingDatagrams())
    {
        datagram.resize(int(socket->pendingDatagramSize()));
        socket->readDatagram(datagram.data(), datagram.size());
        uint8_t receiveBuffer[datagram.size() - 2];
        for( int i = 0; i < datagram.size() - 2; i++ )
        {
            receiveBuffer[i] = quint8(datagram.at(i));
        }

        uint16_t crc_calc = calcCRC(receiveBuffer, datagram.size() - 2);
        uint16_t crc_recv = Uniq( quint8(datagram.at(datagram.size() - 2)), quint8(datagram.at( datagram.size() - 1 )) );
        if ( crc_calc == crc_recv )
        {
            adc1_data[0] = Uniq( receiveBuffer[0], receiveBuffer[1] );
            adc1_data[1] = Uniq( receiveBuffer[2], receiveBuffer[3] );
            adc1_data[2] = Uniq( receiveBuffer[4], receiveBuffer[5] );
            adc1_data[3] = Uniq( receiveBuffer[6], receiveBuffer[7] );
            adc1_data[4] = Uniq( receiveBuffer[8], receiveBuffer[9] );
            adc1_data[5] = Uniq( receiveBuffer[10], receiveBuffer[11] );
            adc1_data[6] = Uniq( receiveBuffer[12], receiveBuffer[13] );
            adc1_data[7] = Uniq( receiveBuffer[14], receiveBuffer[15] );
            adc1_data[8] = Uniq( receiveBuffer[16], receiveBuffer[17] );

            adc3_data[0] = Uniq( receiveBuffer[18], receiveBuffer[19] );
            adc3_data[1] = Uniq( receiveBuffer[20], receiveBuffer[21] );
            adc3_data[2] = Uniq( receiveBuffer[22], receiveBuffer[23] );
            adc3_data[3] = Uniq( receiveBuffer[24], receiveBuffer[25] );
            adc3_data[4] = Uniq( receiveBuffer[26], receiveBuffer[27] );
            adc3_data[5] = Uniq( receiveBuffer[28], receiveBuffer[29] );
            adc3_data[6] = Uniq( receiveBuffer[30], receiveBuffer[31] );
            adc3_data[7] = Uniq( receiveBuffer[32], receiveBuffer[33] );

            adc1_filtered_data[0] = Uniq( receiveBuffer[34], receiveBuffer[35] );
            adc1_filtered_data[1] = Uniq( receiveBuffer[36], receiveBuffer[37] );
            adc1_filtered_data[2] = Uniq( receiveBuffer[38], receiveBuffer[39] );
            adc1_filtered_data[3] = Uniq( receiveBuffer[40], receiveBuffer[41] );
            adc1_filtered_data[4] = Uniq( receiveBuffer[42], receiveBuffer[43] );
            adc1_filtered_data[5] = Uniq( receiveBuffer[44], receiveBuffer[45] );
            adc1_filtered_data[6] = Uniq( receiveBuffer[46], receiveBuffer[47] );
            adc1_filtered_data[7] = Uniq( receiveBuffer[48], receiveBuffer[49] );
            adc1_filtered_data[8] = Uniq( receiveBuffer[50], receiveBuffer[51] );

            adc3_filtered_data[0] = Uniq( receiveBuffer[52], receiveBuffer[53] );
            adc3_filtered_data[1] = Uniq( receiveBuffer[54], receiveBuffer[55] );
            adc3_filtered_data[2] = Uniq( receiveBuffer[56], receiveBuffer[57] );
            adc3_filtered_data[3] = Uniq( receiveBuffer[58], receiveBuffer[59] );
            adc3_filtered_data[4] = Uniq( receiveBuffer[60], receiveBuffer[61] );
            adc3_filtered_data[5] = Uniq( receiveBuffer[62], receiveBuffer[63] );
            adc3_filtered_data[6] = Uniq( receiveBuffer[64], receiveBuffer[65] );
            adc3_filtered_data[7] = Uniq( receiveBuffer[66], receiveBuffer[67] );

            packNumber = MakeU_32Value( receiveBuffer[68], receiveBuffer[69], receiveBuffer[70], receiveBuffer[71] );
            millisec   = MakeU_32Value( receiveBuffer[72], receiveBuffer[73], receiveBuffer[74], receiveBuffer[75] );

        }
    }
}

void SensorConnector::SendReceivedPack()
{
    qint64 currentTime = GetCurrentTime1();
    if ( currentTime - previousTime > 500 )
    {
//        qDebug() << currentTime  <<  previousTime << GetCurrentTime1();
        return;
    }
    else
    {
        qDebug() << currentTime  <<  previousTime << GetCurrentTime1() << currentTime - previousTime;
        SensorPack pack;
        pack.adc1_data = adc1_data;
        pack.adc3_data = adc3_data;
        pack.adc1_filtered_data = adc1_filtered_data;
        pack.adc3_filtered_data = adc3_filtered_data;
        pack.millisec = millisec;
        emit sensorPackReceive( pack );
    }
}
