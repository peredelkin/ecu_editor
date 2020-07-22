#include "mainwindow.h"
#include "ui_mainwindow.h"

extern "C" {
    #include <string.h>
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    return a.exec();
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
    simple_protocol_cmd_read(&ecu_master,4,1,0,4);
    qDebug() << "Read";
}

void MainWindow::on_pushButton_13_clicked() {
    simple_protocol_cmd_write(&ecu_master,4,1,0,4);
    qDebug() << "Write";
}

void MainWindow::on_pushButton_12_clicked() {
    uint16_t count = (16*16) + 1;
    uint16_t point = 0;
    while(--count) {
        qDebug() << "Show angle" << point << ":" << ign_angle_mg_by_cycle[point];
        point++;
    }
}

void MainWindow::serial_readyRead() {
    simple_protocol_handler(&ecu_master,(uint8_t)serial->bytesAvailable(),ecu_addr_ptrs);
}
