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
            ecu_link_layer_service_init(&ecu_master);

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
}

void MainWindow::on_pushButton_14_clicked() {
    //ecu_write_frame_data(&ecu_master,ecu_addr_ptrs,(ECU_CMD_READ | ECU_DATA_TYPE_32),1,0,64);
}

void MainWindow::on_pushButton_13_clicked() {
    ecu_presentation_layer_rw_t write;
    write.addr = 1;
    write.start = 0;
    write.count = 4;
    float angle = 0.5;
    uint8_t data[5+4];
    memcpy(data,&write,5);
    memcpy(&data[5],&angle,4);
    ecu_link_layer_send_frame(&ecu_master,4,ECU_SESSION_LAYER_ID_READ,9,data);
}

void MainWindow::on_pushButton_12_clicked() {
    uint16_t count = 2;
    uint16_t point = 0;
    while(--count) {
        qDebug() << "Angle" << point << ":" << ign_angle_mg_by_cycle[point];
        point++;
    }
}

void MainWindow::serial_readyRead() {
    ecu_link_layer_handler(&ecu_master,(uint8_t)serial->bytesAvailable(),ecu_addr_ptrs);
}
