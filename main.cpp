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
        serial0.setPortName(ui->comboBox_availablePorts->currentText());

        if(serial0.open(QIODevice::ReadWrite)) {
            QString baudrate = ui->comboBox_standardBaudRates->currentText();
            if(!serial0.setBaudRate(baudrate.toInt(),QSerialPort::AllDirections)) {
                qDebug()<<serial0.errorString();
            }
            if(!serial0.setDataBits(QSerialPort::Data8)) {
                qDebug()<<serial0.errorString();
            }
            if(!serial0.setFlowControl(QSerialPort::NoFlowControl)) {
                qDebug()<<serial0.errorString();
            }
            QSerialPort::Parity parity = static_cast<QSerialPort::Parity>(ui->comboBox_Parity->currentData().toInt());
            if(!serial0.setParity(parity)) {
                qDebug()<<serial0.errorString();
            }
            QSerialPort::StopBits stop = static_cast<QSerialPort::StopBits>(ui->comboBox_StopBits->currentData().toInt());
            if(!serial0.setStopBits(stop)) {
                qDebug()<<serial0.errorString();
            }

            ui->pushButton_Connect->setText("Connected");

            statusBar()->showMessage(QString("Порт %1 открыт").arg(serial0.portName()), 5000);

        } else {
            qDebug() << "Serial not opened. Error:"<<serial0.errorString();
        }
    } else {
        serial0.close();
        ui->pushButton_Connect->setText("Connect");

        statusBar()->showMessage(QString("Порт %1 закрыт").arg(serial0.portName()), 5000);
    }
}
