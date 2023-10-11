
#ifndef GRAPHITY_H
#define GRAPHITY_H

#include "updatewindow.h"
#include "serialconnection.h"
#include "qcustomplot.h"
#include "logswindow.h"

#include <QMainWindow>
#include <QLabel>
#include <QCloseEvent>
#include <QMessageBox>
#include <QTime>


QT_BEGIN_NAMESPACE
namespace Ui { class Graphity; }
QT_END_NAMESPACE

#define REFRESH_PLOT_PERIOD_MS          15
#define PLOT_DATA_WIDTH_S               10
#define PLOT_LINE_WIDTH                 1

struct SampleRate
{
    SampleRate( QString name, uint16_t value ):
        name( name ),
        value( value )
    {

    }

    QString name;
    int value;
};

struct AccFullScale
{
    AccFullScale( QString name, uint8_t value ):
        name( name ),
        value( value )
    {

    }

    QString name;
    int value;
};

struct AccDlpfBw
{
    AccDlpfBw( QString name, uint16_t value ):
        name( name ),
        value( value )
    {

    }

    QString name;
    int value;
};

struct GyroFullScale
{
    GyroFullScale( QString name, uint16_t value ):
        name( name ),
        value( value )
    {

    }

    QString name;
    int value;
};

struct GyroDlpfBw
{
    GyroDlpfBw( QString name, uint16_t value ):
        name( name ),
        value( value )
    {

    }

    QString name;
    int value;
};

class Graphity : public QMainWindow
{
    Q_OBJECT

public:
    Graphity( QWidget *parent = nullptr );
    ~Graphity();

public slots:
    void QuitApplication();
    void UpdateSerialConnection( bool isConnected );
    void FillInertialPool( QByteArray packet );
    void UpdatePlotData();
    void UpdateDeviceSettings( QByteArray packet );
    void UpdateDeviceConnection( QByteArray packet );
    void OpenLogsWindow();    
    void CloseLogsWindow();
//    void OpenUpdateWindow();
//    void CloseUpdateWindow();

signals:
    void SendPacket( QByteArray packet );
    void SendAck();
    void SendNack();

private:
    Ui::Graphity *ui;
    SerialConnection *serialConnection = nullptr;
    LogsWindow *logsWindow = nullptr;
    UpdateWindow *updateWindow = nullptr;
    QLabel *status = nullptr;
    QList<SampleRate> sampleRateList;
    QList<AccFullScale> accFullScaleList;
    QList<AccDlpfBw> accDlpfBwList;
    QList<GyroFullScale> gyroFullScaleList;
    QList<GyroDlpfBw> gyroDlpfBwList;
    QCPGraph *accXPlot = nullptr;
    QCPGraph *accYPlot = nullptr;
    QCPGraph *accZPlot = nullptr;
    QCPGraph *gyroXPlot = nullptr;
    QCPGraph *gyroYPlot = nullptr;
    QCPGraph *gyroZPlot = nullptr;
    QCPGraph *magXPlot = nullptr;
    QCPGraph *magYPlot = nullptr;
    QCPGraph *magZPlot = nullptr;
    QCPAxisRect *accAxisRect = nullptr;
    QCPLegend *accLegend = nullptr;
    QCPAxisRect *gyroAxisRect = nullptr;
    QCPLegend *gyroLegend = nullptr;
    QCPAxisRect *magAxisRect = nullptr;
    QCPLegend *magLegend = nullptr;
    QCPMarginGroup *group = nullptr;
    QTimer *refreshPlotTimer = nullptr;
    QQueue<QByteArray> inertialDataQueue;
    bool deviceConnected;
    double sampleNum;
    double samplePeriod;
    double sampleTime;
    double samplePeriodChangedTime;
    double accScaleFactor;
    double gyroScaleFactor;

    void ShowStatusMessage( const QString &message );
    void FillSampleRateComboBox();
    void SampleRateComboBoxChanged();
    void FillAccFullScaleComboBox();
    void AccFullScaleComboBoxChanged();
    void FillAccDlpfComboBox();
    void AccDlpfComboBoxChanged();
    void FillGyroFullScaleComboBox();
    void GyroFullScaleComboBoxChanged();
    void FillGyroDlpfComboBox();
    void GyroDlpfComboBoxChanged();
    void StreamButtonClicked();
    void RequestDeviceSettings();
    void InitPlot();
    void ResetPlot();
    void InitActionsConnections();
    void CloseEvent( QCloseEvent *event );
};

#endif // GRAPHITY_H
