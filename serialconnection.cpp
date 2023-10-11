
#include <QtSerialPort/QtSerialPort>
#include <QMessageBox>
#include "serialconnection.h"

SerialConnection::SerialConnection( QWidget *parent ) :
    QWidget( parent ),
    serial( new QSerialPort )
{
    deviceFound = false;

    connect( serial, &QSerialPort::errorOccurred, this, &SerialConnection::HandleError, Qt::DirectConnection );
    connect( serial, &QSerialPort::readyRead, this, &SerialConnection::ReadData, Qt::DirectConnection );
}

SerialConnection::~SerialConnection()
{

}

void SerialConnection::ConnectDevice()
{
    const auto infos = QSerialPortInfo::availablePorts();
    deviceFound = false;

    for ( const QSerialPortInfo &info : infos )
    {
//        qDebug() << "*****************************";
//        qDebug() << "Port Name: " << info.portName();
//        qDebug() << "Serial Number: " << info.serialNumber();
//        qDebug() << "Description: " << info.description();
//        qDebug() << "Manufacturer: " << info.manufacturer();
//        qDebug() << "System Location: " << info.systemLocation();
//        qDebug() << "IDs: " << info.vendorIdentifier() << ":" << info.productIdentifier();

        if ( info.serialNumber() == COMMUNICATION_USB_DEVICE_SN )
        {
            deviceFound = true;

            if ( serial->isOpen() == false )
            {
                serial->setPortName( info.portName() );
                serial->setBaudRate( 2000000 );
                serial->setDataBits( QSerialPort::Data8 );
                serial->setParity( QSerialPort::NoParity );
                serial->setStopBits( QSerialPort::OneStop );
                serial->setFlowControl( QSerialPort::NoFlowControl );

                if ( serial->open( QIODevice::ReadWrite ) )
                {
                    qDebug() << "Open serial " + info.portName() + " ok";
                    serial->setDataTerminalReady( true );
                    emit SerialUpdateConnection( serial->isOpen() );
                }
                else
                {
                    qDebug() << "Open serial error";
                }
            }
        }
    }

    if ( deviceFound == false )
    {
        QMessageBox::information( this, tr( "Graphity" ), tr( "No device found." ) );
    }
}

void SerialConnection::DisconnectDevice()
{
    if ( serial->isOpen() == true )
    {
        serial->close();
        emit SerialUpdateConnection( serial->isOpen() );
        qDebug() << "Close serial ok";
    }
}

void SerialConnection::SendAck()
{
    QByteArray ack;

    ack.append( ( char ) USB_CONNECTION_PACKET_STX );
    ack.append( ( char ) 0x05 );
    ack.append( ( char ) 0x00 );
    ack.append( ( char ) USB_CONNECTION_PACKET_TYPE_ACK );
    ack.append( ( char ) USB_CONNECTION_PACKET_ETX );

    serial->write( ack );
    qDebug() << "-> ack";
}

void SerialConnection::SendNack()
{
    QByteArray nack;

    nack.append( ( char ) USB_CONNECTION_PACKET_STX );
    nack.append( ( char ) 0x05 );
    nack.append( ( char ) 0x00 );
    nack.append( ( char ) USB_CONNECTION_PACKET_TYPE_NACK );
    nack.append( ( char ) USB_CONNECTION_PACKET_ETX );

    serial->write( nack );
    qDebug() << "-> nack";
}

void SerialConnection::SendPacket( QByteArray packet )
{
    serial->write( packet );
    qDebug() << "-> " + packet.toHex();
}

void SerialConnection::HandleError( QSerialPort::SerialPortError error )
{
    if ( error == QSerialPort::ResourceError )
    {
        if ( serial->isOpen() )
        {
            serial->close();
            emit SerialUpdateConnection( serial->isOpen() );
            qDebug() << "Close serial ok";
        }
    }
}

void SerialConnection::ReadData()
{
    static QByteArray packet;
    static quint16 packetSize;
    static uint8_t packetType;
    static uint8_t packetInfoId;
    int indexStx;

    packet.append( serial->readAll() );

    while ( packet.isEmpty() == false )
    {
        if ( ( indexStx = packet.indexOf( USB_CONNECTION_PACKET_STX ) ) > 0 )
        {
            packet.remove( 0, indexStx );
        }
        else if ( packet.indexOf( 0x02 ) == -1 )
        {
            packet.clear();
        }

        if ( packet.size() < 3 )
        {
            return;
        }
        else
        {
            packetSize = *( ( qint16 * )( packet.data() + 1 ) );
        }

        if ( packet.size() >= packetSize )
        {
            if ( ( packet.at( 0 ) == USB_CONNECTION_PACKET_STX ) && ( packet.at( packetSize - 1 ) == USB_CONNECTION_PACKET_ETX ) )
            {
                packetType = packet.at( USB_CONNECTION_PACKET_TYPE_OFFSET );

                if ( packetType == USB_CONNECTION_PACKET_TYPE_ACK )
                {
                    qDebug() << "<- ack";
                    emit ReceiveAck();
                }
                else if ( packetType == USB_CONNECTION_PACKET_TYPE_NACK )
                {
                    qDebug() << "<- nack";
                    emit ReceiveNack();
                }
                else if ( packetType == USB_CONNECTION_PACKET_TYPE_INFO )
                {
                    packetInfoId = packet.at( USB_CONNECTION_PACKET_INFO_ID_OFFSET );

                    if ( packetInfoId == USB_CONNECTION_PACKET_INFO_ID_STRING )
                    {
                        qDebug() << "<- string";
                        emit ReceivePacketInfoString( packet.first( packetSize ).sliced( USB_CONNECTION_PACKET_INFO_DATA_OFFSET, packetSize - USB_CONNECTION_PACKET_SERVICE_BYTES_NUM ) );
                    }
                    else if ( packetInfoId == USB_CONNECTION_PACKET_INFO_ID_DEVICE_INERTIAL_DATA )
                    {
                        qDebug() << "<- inertial data";
                        emit ReceivePacketInfoDeviceInertialData( packet.first( packetSize ).sliced( USB_CONNECTION_PACKET_INFO_DATA_OFFSET, packetSize - USB_CONNECTION_PACKET_SERVICE_BYTES_NUM ) );
                    }
                    else if ( packetInfoId == USB_CONNECTION_PACKET_INFO_ID_DEVICE_SETTINGS )
                    {
                        qDebug() << "<- device settings";
                        emit ReceivePacketInfoDeviceSettings( packet.first( packetSize ).sliced( USB_CONNECTION_PACKET_INFO_DATA_OFFSET, packetSize - USB_CONNECTION_PACKET_SERVICE_BYTES_NUM ) );
                    }
                    else if ( packetInfoId == USB_CONNECTION_PACKET_INFO_ID_DEVICE_CONNECTED )
                    {
                        qDebug() << "<- device connected";
                        emit ReceivePacketInfoDeviceConnect( packet.first( packetSize ).sliced( USB_CONNECTION_PACKET_INFO_DATA_OFFSET, packetSize - USB_CONNECTION_PACKET_SERVICE_BYTES_NUM ) );
                    }
                }
                else
                {
                    qDebug() << "<- packet type undefined";
                }

                packet.remove( 0, packetSize );
            }
            else
            {
                packet.remove( 0, packetSize );
                SendNack();
            }
        }
        else
        {
            return;
        }
    }
}
