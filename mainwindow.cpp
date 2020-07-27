#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    serial(new QSerialPort)
{
    ui->setupUi(this);

    QTableModel_ign_angle_mg_by_cycle* ign_angle_mg_by_cycle_model = new QTableModel_ign_angle_mg_by_cycle(this);
    ui->tableView_value->setModel(ign_angle_mg_by_cycle_model);

    ecu_addr_ptrs[0] = &GPIOD_ODR;
    ecu_addr_ptrs[1] = ign_angle_mg_by_cycle_model->table;

    connect(serial, &QSerialPort::readyRead, this, &MainWindow::serial_readyRead);

    ecu_master.service.addr = 0;
    ecu_master.addr_ptrs = ecu_addr_ptrs;

    ecu_master.read.device.port = serial;
    ecu_master.read.device.transfer = reinterpret_cast<void(*)(void*,uint8_t*,uint16_t)>(&ecu_protocol_usart_read);

    ecu_master.write.device.port = serial;
    ecu_master.write.device.transfer = reinterpret_cast<void(*)(void*,uint8_t*,uint16_t)>(&ecu_protocol_usart_write);

    ecu_master.data_read.user_pointer = NULL;
    ecu_master.data_read.callback = reinterpret_cast<void(*)(void*,void*)>(&ecu_protocol_net_data_get);

    ecu_master.data_write.user_pointer = NULL;
    ecu_master.data_write.callback = reinterpret_cast<void(*)(void*,void*)>(&ecu_protocol_net_data_written);

    ecu_master.crc_err.user_pointer = NULL;
    ecu_master.crc_err.callback = reinterpret_cast<void(*)(void*,void*)>(&ecu_protocol_link_crc_err);

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

void MainWindow::on_pushButton_Connect_toggled(bool state) {
    if(state) {
        serial->setPortName(ui->comboBox_availablePorts->currentText());

        if(serial->open(QIODevice::ReadWrite)) {
            QString baudrate = ui->comboBox_standardBaudRates->currentText();
            if(!serial->setBaudRate(baudrate.toInt(),QSerialPort::AllDirections)) {
                qDebug()<<serial->errorString();
            }
            if(!serial->setDataBits(QSerialPort::Data8)) {
                qDebug()<<serial->errorString();
            }
            if(!serial->setFlowControl(QSerialPort::FlowControl::NoFlowControl)) {
                qDebug()<<serial->errorString();
            }
            QSerialPort::Parity parity = static_cast<QSerialPort::Parity>(ui->comboBox_Parity->currentData().toInt());
            if(!serial->setParity(parity)) {
                qDebug()<<serial->errorString();
            }
            QSerialPort::StopBits stop = static_cast<QSerialPort::StopBits>(ui->comboBox_StopBits->currentData().toInt());
            if(!serial->setStopBits(stop)) {
                qDebug()<<serial->errorString();
            }

            ui->pushButton_Connect->setText("Close");
            statusBar()->showMessage(QString("Порт %1 открыт").arg(serial->portName()), 5000);
            simple_protocol_service_init(&ecu_master);

        } else {
            qDebug() << "Serial not opened. Error:"<<serial->errorString();
        }
    } else {
        serial->close();
        ui->pushButton_Connect->setText("Open");

        statusBar()->showMessage(QString("Порт %1 закрыт").arg(serial->portName()), 5000);
    }
}

void MainWindow::on_pushButton_read_clicked() {
    qDebug() << "Прочитать";
    simple_protocol_data_read(&ecu_master,1,1,0,64);
}

void MainWindow::on_pushButton_write_clicked() {
    qDebug() << "Записать";
}

void MainWindow::serial_readyRead() {
    simple_protocol_handler(&ecu_master,serial->bytesAvailable());
    if(serial->bytesAvailable() != 0){
        QTimer::singleShot(0, this, &MainWindow::serial_readyRead);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
