#ifndef LOGSWINDOW_H
#define LOGSWINDOW_H

#include <QDialog>

namespace Ui
{
class LogsWindow;
}


struct Level
{
    Level( QString name, int value ):
        name( name ),
        value( value )
    {

    }

    QString name;
    int value;
};


class LogsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LogsWindow( QWidget *parent = nullptr );
    ~LogsWindow();

signals:
    void SendPacket( QByteArray packet );
    void WindowClosed();

public slots:
    void ReceiveString( QByteArray packet );
    void CloseLogsWindow();
    void ClearIncomingData();
    void ComboBoxChanged();
    void UpdateConnection( bool isConnected );

private:
    Ui::LogsWindow *ui;
    QList<Level> levelsList;

    void FillDebugLevelsComboBox();

public:
    void ClearText();

};

#endif // LOGSWINDOW_H
