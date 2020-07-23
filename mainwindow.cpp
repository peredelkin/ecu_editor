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
    ecu_master.read.device.transfer = reinterpret_cast<void(*)(void*,uint8_t*,int16_t)>(&ecu_protocol_usart_read);

    ecu_master.write.device.port = serial;
    ecu_master.write.device.transfer = reinterpret_cast<void(*)(void*,uint8_t*,int16_t)>(&ecu_protocol_usart_write);

    ecu_master.id_handler.user_pointer = NULL;
    ecu_master.id_handler.callback = reinterpret_cast<void(*)(void*,void*)>(&ecu_protocol_id_handler);

    ecu_master.crc_err.user_pointer = NULL;
    ecu_master.crc_err.callback = reinterpret_cast<void(*)(void*,void*)>(&ecu_protocol_crc_err);

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
            if(!serial->setFlowControl(QSerialPort::FlowControl::SoftwareControl)) {
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

void MainWindow::on_pushButton_15_clicked() {
    uint16_t count = 256;
    uint16_t point = 0;
    while(--count) {
        ign_angle_mg_by_cycle[point] = static_cast<float>(0.1);
        point++;
    }
    qDebug() << "Fill";
}

void MainWindow::on_pushButton_14_clicked() {
    //simple_protocol_cmd_read(&ecu_master,4,1,0,4);
    ecu_master.write.frame.head.id = 4;
    ecu_master.write.frame.head.addr = 5;
    ecu_master.write.frame.head.count = SIMPLE_PROTOCOL_ID_RW_HEAD_COUNT;
    simple_protocol_id_rw_t read;
    read.addr = 7;
    read.start = 8;
    read.count = 0;
    memcpy(ecu_master.write.frame.data,&read,ecu_master.write.frame.head.count);

    uint16_t crc_calc = crc16_ccitt((uint8_t*)(&ecu_master.write.frame),SIMPLE_PROTOCOL_LINK_HEAD_COUNT + ecu_master.write.frame.head.count); //вычисление контрольной суммы
        memcpy(&ecu_master.write.frame.data[ecu_master.write.frame.head.count],&crc_calc,SIMPLE_PROTOCOL_LINK_CRC_COUNT);//копирование контрольной суммы в хвост фрейма

    ecu_master.write.device.transfer
            (ecu_master.write.device.port,
             (uint8_t*)(&ecu_master.write.frame),
             SIMPLE_PROTOCOL_LINK_HEAD_COUNT + ecu_master.write.frame.head.count + SIMPLE_PROTOCOL_LINK_CRC_COUNT);
    qDebug() << "Read";
}

void MainWindow::on_pushButton_13_clicked() {
    //simple_protocol_cmd_write(&ecu_master,4,1,0,4);
    qDebug() << "Write";
}

void MainWindow::on_pushButton_12_clicked() {
    qDebug() << "bytesAvailable:" << serial->bytesAvailable();
    qDebug() << "count_remain:" << ecu_master.service.count_remain;
}

void MainWindow::serial_readyRead() {
    simple_protocol_handler(&ecu_master,serial->bytesAvailable(),ecu_addr_ptrs);
}

MainWindow::~MainWindow()
{
    delete ui;
}
