#include "mainwindow.h"
#include "ui_mainwindow.h"

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
            master_protocol.init();
            statusBar()->showMessage(QString("Порт %1 открыт").arg(serial->portName()), 5000);

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
    master_protocol.write_frame_data(ecu_addr_ptrs,(ECU_CMD_READ |ECU_DATA_TYPE_32),1,0,64);
    master_protocol.send_frame(serial);
}

void MainWindow::on_pushButton_13_clicked() {
    master_protocol.write_frame_data(ecu_addr_ptrs,(ECU_CMD_WRITE |ECU_DATA_TYPE_32),1,0,64);
    master_protocol.send_frame(serial);
}

void MainWindow::on_pushButton_12_clicked() {
    qDebug() << "Array 0:" << ign_angle_mg_by_cycle[0];
    qDebug() << "Array 1:" << ign_angle_mg_by_cycle[1];
    qDebug() << "Array 2:" << ign_angle_mg_by_cycle[2];
    qDebug() << "Array 3:" << ign_angle_mg_by_cycle[3];
    qDebug() << "Array 4:" << ign_angle_mg_by_cycle[4];
    qDebug() << "Array 5:" << ign_angle_mg_by_cycle[5];
    qDebug() << "Array 6:" << ign_angle_mg_by_cycle[6];
    qDebug() << "Array 7:" << ign_angle_mg_by_cycle[7];
    qDebug() << "Array 8:" << ign_angle_mg_by_cycle[8];
    qDebug() << "Array 9:" << ign_angle_mg_by_cycle[9];
    qDebug() << "Array 10:" << ign_angle_mg_by_cycle[10];
    qDebug() << "Array 11:" << ign_angle_mg_by_cycle[11];
    qDebug() << "Array 12:" << ign_angle_mg_by_cycle[12];
    qDebug() << "Array 13:" << ign_angle_mg_by_cycle[13];
    qDebug() << "Array 14:" << ign_angle_mg_by_cycle[14];
    qDebug() << "Array 15:" << ign_angle_mg_by_cycle[15];
}

void MainWindow::serial_readyRead() {
    master_protocol.handler(serial,ecu_addr_ptrs);
}
