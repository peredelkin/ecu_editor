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

void MainWindow::on_pushButton_send_clicked() {
    ecu_write.cmd_addr.cmd = 1;
    ecu_write.cmd_addr.addr = 2;
    ecu_write.service_data.start = 3;
    ecu_write.service_data.count = 4;
    *reinterpret_cast<uint32_t*>(&ecu_write.data[0]) = 5;
    *reinterpret_cast<uint16_t*>(&ecu_write.data[4]) = crc16_ccitt(reinterpret_cast<uint8_t*>(&ecu_write), 10);
    serial->write(reinterpret_cast<char*>(&ecu_write),12);
}

void MainWindow::serial_readyRead() {
    if(ecu_read_count_end != ecu_read_count) {
        if(serial->bytesAvailable() >= (ecu_read_count_end - ecu_read_count)) {
            serial->read(&(reinterpret_cast<char*>(&ecu_read)[ecu_read_count]),(ecu_read_count_end - ecu_read_count));
            ecu_read_count = ecu_read_count_end;
            if(ecu_read_count == (ECU_CMD_ADDR_COUNT + ECU_SERVICE_DATA_COUNT)) {
                ecu_read_count_end += ecu_read.service_data.count + ECU_CRC_COUNT;
            }
            if(ecu_read_count == ((ECU_CMD_ADDR_COUNT + ECU_SERVICE_DATA_COUNT + ECU_CRC_COUNT) + ecu_read.service_data.count)) {
                uint16_t crc_calc = crc16_ccitt(reinterpret_cast<uint8_t*>(&ecu_read),ecu_read_count_end - ECU_CRC_COUNT);
                uint16_t crc_read = *reinterpret_cast<uint16_t*>(&ecu_read.data[ecu_read.service_data.count]);
                if(crc_calc == crc_read) {
                    qDebug() << "CRC Correct:" << crc_read;
                } else {
                    qDebug() << "CRC Incorrect:" << crc_calc << crc_read;
                }
            }
        }
    } else {
        ecu_read_count = 0;
        ecu_read_count_end = ECU_CMD_ADDR_COUNT + ECU_SERVICE_DATA_COUNT;
    }
}
