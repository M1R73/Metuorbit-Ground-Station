#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>
#include <QTransform>
#include <QImage>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QMediaCaptureSession>
#include <QCamera>
#include <QVideoWidget>




#define TEAMID 138
#define Rpkg 100
#define Lalt 0
#define Ralt 1000
#define Lpres 80000
#define Rpres 103000
#define Ltemp 273
#define Rtemp 323
#define Lvolt 0
#define Rvolt 6
#define Lmagnet -0.025
#define Rmagnet 0.025
#define Ltilt -20
#define Rtilt 20



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , COMPORT(nullptr)
{
    ui->setupUi(this);

    _serialPort = new QSerialPort(this);
    _serialPort->setPortName(ui->cmbPorts->currentText());

    csvTimer = new QTimer(this);
    connect(csvTimer, &QTimer::timeout, this, &MainWindow::updateFrame);




    QGridLayout *gridLayout = new QGridLayout(this);

    // Spacing ve margin ayarlarını yap
    gridLayout->setSpacing(0); // Satır ve sütunlar arasındaki boşluğu kaldır
    gridLayout->setContentsMargins(0, 0, 0, 0); // Kenar boşluklarını kaldır

    ui->pushButton_2->setStyleSheet("background-color: green; color: white; border: 0.5px solid black;");
    ui->pushButton_6->setStyleSheet("background-color: green; color: white; border: 0.5px solid black;");
    ui->pushButton_7->setStyleSheet("background-color: green; color: white; border: 0.5px solid black;");
    ui->pushButton_8->setStyleSheet("background-color: green; color: white; border: 0.5px solid black;");
    ui->pushButton_9->setStyleSheet("background-color: red; color: white; border: 0.5px solid black;");
    ui->pushButton_10->setStyleSheet("background-color: green; color: white; border: 0.5px solid black;");

    ui->manuelbtn->setStyleSheet("background-color: red; color: white;");

    ui->btnStart->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->btnStart->setIconSize(QSize(32,32));   // ikon boyutunu ayarlar
    ui->btnStart->setText("");



    ui->sourceCam->setFixedSize(80, 32); // Buton boyutunu ayarla (örnek: 80x32)
    updateSourceCamComboBox();
    ui->sourceCam->setStyleSheet(R"(
    QComboBox {
        background-color: #E0E0E0; /* Record butonunun arka plan rengi */
        color: #333333;           /* Yazı rengi */
        border: 1px solid #B0B0B0; /* Kenarlık rengi */
        border-radius: 4px;       /* Kenar yuvarlaklığı */
        font-size: 14px;          /* Yazı boyutu */
        padding: 2px 10px;        /* İç boşluk */
    }
    QComboBox:hover {
        background-color: #D0D0D0; /* Fareyle üzerine gelindiğinde arka plan rengi */
    }
    QComboBox::drop-down {
        border: none; /* Açılır menü kenarlığını kaldır */
    }
)");
    ui->sourceCam->setFixedWidth(150); // Genişliği sabit olarak ayarla

    ui->quickWidget_MapView->setSource(QUrl(QStringLiteral("qrc:/QmlMap.qml")));
    ui->quickWidget_MapView->show();

    ui->baudRate->addItems({"9600","19200","38400","57600","115200"});
    ui->baudRate->setCurrentText("9600"); // Varsayılan olarak "9600" değerini seç

    auto Obje = ui->quickWidget_MapView->rootObject();
    connect(this, SIGNAL(setCenterPosition(QVariant,QVariant)), Obje, SLOT(setCenterPosition(QVariant,QVariant)));
    connect(this, SIGNAL(setLocationMarking(QVariant,QVariant)), Obje, SLOT(setLocationMarking(QVariant,QVariant)));

    emit setCenterPosition(39.8997, 32.8606);
    emit setLocationMarking(39.8997, 32.8606);

    ui->btnOpenPort->setStyleSheet("background-color: red; color: white;");

    QPen red(QColor(255,0,0));
    QPen green(QColor(0,255,0));
    QPen blue(QColor(0,0,255));
    QPen magenta(QColor(255,0,255));
    QPen yellow(QColor(255,255,0));
    QPen cyan(QColor(0, 255, 255));


    // ...MainWindow constructor'unda, graph1 için...

    ui->graph1->addGraph(); // 0: yAlt1
    ui->graph1->addGraph(); // 1: yAlt2
    ui->graph1->addGraph(); // 2: yAltDiff

    ui->graph1->graph(0)->setLineStyle(QCPGraph::lsLine);
    ui->graph1->graph(0)->setName("Altitude1");
    ui->graph1->graph(0)->setPen(red);

    ui->graph1->graph(1)->setLineStyle(QCPGraph::lsLine);
    ui->graph1->graph(1)->setName("Altitude2");
    ui->graph1->graph(1)->setPen(green);

    ui->graph1->graph(2)->setLineStyle(QCPGraph::lsLine);
    ui->graph1->graph(2)->setName("AltitudeDiff");
    ui->graph1->graph(2)->setPen(blue);

    ui->graph1->xAxis->setLabel("Zaman(saniye)");
    ui->graph1->yAxis->setLabel("Yükseklik(m)");
    ui->graph1->xAxis->setRange(0, Rpkg);
    ui->graph1->yAxis->setRange(0, 1000);
    ui->graph1->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    ui->graph1->legend->setVisible(true);
    QFont legendFont1 = ui->graph1->legend->font();
    legendFont1.setPointSize(8);
    ui->graph1->legend->setFont(legendFont1);
    ui->graph1->legend->setIconSize(10, 10);


    ui->graph2->addGraph();
    ui->graph2->graph()->setLineStyle(QCPGraph::lsLine);
    ui->graph2->graph()->setName("Sıcaklık");
    ui->graph2->xAxis->setLabel("Zaman(saniye)");
    ui->graph2->yAxis->setLabel("Sıcaklık(°C)");
    ui->graph2->xAxis->setRange(0,Rpkg);
    ui->graph2->yAxis->setRange(Ltemp,Rtemp);
    ui->graph2->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->graph2->graph()->setPen(green);

    ui->graph2->legend->setVisible(true);
    QFont legendFont2 = ui->graph2->legend->font();
    legendFont2.setPointSize(8);
    ui->graph2->legend->setFont(legendFont2);
    ui->graph2->legend->setIconSize(10, 10);

    ui->graph3->addGraph();
    ui->graph3->graph()->setLineStyle(QCPGraph::lsLine);
    ui->graph3->graph()->setName("Pil Gerilimi");
    ui->graph3->xAxis->setLabel("Zaman(saniye)");
    ui->graph3->yAxis->setLabel("Gerilim(V)");
    ui->graph3->xAxis->setRange(0,Rpkg);
    ui->graph3->yAxis->setRange(Lvolt,Rvolt);
    ui->graph3->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->graph3->graph()->setPen(blue);

    ui->graph3->legend->setVisible(true);
    QFont legendFont3 = ui->graph3->legend->font();
    legendFont3.setPointSize(8);
    ui->graph3->legend->setFont(legendFont3);
    ui->graph3->legend->setIconSize(10, 10);


    ui->graph4->addGraph(); // 0: yPressure1
    ui->graph4->addGraph(); // 1: yPressure2

    ui->graph4->graph(0)->setLineStyle(QCPGraph::lsLine);
    ui->graph4->graph(0)->setName("Basınç1");
    ui->graph4->graph(0)->setPen(magenta);

    ui->graph4->graph(1)->setLineStyle(QCPGraph::lsLine);
    ui->graph4->graph(1)->setName("Basınç2");
    ui->graph4->graph(1)->setPen(yellow); // Farklı bir renk seçebilirsin

    ui->graph4->xAxis->setLabel("Zaman(saniye)");
    ui->graph4->yAxis->setLabel("Basınç(Pa)");
    ui->graph4->xAxis->setRange(0, Rpkg);
    ui->graph4->yAxis->setRange(Lpres, Rpres);
    ui->graph4->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    ui->graph4->legend->setVisible(true);
    QFont legendFont4 = ui->graph4->legend->font();
    legendFont4.setPointSize(8);
    ui->graph4->legend->setFont(legendFont4);
    ui->graph4->legend->setIconSize(10, 10);

    ui->graph5->addGraph();
    ui->graph5->graph()->setLineStyle(QCPGraph::lsLine);
    ui->graph5->graph()->setName("Sürat");
    ui->graph5->xAxis->setLabel("Zaman(saniye)");
    ui->graph5->yAxis->setLabel("Sürat(m/s)");
    ui->graph5->xAxis->setRange(0,100);
    ui->graph5->yAxis->setRange(0,100);
    ui->graph5->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->graph5->graph()->setPen(yellow);

    ui->graph5->legend->setVisible(true);
    QFont legendFont5 = ui->graph5->legend->font();
    legendFont5.setPointSize(8);
    ui->graph5->legend->setFont(legendFont5);
    ui->graph5->legend->setIconSize(10, 10);

    recalibrateGraphs();

    QPixmap pix(":/uydu_new.png"); // Resource içindeki uydu.png
    QPixmap scaledPix = pix.scaled(ui->label_3->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->label_3->setPixmap(scaledPix);


}

MainWindow::~MainWindow()
{

    delete ui;
}

bool MainWindow::didPortsChange(){
    if(lastAvailablePorts.size() != QSerialPortInfo::availablePorts().size()){
        return true;
    }
    for (int i = 0; i < QSerialPortInfo::availablePorts().length(); i++) {
        if (lastAvailablePorts[i].portName() != QSerialPortInfo::availablePorts()[i].portName()) {
            return true;
        }
    }
    return false;
}

bool MainWindow::isPortAvailable(const QString &portName) {
    foreach (auto &info, QSerialPortInfo::availablePorts()) {
        if (info.portName() == portName) {
            return true;
        }
    }
    return false;
}

void MainWindow::loadPorts()
{
    if(!didPortsChange()){
        return;
    }

    if(!isPortAvailable(_serialPort->portName()) && _serialPort->isOpen()){
        _serialPort->close();
        ui->btnOpenPort->setStyleSheet("background-color: red");
        ui->btnOpenPort->setText("Open Port");
        ui->cmbPorts->setDisabled(false);
        ui->baudRate->setDisabled(false);
    }
    ui->cmbPorts->clear();
    foreach(auto &port, QSerialPortInfo::availablePorts()){
        ui->cmbPorts->addItem(port.portName());
    }
    lastAvailablePorts = QSerialPortInfo::availablePorts();
}


void MainWindow::on_btnOpenPort_clicked()
{
    QString portName = ui->cmbPorts->currentText();

    if (portName.isEmpty()) {
        QMessageBox::warning(this, "Uyarı", "Bağlantı noktası seçilmedi.!");
        return;
    }

    if (_serialPort->isOpen()) {
        _serialPort->close();
        ui->btnOpenPort->setStyleSheet("background-color: red; color: white;");
    }

    QString baudRateStr = ui->baudRate->currentText();

    _serialPort->setPortName(portName);
    _serialPort->setBaudRate(baudRateStr.toInt());
    _serialPort->setDataBits(QSerialPort::Data8);
    _serialPort->setParity(QSerialPort::NoParity);
    _serialPort->setStopBits(QSerialPort::OneStop);
    _serialPort->setFlowControl(QSerialPort::NoFlowControl);

    if (_serialPort->open(QIODevice::ReadWrite)) {
        ui->btnOpenPort->setStyleSheet("background-color: green; color: white;");
        QMessageBox::information(this, "Bilgi", "Seri bağlantı noktasına bağlanıldı: " + portName);

        // Veri okuma sinyalini burada bağla
        connect(_serialPort, &QSerialPort::readyRead, this, &MainWindow::readData, Qt::UniqueConnection);

    } else {
        ui->btnOpenPort->setStyleSheet("background-color: red; color: white;");
        QMessageBox::warning(this, "Uyarı", "Bağlantı başarısız : " + _serialPort->errorString());
    }
    createLogFile();
}

void MainWindow::on_btnRefresh_clicked()
{
    if (_serialPort && _serialPort->isOpen()) {
        _serialPort->close();
        ui->btnOpenPort->setStyleSheet("background-color: red; color: white;"); // Kırmızıya dön
        QMessageBox::information(this, "Bilgi", "Port bağlantısı kesildi!");
    }

    ui->cmbPorts->clear();
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();

    for (const QSerialPortInfo &port : ports) {
        ui->cmbPorts->addItem(port.portName());
    }
}


void MainWindow::startDataCommunication()
{
    recalibrateGraphs(); // Adjust graphs based on new data
    connect(_serialPort, &QSerialPort::readyRead, this, &MainWindow::readData); // Start reading data from the port
}

void MainWindow::createLogFile()
{
    // Create a log file with the specified name
    QString fileName = QString("TMUY2025_K1_687324_TLM.csv");
    logFile.setFileName(fileName);

    // Remove existing file if present
    if (logFile.exists()) {
        logFile.remove();
    }

    // Open file for writing and write the header if successful
    if (logFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        logStream.setDevice(&logFile);
        logStream << "PAKET NUMARASI,UYDU STATÜSÜ,HATA KODU,GÖNDERME SAATİ,"
                  << "BASINÇ1,BASINÇ2,YÜKSEKLİK1,YÜKSEKLİK2,İRTİFA FARKI,İNİŞ HIZI,"
                  << "SICAKLIK,PİL GERİLİMİ,GPS1 LATITUDE,GPS1 LONGITUDE,GPS1 ALTITUDE,"
                  << "PITCH,ROLL,YAW,RHRH,IoT S1 DATA,IoT S2 DATA,TAKIM NO\n";

    } else {
        qDebug() << "Dosya Oluşturulamadı:" << logFile.errorString();
    }
}


void MainWindow::readData()
{
    if(_serialPort->isOpen() && _serialPort != nullptr){
        serialData += QString(_serialPort->readAll());

        // Satır satır ayır ve her satırı işle
        while (serialData.contains('\n')) {
            int idx = serialData.indexOf('\n');
            QString line = serialData.left(idx).trimmed();
            serialData = serialData.mid(idx + 1);

            if (!line.isEmpty()) {
                Line = line;
                receivedPkg++;
                updateFrame();
            }
        }
    }
    else {
        QMessageBox::critical(this, "Port Error", "Port is not opened");
        return;
    }
}






void MainWindow::updateFrame()
{
        QStringList List = Line.split(',');
        if (List.size() < 22) return;


        // --- Parse kısmı ---
        int pkg = List[3].toInt();
        x.append(List[0].toDouble());

        yAlt1.append(List[6].toDouble());
        yAlt2.append(List[7].toDouble());
        yAltDiff.append(List[8].toDouble());
        yTemp.append(List[10].toDouble());
        yVoltage.append(List[11].toDouble());
        yDescent.append(List[9].toDouble());
        yPressure1.append(List[4].toDouble());
        yPressure2.append(List[5].toDouble());


        // --- Grafik güncelleme için vektörlere ekleme (maksimum 500 veri tut) ---
        double alt1 = List[6].toDouble();
        double alt2 = List[7].toDouble();
        double altDiff = List[8].toDouble();
        ui->graph1->graph(0)->setData(x, yAlt1);
        ui->graph1->graph(1)->setData(x, yAlt2);
        ui->graph1->graph(2)->setData(x, yAltDiff);
        if(pkg > Rpkg){ui->graph1->xAxis->rescale();}
        if(alt1 > Ralt || alt1 < Lalt){ui->graph1->yAxis->rescale();}
        if(alt2 > Ralt || alt2 < Lalt){ui->graph1->yAxis->rescale();}
        if(altDiff > Ralt || altDiff < Lalt){ui->graph1->yAxis->rescale();}
        ui->graph1->replot();


        double temp = List[10].toDouble();
        ui->graph2->graph()->setData(x, yTemp);
        if(temp > Rtemp || temp < Ltemp){ui->graph2->yAxis->rescale();}
        if(pkg > Rpkg){ui->graph2->xAxis->rescale();}
        ui->graph2->replot();

        double volt = List[11].toDouble();
        ui->graph3->graph()->setData(x, yVoltage);
        if(volt > Rvolt || volt < Lvolt){ui->graph3->yAxis->rescale();}
        if(pkg > Rpkg){ui->graph3->xAxis->rescale();}
        ui->graph3->replot();

        double pressure1 = List[4].toDouble();
        double pressure2 = List[5].toDouble();
        ui->graph4->graph(0)->setData(x, yPressure1);
        ui->graph4->graph(1)->setData(x, yPressure2);
        if(pressure1 > Rpres || pressure1 < Lpres){ui->graph4->yAxis->rescale();}
        if(pressure2 > Rpres || pressure2 < Lpres){ui->graph4->yAxis->rescale();}
        if(pkg > Rpkg){ui->graph4->xAxis->rescale();}
        ui->graph4->replot();

        double decRate = List[9].toDouble();
        ui->graph5->graph()->setData(x, yDescent);
        if(decRate > Rvolt || decRate < Lvolt){ui->graph5->yAxis->rescale();}
        if(pkg > Rpkg){ui->graph5->xAxis->rescale();}
        ui->graph5->replot();



        // --- QLabel güncelleme ---
        ui->packetCount->setText("Paket Numarası: " + List[0]);
        ui->status     ->setText("Uydu Statüsü: " + List[1]);
        ui->errorCode  ->setText("Hata Kodu: "   + List[2]);
        ui->gpsTime    ->setText("Gönderme Saati: " + List[3]);
        ui->pressure1  ->setText("Basınç1: "     + List[4]);
        ui->pressure2  ->setText("Basınç2: "     + List[5]);
        ui->alt1       ->setText("Yükseklik1: "  + List[6]);
        ui->alt2       ->setText("Yükseklik2: "  + List[7]);
        ui->altDiff    ->setText("İrtifa Farkı: " + List[8]);
        ui->descentRate->setText("İniş Hızı: "   + List[9]);
        ui->temp       ->setText("Sıcaklık: "    + List[10]);
        ui->voltage    ->setText("Pil Gerilimi: "+ List[11]);
        ui->gpsLatitude   ->setText("GPS1 Latitude: "  + List[12]);
        ui->gpsLongtitude ->setText("GPS1 Longitude: " + List[13]);
        ui->gpsAltitude   ->setText("GPS1 Altitude: "  + List[14]);
        ui->pitch      ->setText("Pitch: "        + List[15]);
        ui->roll       ->setText("Roll: "         + List[16]);
        ui->yaw        ->setText("Yaw: "          + List[17]);
        ui->rhrh       ->setText("RHRH: "         + List[18]);
        ui->s1Temp     ->setText("IoT S1 Data: "  + List[19]);
        ui->s2Temp     ->setText("IoT S2 Data: "  + List[20]);
        ui->teamID     ->setText("Takım No: "     + List[21]);

        emit setCenterPosition(List[12].toDouble(), List[13].toDouble());
        emit clearMarkers();
        emit setLocationMarking(List[12].toDouble(), List[13].toDouble());

        QString errorCode = List[2].trimmed(); // HATA KODU alanı
        ARAS(errorCode);

        if (logFile.isOpen()) {
            logStream << Line << "\n";
            logStream.flush();
        }


        /*double pitch = List[15].toDouble();
        double roll  = List[16].toDouble();
        double yaw   = List[17].toDouble();

        updateSatelliteOrientation(pitch, roll, yaw);*/
}




void MainWindow::on_btnReadLive_clicked()
{


        if(!_serialPort->isOpen() || !isPortAvailable(_serialPort->portName())){
            QMessageBox::critical(this, "Port Error", "Bir port açmalısınız!");
            return;
        }

        recalibrateGraphs(); // Grafik ve verileri sıfırla

        QMessageBox::information(this, "Bilgi", "Canlı veri okuma başlatıldı. Gelen veriler otomatik olarak işlenecek.");
        // Canlı okuma için ek bir işleme gerek yok, veri geldikçe readData fonksiyonunuz zaten çalışacak.

}

void MainWindow::recalibrateGraphs(){

    emit clearMarkers();

    x.clear();
    yAlt1.clear();
    yAlt2.clear();
    yAltDiff.clear();
    yTemp.clear();
    yPressure1.clear();
    yPressure2.clear();
    yVoltage.clear();
    yDescent.clear();


    ui->graph1->graph(0)->setData(x, yAlt1);
    ui->graph1->graph(1)->setData(x, yAlt2);
    ui->graph1->graph(2)->setData(x, yAltDiff);
    ui->graph1->xAxis->setRange(0, Rpkg);
    ui->graph1->yAxis->setRange(Lalt, Ralt);
    ui->graph1->replot();

    ui->graph2->graph()->setData(x, yTemp);
    ui->graph2->xAxis->setRange(0, Rpkg);
    ui->graph2->yAxis->setRange(Ltemp, Rtemp);
    ui->graph2->replot();

    ui->graph3->graph()->setData(x, yVoltage);
    ui->graph3->xAxis->setRange(0, Rpkg);
    ui->graph3->yAxis->setRange(Lvolt, Rvolt);
    ui->graph3->replot();

    ui->graph4->graph(0)->setData(x, yPressure1);
    ui->graph4->graph(1)->setData(x, yPressure2);
    ui->graph4->xAxis->setRange(0, Rpkg);
    ui->graph4->yAxis->setRange(Lpres, Rpres);
    ui->graph4->replot();

    ui->graph5->graph()->setData(x, yDescent);
    ui->graph5->xAxis->setRange(0, Rpkg);
    ui->graph5->yAxis->setRange(Ltilt, Rtilt);
    ui->graph5->replot();
}



void MainWindow::on_ports_clicked()
{
    ui->releasePorts->clear();
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        ui->releasePorts->addItem(info.portName());
    }
}


void MainWindow::on_manuelbtn_clicked()
{
    QString selectedPort = ui->releasePorts->currentText();
    if (selectedPort.isEmpty()) {
        QMessageBox::warning(this, "Uyarı", "Lütfen bir port seçin!");
        return;
    }

    QSerialPort tempPort;
    tempPort.setPortName(selectedPort);
    tempPort.setBaudRate(115200); // Gerekirse uygun baudrate'i ayarla
    tempPort.setDataBits(QSerialPort::Data8);
    tempPort.setParity(QSerialPort::NoParity);
    tempPort.setStopBits(QSerialPort::OneStop);
    tempPort.setFlowControl(QSerialPort::NoFlowControl);

    if (tempPort.open(QIODevice::WriteOnly)) {
        QByteArray cmd = "AYRIL\r\n";
        tempPort.write(cmd);
        tempPort.flush();
        tempPort.waitForBytesWritten(100);
        tempPort.close();
        QMessageBox::information(this, "Bilgi", QString("'%1' portuna AYRIL komutu gönderildi.").arg(selectedPort));
    } else {
        QMessageBox::critical(this, "Hata", QString("Port açılamadı: %1").arg(selectedPort));
    }
}

void MainWindow::ARAS(const QString& errorCode)
{
    // Hata kodu 6 haneli olmalı, değilse başına sıfır ekle
    QString code = errorCode;
    while (code.length() < 6)
        code = "0" + code;

    // Butonları sırayla diziye al
    QPushButton* arasButtons[6] = {
        ui->pushButton_6,
        ui->pushButton_7,
        ui->pushButton_8,
        ui->pushButton_9,
        ui->pushButton_2,
        ui->pushButton_10
    };

    // Her haneye göre buton rengini ayarla
    for (int i = 0; i < 6; ++i) {
        if (code[i] == '1') {
            arasButtons[i]->setStyleSheet("background-color: red; color: white; border: 0.5px solid black;");
        } else {
            arasButtons[i]->setStyleSheet("background-color: green; color: white; border: 0.5px solid black;");
        }
    }
}


void MainWindow::updateSourceCamComboBox()
{
    ui->sourceCam->clear();
    const auto cameras = QMediaDevices::videoInputs();
    for (const QCameraDevice &device : cameras) {
        ui->sourceCam->addItem(device.description());
    }
}

/*void MainWindow::updateSatelliteOrientation(double pitch, double roll, double yaw)
{
    // Orijinal pixmap'i sakla (örneğin constructor'da bir değişkende)
    static QPixmap originalPix(":/uydu_new.png");

    // Sadece roll ve pitch ile 2D döndürme (örnek: roll ile yatay, pitch ile dikey döndürme)
    QTransform transform;
    transform.rotate(roll, Qt::ZAxis);   // Z ekseninde döndür (sağa-sola eğim)
    transform.rotate(pitch, Qt::XAxis);  // X ekseninde döndür (yukarı-aşağı eğim)
    // Yaw genellikle 2D görselde kullanılmaz, istersen ekleyebilirsin

    QPixmap rotatedPix = originalPix.transformed(transform, Qt::SmoothTransformation);

    // QLabel'a sığacak şekilde ölçekle
    QPixmap scaledPix = rotatedPix.scaled(ui->label_3->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    ui->label_3->setPixmap(scaledPix);
}*/



// Başlat/Durdur butonu mantığı
void MainWindow::on_btnStart_clicked()
{
    // Kamera açıksa kapat
    if (camera) {
        if (mediaRecorder) {
            mediaRecorder->stop();
            delete mediaRecorder;
            mediaRecorder = nullptr;
        }
        camera->stop();
        delete camera;
        camera = nullptr;
        if (captureSession) {
            delete captureSession;
            captureSession = nullptr;
        }
        if (videoWidget) {
            videoWidget->hide();
            delete videoWidget;
            videoWidget = nullptr;
        }
        ui->btnStart->setStyleSheet("background-color: red; color: white;");
        ui->btnStart->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        return;
    }

    // Kamera kapalıysa aç
    int camIndex = ui->sourceCam->currentIndex();
    if (camIndex < 0) {
        QMessageBox::warning(this, "Uyarı", "Kamera seçilmedi!");
        return;
    }

    const auto cameras = QMediaDevices::videoInputs();
    if (camIndex >= cameras.size()) {
        QMessageBox::warning(this, "Uyarı", "Kamera bulunamadı!");
        return;
    }

    camera = new QCamera(cameras[camIndex], this);
    captureSession = new QMediaCaptureSession(this);

    // Video kaydedici oluştur ve captureSession'a ekle
    mediaRecorder = new QMediaRecorder(this);
    captureSession->setRecorder(mediaRecorder);

    // QLabel'ın üstüne video widget ekle
    videoWidget = new QVideoWidget(ui->label_5);
    videoWidget->setGeometry(ui->label_5->rect());
    videoWidget->show();

    captureSession->setCamera(camera);
    captureSession->setVideoOutput(videoWidget);

    camera->start();

    // Video kaydını başlat (örnek dosya adı: "kayit.mp4")
    mediaRecorder->setOutputLocation(QUrl::fromLocalFile("kayit.mp4"));
    mediaRecorder->record();

    ui->btnStart->setStyleSheet("background-color: green; color: white;");
    ui->btnStart->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    QMessageBox::information(this, "Bilgi", "Kamera açıldı ve kayıt başladı!");
}
