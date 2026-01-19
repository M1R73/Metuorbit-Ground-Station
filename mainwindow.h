#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QMessageBox>
#include <QVariant>
#include <QtCore>
#include <QtGui>
#include <QtQuick>
#include <QFile>
#include <QVector>
#include <QTextStream>
#include <QThread>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QCamera>
#include <QCameraDevice>
#include <QMediaCaptureSession>
#include <QVideoWidget>
#include <QMediaDevices>
#include <QMediaRecorder>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:

    void on_btnOpenPort_clicked();

    void on_btnRefresh_clicked();

    void on_btnReadLive_clicked();

    bool didPortsChange();

    bool isPortAvailable(const QString &portName);

    void loadPorts();

    void startDataCommunication();

    void createLogFile();

    void readData();

    void updateFrame();

    void recalibrateGraphs();

    void on_ports_clicked();

    void on_manuelbtn_clicked();

    void ARAS(const QString& errorCode);

    void updateSourceCamComboBox();


    //void updateSatelliteOrientation(double pitch, double roll, double yaw);

    void on_btnStart_clicked();



private:
    Ui::MainWindow *ui;
    QSerialPort* COMPORT;
    QString currentCSVFile;
    QTimer *csvTimer;
    QStringList allLines;
    QFile csvFile;
    int currentLineIndex = 0;
    QFile logFile;
    QTextStream logStream;
    int receivedPkg;
    QString serialData;
    QString Line;
    QSerialPort *camSerialPort = nullptr;
    QFile *videoFile = nullptr;
    QMediaPlayer *mediaPlayer = nullptr;
    QVideoWidget *videoWidget = nullptr;
    QByteArray camImageBuffer;
    QCamera *camera = nullptr;
    QMediaCaptureSession *captureSession = nullptr;
    QMediaRecorder *mediaRecorder = nullptr;



signals:
    void setCenterPosition(QVariant, QVariant);
    void setLocationMarking(QVariant, QVariant);
    void clearMarkers();

private:
    QVector<double> x,yAlt1,yAlt2,yAltDiff;
    QVector<double> yTemp;
    QVector<double> yVoltage;
    QVector<double> yDescent;
    QVector<double> yPressure1, yPressure2;
    QList<QSerialPortInfo> lastAvailablePorts;
    QSerialPort *_serialPort;

};

#endif // MAINWINDOW_H

