#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    serial(new QSerialPort)
{
    ui->setupUi(this);

    connect(serial, &QSerialPort::readyRead, this, &MainWindow::serial_readyRead);

    QList<QSerialPortInfo> availablePorts = QSerialPortInfo::availablePorts();
    QList<QSerialPortInfo>::iterator availablePorts_count;
    for(availablePorts_count = availablePorts.begin(); availablePorts_count != availablePorts.end(); availablePorts_count++) {
        ui->comboBox_availablePorts->addItem(availablePorts_count->portName());
    }

    QList<qint32> standardBaudRates = QSerialPortInfo::standardBaudRates();
    QList<qint32>::iterator standardBaudRates_count;
    for(standardBaudRates_count = standardBaudRates.begin(); standardBaudRates_count != standardBaudRates.end(); standardBaudRates_count++) {
        ui->comboBox_standardBaudRates->addItem(QString::number(*standardBaudRates_count));
    }

    ui->comboBox_Parity->addItem("No Parity",QSerialPort::NoParity);
    ui->comboBox_Parity->addItem("Even Parity",QSerialPort::EvenParity);
    ui->comboBox_Parity->addItem("Odd Parity",QSerialPort::OddParity);
    ui->comboBox_Parity->addItem("Space Parity",QSerialPort::SpaceParity);
    ui->comboBox_Parity->addItem("Mark Parity",QSerialPort::MarkParity);

    ui->comboBox_StopBits->addItem("Stop Bits 1",QSerialPort::OneStop);
    ui->comboBox_StopBits->addItem("Stop Bits 1.5",QSerialPort::OneAndHalfStop);
    ui->comboBox_StopBits->addItem("Stop Bits 2",QSerialPort::TwoStop);

    ui->tableWidget->setRowCount(16);
    ui->tableWidget->setColumnCount(16);
    QStringList tableWidget_VerticalHeaderLabels = {"29","57","85","113","141","169","197","225","253","281","309","337","365","393","421","449"};
    ui->tableWidget->setVerticalHeaderLabels(tableWidget_VerticalHeaderLabels);
    QStringList tableWidget_HorizontalHeaderLabels = {"600","720","840","990","1170","1380","1560","1950","2310","2730","3210","3840","4530","5370","6360","7650"};
    ui->tableWidget->setHorizontalHeaderLabels(tableWidget_HorizontalHeaderLabels);
}

MainWindow::~MainWindow()
{
    delete ui;
}
