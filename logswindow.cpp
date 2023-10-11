
#include "logswindow.h"
#include "ui_logswindow.h"

LogsWindow::LogsWindow( QWidget *parent ) :
    QDialog( parent ),
    ui( new Ui::LogsWindow )
{
    ui->setupUi( this );
    setWindowIcon( QIcon( ":/icons/Icons/icon_logs") );
    setWindowTitle( "Logs" );

    FillDebugLevelsComboBox();

    connect( ui->closePushButton, &QPushButton::clicked, this, &LogsWindow::CloseLogsWindow );
    connect( ui->clearPushButton, &QPushButton::clicked, this, &LogsWindow::ClearIncomingData );
    connect( ui->levelComboBox, &QComboBox::currentTextChanged, this, &LogsWindow::ComboBoxChanged );
    connect( this, &LogsWindow::finished, this, &LogsWindow::CloseLogsWindow );
}


LogsWindow::~LogsWindow()
{
    delete ui;
}


void LogsWindow::ReceiveString( QByteArray packet )
{
    QString log = packet;
    ui->logsASCIITextEdit->insertPlainText( log );
    ui->logsASCIITextEdit->ensureCursorVisible();
}


void LogsWindow::CloseLogsWindow()
{
    hide();
    ui->logsASCIITextEdit->clear();
    emit WindowClosed();
    close();
}


void LogsWindow::ClearIncomingData()
{
    ui->logsASCIITextEdit->clear();
}


void LogsWindow::ComboBoxChanged()
{
    int levelValue = ui->levelComboBox->currentData().toInt();
    QByteArray cmd;
    QByteArray cmdSize;
    uint16_t cmdSizeTmp;

    cmd.append( ( char ) 0x02 );
    cmd.append( ( char ) 0x00 );
    cmd.append( ( char ) 0x00 );
    cmd.append( ( char ) 0x03 );
    cmd.append( ( char ) 0x01 );
    cmd.append( levelValue );
    cmd.append( ( char ) 0x03 );

    cmdSizeTmp = cmd.length();
    cmdSize.append( *( char * )&cmdSizeTmp );
    cmdSize.append( *( ( char * )&cmdSizeTmp + 1 ) );
    cmd.replace( 1, 2, cmdSize );

    qDebug() << cmd;
    emit SendPacket( cmd );
}


void LogsWindow::UpdateConnection( bool isConnected )
{
    if ( isConnected == false )
    {
        hide();
        ui->logsASCIITextEdit->clear();
        close();
    }
}


void LogsWindow::FillDebugLevelsComboBox()
{
    levelsList.append( Level( "No Logs", 0 ) );
    levelsList.append( Level( "Fatal",   1 ) );
    levelsList.append( Level( "Error",   2 ) );
    levelsList.append( Level( "Warning", 3 ) );
    levelsList.append( Level( "Info",    4 ) );

    for ( int i = 0; i < levelsList.length(); i++ )
    {
        ui->levelComboBox->addItem( levelsList.at( i ).name, levelsList.at( i ).value );
    }
}
