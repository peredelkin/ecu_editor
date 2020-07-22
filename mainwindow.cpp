#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    serial(new QSerialPort)
{
    ui->setupUi(this);

    connect(serial, &QSerialPort::readyRead, this, &MainWindow::serial_readyRead);

    ecu_master.service.addr = 0;

    ecu_master.read.device.port = serial;
    ecu_master.read.device.transfer = reinterpret_cast<void(*)(void*,uint8_t*,uint16_t)>(&ecu_protocol_usart_read);

    ecu_master.write.device.port = serial;
    ecu_master.write.device.transfer = reinterpret_cast<void(*)(void*,uint8_t*,uint16_t)>(&ecu_protocol_usart_write);

    ecu_master.ack_received.user_pointer = NULL;
    ecu_master.ack_received.callback = reinterpret_cast<void(*)(void*,void*)>(&ecu_protocol_ack);

    ecu_master.crc_err.user_pointer = NULL;
    ecu_master.crc_err.callback = reinterpret_cast<void(*)(void*,void*)>(&ecu_protocol_crc_err);

    ecu_master.data_received.user_pointer = NULL;
    ecu_master.data_received.callback = reinterpret_cast<void(*)(void*,void*)>(&ecu_protocol_data_received);

    ecu_master.data_transmitted.user_pointer = NULL;
    ecu_master.data_transmitted.callback = reinterpret_cast<void(*)(void*,void*)>(&ecu_protocol_data_transmitted);

    ecu_master.wrong_id.user_pointer = NULL;
    ecu_master.wrong_id.callback = reinterpret_cast<void(*)(void*,void*)>(&ecu_protocol_wrong_id);

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
}

MainWindow::~MainWindow()
{
    delete ui;
}
