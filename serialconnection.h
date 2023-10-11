
#ifndef SERIALCONNECTION_H
#define SERIALCONNECTION_H

#include <QWidget>
#include <QObject>
#include <QtSerialPort/QSerialPort>


/*******************************************************************************
 *  Macro & Types Definitions
 ******************************************************************************/
#define COMMUNICATION_USB_DEVICE_SN                     "E95019ED63ABFC98"
#define COMMUNICATION_USB_PACKET_WAIT_MAX_MS            2000

#define USB_CONNECTION_PACKET_STX                       0x02
#define USB_CONNECTION_PACKET_ETX                       0x03
#define USB_CONNECTION_PACKET_TYPE_OFFSET               3
#define USB_CONNECTION_PACKET_EXEC_ID_OFFSET            4
#define USB_CONNECTION_PACKET_INFO_ID_OFFSET            4
#define USB_CONNECTION_PACKET_EXEC_DATA_OFFSET          5
#define USB_CONNECTION_PACKET_INFO_DATA_OFFSET          5
#define USB_CONNECTION_PACKET_SERVICE_BYTES_NUM         6

typedef enum
{
    USB_CONNECTION_PACKET_TYPE_UNDEFINED        = 0x00,
    USB_CONNECTION_PACKET_TYPE_ACK              = 0x01,
    USB_CONNECTION_PACKET_TYPE_NACK             = 0x02,
    USB_CONNECTION_PACKET_TYPE_EXEC             = 0x03,
    USB_CONNECTION_PACKET_TYPE_INFO             = 0x04

} usb_connection_packet_type_t;

#define USB_CONNECTION_PACKET_EXEC_ID_ENABLE_INERTIAL_DATA_STREAM_SIZE              7  // | STX | LEN1 | LEN2 | TYP | IDN | ENA | ETX |
#define USB_CONNECTION_PACKET_EXEC_ID_SET_SAMPLE_RATE_SIZE                          8  // | STX | LEN1 | LEN2 | TYP | IDN | SR0 | SP1 | ETX |
#define USB_CONNECTION_PACKET_EXEC_ID_SET_ACC_FULL_SCALE_SIZE                       7  // | STX | LEN1 | LEN2 | TYP | IDN | FS0 | ETX |
#define USB_CONNECTION_PACKET_EXEC_ID_SET_ACC_DLPF_BW_SIZE                          8  // | STX | LEN1 | LEN2 | TYP | IDN | BW0 | BW1 | ETX |
#define USB_CONNECTION_PACKET_EXEC_ID_SET_GYRO_FULL_SCALE_SIZE                      8  // | STX | LEN1 | LEN2 | TYP | IDN | FS0 | FS1 | ETX |
#define USB_CONNECTION_PACKET_EXEC_ID_SET_GYRO_DLPF_BW_SIZE                         8  // | STX | LEN1 | LEN2 | TYP | IDN | BW0 | BW1 | ETX |
#define USB_CONNECTION_PACKET_EXEC_ID_REQUEST_DEVICE_SETTINGS_SIZE                  6  // | STX | LEN1 | LEN2 | TYP | IDN | ETX |
#define USB_CONNECTION_PACKET_EXEC_ID_SET_LOGGING_LEVEL_SIZE                        7  // | STX | LEN1 | LEN2 | TYP | IDN | LEV | ETX |

typedef enum
{
    USB_CONNECTION_PACKET_EXEC_ID_ENABLE_INERTIAL_DATA_STREAM              = 0x01, // command GUI to DONGLE
    USB_CONNECTION_PACKET_EXEC_ID_SET_SAMPLE_RATE                          = 0x02, // command GUI to DONGLE
    USB_CONNECTION_PACKET_EXEC_ID_SET_ACC_FULL_SCALE                       = 0x03, // command GUI to DONGLE
    USB_CONNECTION_PACKET_EXEC_ID_SET_ACC_DLPF_BW                          = 0x04, // command GUI to DONGLE
    USB_CONNECTION_PACKET_EXEC_ID_SET_GYRO_FULL_SCALE                      = 0x05, // command GUI to DONGLE
    USB_CONNECTION_PACKET_EXEC_ID_SET_GYRO_DLPF_BW                         = 0x06, // command GUI to DONGLE
    USB_CONNECTION_PACKET_EXEC_ID_REQUEST_DEVICE_SETTINGS                  = 0x07, // command GUI to DONGLE
    USB_CONNECTION_PACKET_EXEC_ID_SET_LOGGING_LEVEL                        = 0x08, // command GUI to DONGLE

} usb_connection_exec_t;

#define USB_CONNECTION_PACKET_INFO_ID_DEVICE_CONNECTED_SIZE                           13  // | STX | LEN1 | LEN2 | TYP | IDN | MAC0 | MAC1 | MAC2 | MAC3 | MAC4 | MAC5 | STAT | ETX |
#define USB_CONNECTION_PACKET_INFO_ID_DEVICE_SETTINGS_SIZE                            117  // | STX | LEN1 | LEN2 | TYP | IDN | MAC0 | MAC1 | MAC2 | MAC3 | MAC4 | MAC5 | ... | ETX |

typedef enum
{
    USB_CONNECTION_PACKET_INFO_ID_DEVICE_CONNECTED                          = 0x01, // info DONGLE to GUI
    USB_CONNECTION_PACKET_INFO_ID_DEVICE_SETTINGS                           = 0x02, // info DONGLE to GUI
    USB_CONNECTION_PACKET_INFO_ID_DEVICE_INERTIAL_DATA                      = 0x03, // info DONGLE to GUI
    USB_CONNECTION_PACKET_INFO_ID_STRING                                    = 0x04, // info DONGLE to GUI

} usb_connection_info_t;


/*******************************************************************************
 *  Interface Definitions
 ******************************************************************************/

class SerialConnection : public QWidget
{
    Q_OBJECT
public:
    explicit SerialConnection(QWidget *parent = nullptr);
    ~SerialConnection();

signals:
    void SerialUpdateConnection( bool isConnected );
    void ReceiveAck();
    void ReceiveNack();
    void ReceivePacketInfoString( QByteArray string );
    void ReceivePacketInfoDeviceInertialData( QByteArray data );
    void ReceivePacketInfoDeviceSettings( QByteArray deviceSettings );
    void ReceivePacketInfoDeviceConnect( QByteArray deviceConnect );

public slots:
    void SendAck();
    void SendNack();
    void SendPacket( QByteArray packet );
    void ReadData();
    void HandleError( QSerialPort::SerialPortError error );
    void ConnectDevice();
    void DisconnectDevice();

private:
    QSerialPort *serial = nullptr;
    bool deviceFound;

};

#endif // SERIALCONNECTION_H
