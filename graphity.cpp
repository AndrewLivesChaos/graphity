
#include "graphity.h"
#include "ui_graphity.h"


Graphity::Graphity( QWidget *parent )
    : QMainWindow( parent ),
    ui( new Ui::Graphity ),
    serialConnection( new SerialConnection ),
//    updateWindow( new UpdateWindow ),
    logsWindow( new LogsWindow ),
    status( new QLabel ),
    group( new QCPMarginGroup( ui->inertialPlot ) ),
    refreshPlotTimer( new QTimer( this ) )
{
    ui->setupUi( this );
    ui->statusbar->addWidget( status );
    ui->actionConnect->setEnabled( true );
    ui->actionDisconnect->setEnabled( false );
    ui->actionUpdate->setEnabled( true );
    ui->actionLogs->setEnabled( true );
    ui->actionSettings->setEnabled( false );
    setWindowIcon( QIcon( ":/icons/Icons/icon_apple") );
    setWindowTitle( "Gravity" );
    ShowStatusMessage( tr( "Disconnected" ) );
    deviceConnected = false;

    FillSampleRateComboBox();
    FillAccFullScaleComboBox();
    FillAccDlpfComboBox();
    FillGyroFullScaleComboBox();
    FillGyroDlpfComboBox();
    InitActionsConnections();
    InitPlot();
}


Graphity::~Graphity()
{
    if ( deviceConnected == true )
    {
        serialConnection->DisconnectDevice();
    }

    delete ui;
}


void Graphity::UpdateSerialConnection( bool isConnected )
{
    if ( isConnected )
    {
        deviceConnected = true;
        ui->actionConnect->setEnabled( false );
        ui->actionDisconnect->setEnabled( true );
        ShowStatusMessage( tr( "Connected to Gravity" ) );
    }
    else
    {
        deviceConnected = false;
        ui->actionConnect->setEnabled( true );
        ui->actionDisconnect->setEnabled( false );
        ui->actionLogs->setEnabled( true );
        ui->deviceConnectedValueLabel->setText( "NO DEVICE" );
        ShowStatusMessage( tr( "Disconnected" ) );
    }
}


void Graphity::FillInertialPool( QByteArray packet )
{
    QByteArray sampleValues;
    uint16_t packetSamplesNum = ( packet.size() - 6 ) / 20;

    sampleValues.append( packet.first( 6 ) );

    for ( int i = 0; i < packetSamplesNum; i++ )
    {
        sampleValues.replace( 6, 20, packet.sliced( 6 + ( i * 20 ), 20 ) );
        inertialDataQueue.enqueue( sampleValues );
    }
}


void Graphity::UpdatePlotData()
{
    QByteArray sampleValues;
    QByteArray macAddr;
    double temp;
    char *data = sampleValues.data();

    while ( !inertialDataQueue.isEmpty() )
    {
        sampleValues = inertialDataQueue.dequeue();
        sampleTime = samplePeriodChangedTime + ( sampleNum * samplePeriod );
        macAddr = sampleValues.first( 6 );

        accXPlot->addData( sampleTime, ( double )( *( int16_t * )( data + 6 + 0 ) ) * accScaleFactor );
        accYPlot->addData( sampleTime, ( double )( *( int16_t * )( data + 6 + 2 ) ) * accScaleFactor );
        accZPlot->addData( sampleTime, ( double )( *( int16_t * )( data + 6 + 4 ) ) * accScaleFactor );

        gyroXPlot->addData( sampleTime, ( double )( *( int16_t * )( data + 6 + 6 ) ) * gyroScaleFactor );
        gyroYPlot->addData( sampleTime, ( double )( *( int16_t * )( data + 6 + 8 ) ) * gyroScaleFactor );
        gyroZPlot->addData( sampleTime, ( double )( *( int16_t * )( data + 6 + 10 ) ) * gyroScaleFactor );

        temp = ( double )( *( int16_t * )( data + 6 + 12 ) ) * ( 1 / 333.87 );

        magXPlot->addData( sampleTime, ( double )( *( int16_t * )( data + 6 + 14 ) ) * 0.15 );
        magYPlot->addData( sampleTime, ( double )( *( int16_t * )( data + 6 + 16 ) ) * 0.15 );
        magZPlot->addData( sampleTime, ( double )( *( int16_t * )( data + 6 + 18 ) ) * 0.15 );

        sampleNum++;
    }


//    QByteArray macAddr = packet.first( 6 );
//    uint16_t packetSamplesNum = ( packet.size() - 6 ) / 20;
//    double temp;
//    QVector<double> samplesTime;
//    QVector<double> accXSamples;
//    QVector<double> accYSamples;
//    QVector<double> accZSamples;
//    QVector<double> gyroXSamples;
//    QVector<double> gyroYSamples;
//    QVector<double> gyroZSamples;
//    QVector<double> magXSamples;
//    QVector<double> magYSamples;
//    QVector<double> magZSamples;

//    for ( int i = 0; i < packetSamplesNum; i++ )
//    {
//        sampleTime = samplePeriodChangedTime + ( sampleNum * samplePeriod );
//        samplesTime.append( sampleTime );
//        accXSamples.append( ( double )( *( int16_t * )( data + 6 + ( i * 20 ) + 0 ) ) * accScaleFactor );
//        accYSamples.append( ( double )( *( int16_t * )( data + 6 + ( i * 20 ) + 2 ) ) * accScaleFactor );
//        accZSamples.append( ( double )( *( int16_t * )( data + 6 + ( i * 20 ) + 4 ) ) * accScaleFactor );
//        gyroXSamples.append( ( double )( *( int16_t * )( data + 6 + ( i * 20 ) + 6 ) ) * accScaleFactor );
//        gyroYSamples.append( ( double )( *( int16_t * )( data + 6 + ( i * 20 ) + 8 ) ) * accScaleFactor );
//        gyroZSamples.append( ( double )( *( int16_t * )( data + 6 + ( i * 20 ) + 10 ) ) * accScaleFactor );
//        temp = ( double )( *( int16_t * )( data + 6 + ( i * 20 ) + 12 ) ) * ( 1 / 333.87 );
//        magXSamples.append( ( double )( *( int16_t * )( data + 6 + ( i * 20 ) + 14 ) ) * accScaleFactor );
//        magYSamples.append( ( double )( *( int16_t * )( data + 6 + ( i * 20 ) + 16 ) ) * accScaleFactor );
//        magZSamples.append( ( double )( *( int16_t * )( data + 6 + ( i * 20 ) + 18 ) ) * accScaleFactor );
//        sampleNum++;
//    }

//    QByteArray macAddr = packet.first( 6 );
//    uint16_t packetSamplesNum = ( packet.size() - 6 ) / 20;
//    char *data = packet.data();
//    double temp;

//    for ( int i = 0; i < packetSamplesNum; i++ )
//    {
//        sampleTime = samplePeriodChangedTime + ( sampleNum * samplePeriod );

//        accXPlot->addData( sampleTime, ( double )( *( int16_t * )( data + 6 + ( i * 20 ) + 0 ) ) * accScaleFactor );
//        accYPlot->addData( sampleTime, ( double )( *( int16_t * )( data + 6 + ( i * 20 ) + 2 ) ) * accScaleFactor );
//        accZPlot->addData( sampleTime, ( double )( *( int16_t * )( data + 6 + ( i * 20 ) + 4 ) ) * accScaleFactor );

//        gyroXPlot->addData( sampleTime, ( double )( *( int16_t * )( data + 6 + ( i * 20 ) + 6 ) ) * gyroScaleFactor );
//        gyroYPlot->addData( sampleTime, ( double )( *( int16_t * )( data + 6 + ( i * 20 ) + 8 ) ) * gyroScaleFactor );
//        gyroZPlot->addData( sampleTime, ( double )( *( int16_t * )( data + 6 + ( i * 20 ) + 10 ) ) * gyroScaleFactor );

//        temp = ( double )( *( int16_t * )( data + 6 + ( i * 20 ) + 12 ) ) * ( 1 / 333.87 );

//        magXPlot->addData( sampleTime, ( double )( *( int16_t * )( data + 6 + ( i * 20 ) + 14 ) ) * 0.15 );
//        magYPlot->addData( sampleTime, ( double )( *( int16_t * )( data + 6 + ( i * 20 ) + 16 ) ) * 0.15 );
//        magZPlot->addData( sampleTime, ( double )( *( int16_t * )( data + 6 + ( i * 20 ) + 18 ) ) * 0.15 );

//        sampleNum++;
//    }

    if ( sampleTime > accAxisRect->axis( QCPAxis::atBottom )->range().upper )
    {
        accAxisRect->axis( QCPAxis::atBottom )->setRange( sampleTime, PLOT_DATA_WIDTH_S, Qt::AlignRight );
    }

    if ( sampleTime > gyroAxisRect->axis( QCPAxis::atBottom )->range().upper )
    {
        gyroAxisRect->axis( QCPAxis::atBottom )->setRange( sampleTime, PLOT_DATA_WIDTH_S, Qt::AlignRight );
    }

    if ( sampleTime > magAxisRect->axis( QCPAxis::atBottom )->range().upper )
    {
        magAxisRect->axis( QCPAxis::atBottom )->setRange( sampleTime, PLOT_DATA_WIDTH_S, Qt::AlignRight );
    }

    ui->accXValueLabel->setText( QString::number( accXPlot->dataMainValue( accXPlot->dataCount() - 1 ), 'f', 2) );
    ui->accYValueLabel->setText( QString::number( accYPlot->dataMainValue( accYPlot->dataCount() - 1 ), 'f', 2) );
    ui->accZValueLabel->setText( QString::number( accZPlot->dataMainValue( accZPlot->dataCount() - 1 ), 'f', 2) );
    ui->gyroXValueLabel->setText( QString::number( gyroXPlot->dataMainValue( gyroXPlot->dataCount() - 1 ), 'f', 2) );
    ui->gyroYValueLabel->setText( QString::number( gyroYPlot->dataMainValue( gyroYPlot->dataCount() - 1 ), 'f', 2) );
    ui->gyroZValueLabel->setText( QString::number( gyroZPlot->dataMainValue( gyroZPlot->dataCount() - 1 ), 'f', 2) );
    ui->magXValueLabel->setText( QString::number( magXPlot->dataMainValue( magXPlot->dataCount() - 1 ), 'f', 2) );
    ui->magYValueLabel->setText( QString::number( magYPlot->dataMainValue( magYPlot->dataCount() - 1 ), 'f', 2) );
    ui->magZValueLabel->setText( QString::number( magZPlot->dataMainValue( magZPlot->dataCount() - 1 ), 'f', 2) );
    ui->tempValueLabel->setText( QString::number( temp, 'f', 2) );

    ui->inertialPlot->replot();
}


void Graphity::UpdateDeviceSettings( QByteArray packet )
{
    QByteArray macAddr = packet.first( 6 );
    QByteArray accCalibration = packet.sliced( 6, 32 );
    QByteArray gyroCalibration = packet.sliced( 38, 32 );
    QByteArray magCalibration = packet.sliced( 70, 32 );
    uint16_t sampleRate = ( uint16_t )( ( ( ( uint16_t )packet.at( 103 ) & 0x00FF ) << 8 ) | ( ( uint16_t )packet.at( 102 ) & 0x00FF ) );
    uint16_t accFullScale = ( uint16_t )packet.at( 104 ) & 0x00FF;
    uint16_t accDlpfBw = ( uint16_t )( ( ( ( uint16_t )packet.at( 106 ) & 0x00FF ) << 8 ) | ( ( uint16_t )packet.at( 105 ) & 0x00FF ) );
    uint16_t gyroFullScale = ( uint16_t )( ( ( ( uint16_t )packet.at( 108 ) & 0x00FF ) << 8 ) | ( ( uint16_t )packet.at( 107 ) & 0x00FF ) );
    uint16_t gyroDlpfBw = ( uint16_t )( ( ( ( uint16_t )packet.at( 110 ) & 0x00FF ) << 8 ) | ( ( uint16_t )packet.at( 109 ) & 0x00FF ) );

    disconnect( ui->sampleRateComboBox, &QComboBox::currentIndexChanged, this, &Graphity::SampleRateComboBoxChanged );
    disconnect( ui->accFullScaleComboBox, &QComboBox::currentIndexChanged, this, &Graphity::AccFullScaleComboBoxChanged );
    disconnect( ui->accDlpfComboBox, &QComboBox::currentIndexChanged, this, &Graphity::AccDlpfComboBoxChanged );
    disconnect( ui->gyroFullScaleComboBox, &QComboBox::currentIndexChanged, this, &Graphity::GyroFullScaleComboBoxChanged );
    disconnect( ui->gyroDlpfComboBox, &QComboBox::currentIndexChanged, this, &Graphity::GyroDlpfComboBoxChanged );

    ui->sampleRateComboBox->setCurrentIndex( ui->sampleRateComboBox->findData( sampleRate ) );
    ui->accFullScaleComboBox->setCurrentIndex( ui->accFullScaleComboBox->findData( accFullScale ) );
    ui->accDlpfComboBox->setCurrentIndex( ui->accDlpfComboBox->findData( accDlpfBw ) );
    ui->gyroFullScaleComboBox->setCurrentIndex( ui->gyroFullScaleComboBox->findData( gyroFullScale ) );
    ui->gyroDlpfComboBox->setCurrentIndex( ui->gyroDlpfComboBox->findData( gyroDlpfBw ) );

    samplePeriod = 1 / ui->sampleRateComboBox->currentData().toDouble();
    accScaleFactor = 1 / ( ( double )0x8000 / ui->accFullScaleComboBox->currentData().toDouble() );
    gyroScaleFactor = 1 / ( ( double )0x8000 / ui->gyroFullScaleComboBox->currentData().toDouble() );

    connect( ui->sampleRateComboBox, &QComboBox::currentIndexChanged, this, &Graphity::SampleRateComboBoxChanged );
    connect( ui->accFullScaleComboBox, &QComboBox::currentIndexChanged, this, &Graphity::AccFullScaleComboBoxChanged );
    connect( ui->accDlpfComboBox, &QComboBox::currentIndexChanged, this, &Graphity::AccDlpfComboBoxChanged );
    connect( ui->gyroFullScaleComboBox, &QComboBox::currentIndexChanged, this, &Graphity::GyroFullScaleComboBoxChanged );
    connect( ui->gyroDlpfComboBox, &QComboBox::currentIndexChanged, this, &Graphity::GyroDlpfComboBoxChanged );
}


void Graphity::UpdateDeviceConnection( QByteArray packet )
{
    QByteArray macAddr = packet.first( 6 );
    uint8_t connected = ( uint8_t )packet.at( 6 );

    std::reverse( macAddr.begin(), macAddr.end() );

    if ( connected == 0 )
    {
        ui->deviceConnectedValueLabel->setText( "NO DEVICE" );
    }
    else
    {
        ui->deviceConnectedValueLabel->setText( macAddr.toHex( ':' ).toUpper() );
        QTimer::singleShot( 2000, this, &Graphity::RequestDeviceSettings );
    }
}


void Graphity::OpenLogsWindow()
{
    connect( logsWindow, &LogsWindow::SendPacket, serialConnection, &SerialConnection::SendPacket );
    connect( serialConnection, &SerialConnection::ReceivePacketInfoString, logsWindow, &LogsWindow::ReceiveString );
    connect( serialConnection, &SerialConnection::SerialUpdateConnection, logsWindow, &LogsWindow::UpdateConnection );
    connect( logsWindow, &LogsWindow::WindowClosed, this, &Graphity::CloseLogsWindow );
    ui->actionLogs->setEnabled( false );
    logsWindow->show();
    qDebug() << "Log open";
}


void Graphity::CloseLogsWindow()
{
    disconnect( logsWindow, &LogsWindow::SendPacket, serialConnection, &SerialConnection::SendPacket );
    disconnect( serialConnection, &SerialConnection::ReceivePacketInfoString, logsWindow, &LogsWindow::ReceiveString );
    disconnect( serialConnection, &SerialConnection::SerialUpdateConnection, logsWindow, &LogsWindow::UpdateConnection );
    disconnect( logsWindow, &LogsWindow::WindowClosed, this, &Graphity::CloseLogsWindow );
    ui->actionLogs->setEnabled( true );
    qDebug() << "Log close";
}


//void Graphity::OpenUpdateWindow()
//{
//    connect( updateWindow, &UpdateWindow::WindowClosed, this, &Graphity::CloseUpdateWindow );
//    ui->actionUpdate->setEnabled( false );
//    updateWindow->show();
//    qDebug() << "Update open";
//}


//void Graphity::CloseUpdateWindow()
//{
//    disconnect( updateWindow, &UpdateWindow::WindowClosed, this, &Graphity::CloseUpdateWindow );
//    ui->actionUpdate->setEnabled( true );
//    qDebug() << "Update close";
//}


void Graphity::ShowStatusMessage( const QString &message )
{
    status->setText( message );
}


void Graphity::FillSampleRateComboBox()
{
    sampleRateList.append( SampleRate( "50 Hz", 50 ) );
    sampleRateList.append( SampleRate( "100 Hz", 100 ) );
    sampleRateList.append( SampleRate( "200 Hz", 200 ) );
    sampleRateList.append( SampleRate( "500 Hz", 500 ) );
    sampleRateList.append( SampleRate( "1000 Hz", 1000 ) );

    for ( int i = 0; i < sampleRateList.length(); i++ )
    {
        ui->sampleRateComboBox->addItem( sampleRateList.at( i ).name, sampleRateList.at( i ).value );
    }

    ui->sampleRateComboBox->setCurrentText( "50 Hz" );
    samplePeriod = 1 / ui->sampleRateComboBox->currentData().toDouble();
    samplePeriodChangedTime = sampleTime;
}


void Graphity::SampleRateComboBoxChanged()
{
    uint16_t sampleRateValue = ( uint16_t )ui->sampleRateComboBox->currentData().toUInt();
    QByteArray cmd;
    QByteArray cmdSize;
    uint16_t cmdSizeTmp;

    samplePeriod = 1 / ui->sampleRateComboBox->currentData().toDouble();
    samplePeriodChangedTime = sampleTime;
    sampleNum = 0;

    cmd.append( ( char ) USB_CONNECTION_PACKET_STX );
    cmd.append( ( char ) USB_CONNECTION_PACKET_EXEC_ID_SET_SAMPLE_RATE_SIZE );
    cmd.append( ( char ) 0x00 );
    cmd.append( ( char ) USB_CONNECTION_PACKET_TYPE_EXEC );
    cmd.append( ( char ) USB_CONNECTION_PACKET_EXEC_ID_SET_SAMPLE_RATE );
    cmd.append( ( uint8_t )( ( sampleRateValue & 0x00FF ) ) );
    cmd.append( ( uint8_t )( ( ( sampleRateValue >> 8 ) & 0x00FF ) ) );
    cmd.append( ( char ) USB_CONNECTION_PACKET_ETX );

    cmdSizeTmp = cmd.length();
    cmdSize.append( *( char * )&cmdSizeTmp );
    cmdSize.append( *( ( char * )&cmdSizeTmp + 1 ) );
    cmd.replace( 1, 2, cmdSize );
    emit SendPacket( cmd );
}


void Graphity::FillAccFullScaleComboBox()
{
    accFullScaleList.append( AccFullScale( "2 g", 2 ) );
    accFullScaleList.append( AccFullScale( "4 g", 4 ) );
    accFullScaleList.append( AccFullScale( "8 g", 8 ) );
    accFullScaleList.append( AccFullScale( "16 g", 16 ) );

    for ( int i = 0; i < accFullScaleList.length(); i++ )
    {
        ui->accFullScaleComboBox->addItem( accFullScaleList.at( i ).name, accFullScaleList.at( i ).value );
    }

    ui->accFullScaleComboBox->setCurrentText( "2 g" );
}


void Graphity::AccFullScaleComboBoxChanged()
{
    uint8_t fullScaleValue = ( uint8_t )ui->accFullScaleComboBox->currentData().toUInt();
    QByteArray cmd;
    QByteArray cmdSize;
    uint16_t cmdSizeTmp;

    accScaleFactor = 1 / ( ( double )0x8000 / ( double )fullScaleValue );

    cmd.append( ( char ) USB_CONNECTION_PACKET_STX );
    cmd.append( ( char ) USB_CONNECTION_PACKET_EXEC_ID_SET_ACC_FULL_SCALE_SIZE );
    cmd.append( ( char ) 0x00 );
    cmd.append( ( char ) USB_CONNECTION_PACKET_TYPE_EXEC );
    cmd.append( ( char ) USB_CONNECTION_PACKET_EXEC_ID_SET_ACC_FULL_SCALE );
    cmd.append( fullScaleValue );
    cmd.append( ( char ) USB_CONNECTION_PACKET_ETX );

    cmdSizeTmp = cmd.length();
    cmdSize.append( *( char * )&cmdSizeTmp );
    cmdSize.append( *( ( char * )&cmdSizeTmp + 1 ) );
    cmd.replace( 1, 2, cmdSize );

    accAxisRect->axis( QCPAxis::atLeft )->setRange( -fullScaleValue, fullScaleValue );
    ui->inertialPlot->replot();
    emit SendPacket( cmd );
}


void Graphity::FillAccDlpfComboBox()
{
    accDlpfBwList.append( AccDlpfBw( "Disabled", 0xFFFF ) );
    accDlpfBwList.append( AccDlpfBw( "5 Hz", 5 ) );
    accDlpfBwList.append( AccDlpfBw( "11 Hz", 11 ) );
    accDlpfBwList.append( AccDlpfBw( "23 Hz", 23 ) );
    accDlpfBwList.append( AccDlpfBw( "50 Hz", 50 ) );
    accDlpfBwList.append( AccDlpfBw( "111 Hz", 111 ) );
    accDlpfBwList.append( AccDlpfBw( "246 Hz", 246 ) );
    accDlpfBwList.append( AccDlpfBw( "473 Hz", 473 ) );

    for ( int i = 0; i < accDlpfBwList.length(); i++ )
    {
        ui->accDlpfComboBox->addItem( accDlpfBwList.at( i ).name, accDlpfBwList.at( i ).value );
    }

    ui->accDlpfComboBox->setCurrentText( "Disabled" );
}


void Graphity::AccDlpfComboBoxChanged()
{
    uint16_t dlpfValue = ( uint16_t )ui->accDlpfComboBox->currentData().toUInt();
    QByteArray cmd;
    QByteArray cmdSize;
    uint16_t cmdSizeTmp;

    cmd.append( ( char ) USB_CONNECTION_PACKET_STX );
    cmd.append( ( char ) USB_CONNECTION_PACKET_EXEC_ID_SET_ACC_DLPF_BW_SIZE );
    cmd.append( ( char ) 0x00 );
    cmd.append( ( char ) USB_CONNECTION_PACKET_TYPE_EXEC );
    cmd.append( ( char ) USB_CONNECTION_PACKET_EXEC_ID_SET_ACC_DLPF_BW );
    cmd.append( ( uint8_t )( ( dlpfValue & 0x00FF ) ) );
    cmd.append( ( uint8_t )( ( ( dlpfValue >> 8 ) & 0x00FF ) ) );
    cmd.append( ( char ) USB_CONNECTION_PACKET_ETX );

    cmdSizeTmp = cmd.length();
    cmdSize.append( *( char * )&cmdSizeTmp );
    cmdSize.append( *( ( char * )&cmdSizeTmp + 1 ) );
    cmd.replace( 1, 2, cmdSize );
    emit SendPacket( cmd );
}


void Graphity::FillGyroFullScaleComboBox()
{
    gyroFullScaleList.append( GyroFullScale( "250 dps", 250 ) );
    gyroFullScaleList.append( GyroFullScale( "500 dps", 500 ) );
    gyroFullScaleList.append( GyroFullScale( "1000 dps", 1000 ) );
    gyroFullScaleList.append( GyroFullScale( "2000 dps", 2000 ) );

    for ( int i = 0; i < gyroFullScaleList.length(); i++ )
    {
        ui->gyroFullScaleComboBox->addItem( gyroFullScaleList.at( i ).name, gyroFullScaleList.at( i ).value );
    }

    ui->gyroFullScaleComboBox->setCurrentText( "250 dps" );
}


void Graphity::GyroFullScaleComboBoxChanged()
{
    uint16_t fullScaleValue = ( uint16_t )ui->gyroFullScaleComboBox->currentData().toUInt();
    QByteArray cmd;
    QByteArray cmdSize;
    uint16_t cmdSizeTmp;

    gyroScaleFactor = 1 / ( ( double )0x8000 / ( double )fullScaleValue );

    cmd.append( ( char ) USB_CONNECTION_PACKET_STX );
    cmd.append( ( char ) USB_CONNECTION_PACKET_EXEC_ID_SET_GYRO_FULL_SCALE_SIZE );
    cmd.append( ( char ) 0x00 );
    cmd.append( ( char ) USB_CONNECTION_PACKET_TYPE_EXEC );
    cmd.append( ( char ) USB_CONNECTION_PACKET_EXEC_ID_SET_GYRO_FULL_SCALE );
    cmd.append( ( uint8_t )( ( fullScaleValue & 0x00FF ) ) );
    cmd.append( ( uint8_t )( ( ( fullScaleValue >> 8 ) & 0x00FF ) ) );
    cmd.append( ( char ) USB_CONNECTION_PACKET_ETX );

    cmdSizeTmp = cmd.length();
    cmdSize.append( *( char * )&cmdSizeTmp );
    cmdSize.append( *( ( char * )&cmdSizeTmp + 1 ) );
    cmd.replace( 1, 2, cmdSize );

    gyroAxisRect->axis( QCPAxis::atLeft )->setRange( -fullScaleValue, fullScaleValue );
    ui->inertialPlot->replot();
    emit SendPacket( cmd );
}


void Graphity::FillGyroDlpfComboBox()
{
    gyroDlpfBwList.append( GyroDlpfBw( "Disabled", 0xFFFF ) );
    gyroDlpfBwList.append( GyroDlpfBw( "5 Hz", 5 ) );
    gyroDlpfBwList.append( GyroDlpfBw( "11 Hz", 11 ) );
    gyroDlpfBwList.append( GyroDlpfBw( "23 Hz", 23 ) );
    gyroDlpfBwList.append( GyroDlpfBw( "51 Hz", 51 ) );
    gyroDlpfBwList.append( GyroDlpfBw( "119 Hz", 119 ) );
    gyroDlpfBwList.append( GyroDlpfBw( "151 Hz", 151 ) );
    gyroDlpfBwList.append( GyroDlpfBw( "196 Hz", 196 ) );
    gyroDlpfBwList.append( GyroDlpfBw( "361 Hz", 361 ) );

    for ( int i = 0; i < gyroDlpfBwList.length(); i++ )
    {
        ui->gyroDlpfComboBox->addItem( gyroDlpfBwList.at( i ).name, gyroDlpfBwList.at( i ).value );
    }

    ui->gyroDlpfComboBox->setCurrentText( "Disabled" );
}


void Graphity::GyroDlpfComboBoxChanged()
{
    uint16_t dlpfValue = ( uint16_t )ui->gyroDlpfComboBox->currentData().toUInt();
    QByteArray cmd;
    QByteArray cmdSize;
    uint16_t cmdSizeTmp;

    cmd.append( ( char ) USB_CONNECTION_PACKET_STX );
    cmd.append( ( char ) USB_CONNECTION_PACKET_EXEC_ID_SET_GYRO_DLPF_BW_SIZE );
    cmd.append( ( char ) 0x00 );
    cmd.append( ( char ) USB_CONNECTION_PACKET_TYPE_EXEC );
    cmd.append( ( char ) USB_CONNECTION_PACKET_EXEC_ID_SET_GYRO_DLPF_BW );
    cmd.append( ( uint8_t )( ( dlpfValue & 0x00FF ) ) );
    cmd.append( ( uint8_t )( ( ( dlpfValue >> 8 ) & 0x00FF ) ) );
    cmd.append( ( char ) USB_CONNECTION_PACKET_ETX );

    cmdSizeTmp = cmd.length();
    cmdSize.append( *( char * )&cmdSizeTmp );
    cmdSize.append( *( ( char * )&cmdSizeTmp + 1 ) );
    cmd.replace( 1, 2, cmdSize );
    emit SendPacket( cmd );
}


void Graphity::StreamButtonClicked()
{
    QByteArray cmd;
    QByteArray cmdSize;
    uint16_t cmdSizeTmp;

    cmd.append( ( char ) USB_CONNECTION_PACKET_STX );
    cmd.append( ( char ) USB_CONNECTION_PACKET_EXEC_ID_ENABLE_INERTIAL_DATA_STREAM_SIZE );
    cmd.append( ( char ) 0x00 );
    cmd.append( ( char ) USB_CONNECTION_PACKET_TYPE_EXEC );
    cmd.append( ( char ) USB_CONNECTION_PACKET_EXEC_ID_ENABLE_INERTIAL_DATA_STREAM );

    if ( ui->streamButton->isChecked() == true )
    {
        ui->streamButton->setText( "STOP" );
        cmd.append( ( char ) 0x01 );
        ResetPlot();
        refreshPlotTimer->start( REFRESH_PLOT_PERIOD_MS );
        qDebug() << "start stream";
    }
    else
    {
        ui->streamButton->setText( "START" );
        cmd.append( ( char ) 0x00 );
        refreshPlotTimer->stop();
        qDebug() << "stop stream";
    }

    cmd.append( ( char ) USB_CONNECTION_PACKET_ETX );
    cmdSizeTmp = cmd.length();
    cmdSize.append( *( char * )&cmdSizeTmp );
    cmdSize.append( *( ( char * )&cmdSizeTmp + 1 ) );
    cmd.replace( 1, 2, cmdSize );
    emit SendPacket( cmd );
}


void Graphity::RequestDeviceSettings()
{
    QByteArray cmd;
    QByteArray cmdSize;
    uint16_t cmdSizeTmp;

    cmd.append( ( char ) USB_CONNECTION_PACKET_STX );
    cmd.append( ( char ) USB_CONNECTION_PACKET_EXEC_ID_REQUEST_DEVICE_SETTINGS_SIZE );
    cmd.append( ( char ) 0x00 );
    cmd.append( ( char ) USB_CONNECTION_PACKET_TYPE_EXEC );
    cmd.append( ( char ) USB_CONNECTION_PACKET_EXEC_ID_REQUEST_DEVICE_SETTINGS );
    cmd.append( ( char ) USB_CONNECTION_PACKET_ETX );

    cmdSizeTmp = cmd.length();
    cmdSize.append( *( char * )&cmdSizeTmp );
    cmdSize.append( *( ( char * )&cmdSizeTmp + 1 ) );
    cmd.replace( 1, 2, cmdSize );
    emit SendPacket( cmd );
}


void Graphity::InitPlot()
{
    QSharedPointer<QCPAxisTickerTime> timeTicker( new QCPAxisTickerTime );
    timeTicker->setTimeFormat( "%m:%s" );
    QColor xColor( 130, 0, 0, 255 );
    QColor yColor( 0, 0, 130, 255 );
    QColor zColor( 0, 130, 0, 255 );
    QColor xColorBrush( 130, 0, 0, 10 );
    QColor yColorBrush( 0, 0, 130, 10 );
    QColor zColorBrush( 0, 130, 0, 10 );

    ui->inertialPlot->plotLayout()->clear();
    ui->inertialPlot->setAutoAddPlottableToLegend( false );
    // Accelerometer plot
    ui->inertialPlot->plotLayout()->addElement( 0, 0, new QCPTextElement( ui->inertialPlot, "Accelerometer", QFont( "sans", 10, QFont::Bold ) ) );
    accAxisRect = new QCPAxisRect( ui->inertialPlot );
    ui->inertialPlot->plotLayout()->addElement( 1, 0, accAxisRect );
    ui->inertialPlot->axisRect( 0 )->setMarginGroup( QCP::msLeft | QCP::msRight, group );
    accAxisRect->axis( QCPAxis::atBottom )->setTicker( timeTicker );
    accAxisRect->axis( QCPAxis::atLeft )->setLabel( "g" );
    accAxisRect->axis( QCPAxis::atLeft )->setLabelFont( QFont( "sans", 10, QFont::StyleNormal ) );
    accAxisRect->axis( QCPAxis::atLeft )->setRange( -ui->accFullScaleComboBox->currentData().toInt(), ui->accFullScaleComboBox->currentData().toInt() );
    accAxisRect->axis( QCPAxis::atBottom )->setRange( 0, PLOT_DATA_WIDTH_S );
    accXPlot = ui->inertialPlot->addGraph( accAxisRect->axis( QCPAxis::atBottom ), accAxisRect->axis( QCPAxis::atLeft ) );
    accXPlot->setLineStyle( QCPGraph::lsLine );
    accXPlot->setPen( QPen( xColor, PLOT_LINE_WIDTH ) );
    accXPlot->setName( "x acc" );
    accYPlot = ui->inertialPlot->addGraph( accAxisRect->axis( QCPAxis::atBottom ), accAxisRect->axis( QCPAxis::atLeft ) );
    accYPlot->setLineStyle( QCPGraph::lsLine );
    accYPlot->setPen( QPen( yColor, PLOT_LINE_WIDTH ) );
    accYPlot->setName( "y acc" );
    accZPlot = ui->inertialPlot->addGraph( accAxisRect->axis( QCPAxis::atBottom ), accAxisRect->axis( QCPAxis::atLeft ) );
    accZPlot->setLineStyle( QCPGraph::lsLine );
    accZPlot->setPen( QPen( zColor, PLOT_LINE_WIDTH ) );
    accZPlot->setName( "z acc" );
    accLegend = new QCPLegend;
    accAxisRect->insetLayout()->addElement( accLegend, Qt::AlignTop | Qt::AlignLeft );
    //    accLegend->setLayer( "acc legend" );
    //    accLegend->setBorderPen( Qt::NoPen );
    accXPlot->addToLegend( accLegend );
    accYPlot->addToLegend( accLegend );
    accZPlot->addToLegend( accLegend );
    accXPlot->setBrush( QBrush( xColorBrush ) );
    accYPlot->setBrush( QBrush( yColorBrush ) );
    accZPlot->setBrush( QBrush( zColorBrush ) );

    // Gyroscope plot
    ui->inertialPlot->plotLayout()->addElement( 2, 0, new QCPTextElement( ui->inertialPlot, "Gyroscope", QFont( "sans", 10, QFont::Bold ) ) );
    gyroAxisRect = new QCPAxisRect( ui->inertialPlot );
    ui->inertialPlot->plotLayout()->addElement( 3, 0, gyroAxisRect );
    ui->inertialPlot->axisRect( 1 )->setMarginGroup( QCP::msLeft | QCP::msRight, group );
    gyroAxisRect->axis( QCPAxis::atBottom )->setTicker( timeTicker );
    gyroAxisRect->axis( QCPAxis::atLeft )->setLabel( "dps" );
    gyroAxisRect->axis( QCPAxis::atLeft )->setLabelFont( QFont( "sans", 10, QFont::StyleNormal ) );
    gyroAxisRect->axis( QCPAxis::atLeft )->setRange( -ui->gyroFullScaleComboBox->currentData().toInt(), ui->gyroFullScaleComboBox->currentData().toInt() );
    gyroAxisRect->axis( QCPAxis::atBottom )->setRange( 0, PLOT_DATA_WIDTH_S );
    gyroXPlot = ui->inertialPlot->addGraph( gyroAxisRect->axis( QCPAxis::atBottom ), gyroAxisRect->axis( QCPAxis::atLeft ) );
    gyroXPlot->setLineStyle( QCPGraph::lsLine );
    gyroXPlot->setPen( QPen( xColor, PLOT_LINE_WIDTH ) );
    gyroXPlot->setName( "x gyr" );
    gyroYPlot = ui->inertialPlot->addGraph( gyroAxisRect->axis( QCPAxis::atBottom ), gyroAxisRect->axis( QCPAxis::atLeft ) );
    gyroYPlot->setLineStyle( QCPGraph::lsLine );
    gyroYPlot->setPen( QPen( yColor, PLOT_LINE_WIDTH ) );
    gyroYPlot->setName( "y gyr" );
    gyroZPlot = ui->inertialPlot->addGraph( gyroAxisRect->axis( QCPAxis::atBottom ), gyroAxisRect->axis( QCPAxis::atLeft ) );
    gyroZPlot->setLineStyle( QCPGraph::lsLine );
    gyroZPlot->setPen( QPen( zColor, PLOT_LINE_WIDTH ) );
    gyroZPlot->setName( "z gyr" );
    gyroLegend = new QCPLegend;
    gyroAxisRect->insetLayout()->addElement( gyroLegend, Qt::AlignTop | Qt::AlignLeft );
    //    gyroLegend->setLayer( "gyro legend" );
    //    gyroLegend->setBorderPen( Qt::NoPen );
    gyroXPlot->addToLegend( gyroLegend );
    gyroYPlot->addToLegend( gyroLegend );
    gyroZPlot->addToLegend( gyroLegend );
    gyroXPlot->setBrush( QBrush( xColorBrush ) );
    gyroYPlot->setBrush( QBrush( yColorBrush ) );
    gyroZPlot->setBrush( QBrush( zColorBrush ) );

    // Magnetometer plot
    ui->inertialPlot->plotLayout()->addElement( 4, 0, new QCPTextElement( ui->inertialPlot, "Magnetometer", QFont( "sans", 10, QFont::Bold ) ) );
    magAxisRect = new QCPAxisRect( ui->inertialPlot );
    ui->inertialPlot->plotLayout()->addElement( 5, 0, magAxisRect );
    ui->inertialPlot->axisRect( 2 )->setMarginGroup( QCP::msLeft | QCP::msRight, group );
    magAxisRect->axis( QCPAxis::atLeft )->setRange( -4900, 4900 );
    magAxisRect->axis( QCPAxis::atBottom )->setTicker( timeTicker );
    magAxisRect->axis( QCPAxis::atLeft )->setLabel( "uT" );
    magAxisRect->axis( QCPAxis::atLeft )->setLabelFont( QFont( "sans", 10, QFont::StyleNormal ) );
    magAxisRect->axis( QCPAxis::atBottom )->setRange( 0, PLOT_DATA_WIDTH_S );
    magXPlot = ui->inertialPlot->addGraph( magAxisRect->axis( QCPAxis::atBottom ), magAxisRect->axis( QCPAxis::atLeft ) );
    magXPlot->setLineStyle( QCPGraph::lsLine );
    magXPlot->setPen( QPen( xColor, PLOT_LINE_WIDTH ) );
    magXPlot->setName( "x mag" );
    magYPlot = ui->inertialPlot->addGraph( magAxisRect->axis( QCPAxis::atBottom ), magAxisRect->axis( QCPAxis::atLeft ) );
    magYPlot->setLineStyle( QCPGraph::lsLine );
    magYPlot->setPen( QPen( yColor, PLOT_LINE_WIDTH ) );
    magYPlot->setName( "y mag" );
    magZPlot = ui->inertialPlot->addGraph( magAxisRect->axis( QCPAxis::atBottom ), magAxisRect->axis( QCPAxis::atLeft ) );
    magZPlot->setLineStyle( QCPGraph::lsLine );
    magZPlot->setPen( QPen( zColor, PLOT_LINE_WIDTH ) );
    magZPlot->setName( "z mag" );
    magLegend = new QCPLegend;
    magAxisRect->insetLayout()->addElement( magLegend, Qt::AlignTop | Qt::AlignLeft );
    //    magLegend->setLayer( "mag legend" );
    //    magLegend->setBorderPen( Qt::NoPen );
    //    magLegend->setBrush( QBrush( QColor( 255, 255, 255, 0 ), Qt::SolidPattern ) );
    magXPlot->addToLegend( magLegend );
    magYPlot->addToLegend( magLegend );
    magZPlot->addToLegend( magLegend );
    magXPlot->setBrush( QBrush( xColorBrush ) );
    magYPlot->setBrush( QBrush( yColorBrush ) );
    magZPlot->setBrush( QBrush( zColorBrush ) );
}


void Graphity::ResetPlot()
{
    sampleNum = 0;
    samplePeriodChangedTime = 0;
    accXPlot->data()->clear();
    accYPlot->data()->clear();
    accZPlot->data()->clear();
    accAxisRect->axis( QCPAxis::atBottom )->setRange( 0, PLOT_DATA_WIDTH_S );
    gyroXPlot->data()->clear();
    gyroYPlot->data()->clear();
    gyroZPlot->data()->clear();
    gyroAxisRect->axis( QCPAxis::atBottom )->setRange( 0, PLOT_DATA_WIDTH_S );
    magXPlot->data()->clear();
    magYPlot->data()->clear();
    magZPlot->data()->clear();
    magAxisRect->axis( QCPAxis::atBottom )->setRange( 0, PLOT_DATA_WIDTH_S );
}


void Graphity::InitActionsConnections()
{
    connect( serialConnection, &SerialConnection::SerialUpdateConnection, this, &Graphity::UpdateSerialConnection );
    connect( this, &Graphity::SendPacket, serialConnection, &SerialConnection::SendPacket );
    connect( ui->actionLogs, &QAction::triggered, this, &Graphity::OpenLogsWindow );
//    connect( ui->actionUpdate, &QAction::triggered, this, &Graphity::OpenUpdateWindow );
    connect( refreshPlotTimer, &QTimer::timeout, this, &Graphity::UpdatePlotData );
//    connect( serialConnection, &SerialConnection::ReceivePacketInfoDeviceInertialData, this, &Graphity::UpdatePlotData );
    connect( serialConnection, &SerialConnection::ReceivePacketInfoDeviceSettings, this, &Graphity::UpdateDeviceSettings );
    connect( serialConnection, &SerialConnection::ReceivePacketInfoDeviceConnect, this, &Graphity::UpdateDeviceConnection );
    connect( ui->actionQuit, &QAction::triggered, this, &Graphity::QuitApplication );
    connect( ui->actionConnect, &QAction::triggered, serialConnection, &SerialConnection::ConnectDevice );
    connect( ui->actionDisconnect, &QAction::triggered, serialConnection, &SerialConnection::DisconnectDevice );
    connect( ui->sampleRateComboBox, &QComboBox::currentIndexChanged, this, &Graphity::SampleRateComboBoxChanged );
    connect( ui->accFullScaleComboBox, &QComboBox::currentIndexChanged, this, &Graphity::AccFullScaleComboBoxChanged );
    connect( ui->accDlpfComboBox, &QComboBox::currentIndexChanged, this, &Graphity::AccDlpfComboBoxChanged );
    connect( ui->gyroFullScaleComboBox, &QComboBox::currentIndexChanged, this, &Graphity::GyroFullScaleComboBoxChanged );
    connect( ui->gyroDlpfComboBox, &QComboBox::currentIndexChanged, this, &Graphity::GyroDlpfComboBoxChanged );
    connect( ui->streamButton, &QPushButton::clicked, this, &Graphity::StreamButtonClicked );
    ui->streamButton->setCheckable( true );
}


void Graphity::QuitApplication()
{
    QMessageBox::StandardButton reply;

    reply = QMessageBox::question( this, "Confirm Exit", "Do you wanna quit the application?", QMessageBox::Yes | QMessageBox::No );

    if ( reply == QMessageBox::Yes )
    {
        if ( deviceConnected == true )
        {
            serialConnection->DisconnectDevice();
        }

        QApplication::quit();
    }
}


void Graphity::CloseEvent( QCloseEvent *event )
{
    qDebug() << "CloseEvent";
    QApplication::quit();
    event->ignore();
//    QMessageBox::StandardButton reply;
//    reply = QMessageBox::question( this, "Confirm Exit", "Do you wanna quit the application?", QMessageBox::Yes | QMessageBox::No );

//    if ( reply == QMessageBox::Yes )
//    {
//        event->accept();
//    }
//    else
//    {
//        event->ignore();
//    }
}

