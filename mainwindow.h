#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QDebug>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>

#include "qtablemodel_ign_angle_mg_by_cycle.h"

extern "C" {
    #include "simple_protocol.h"
}

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    static uint16_t read_count;

    Ui::MainWindow *ui;

    QSerialPort *serial;

    simple_protocol_link_layer_t ecu_master;

    QTableModel_ign_angle_mg_by_cycle* ign_angle_mg_by_cycle_model;

    float ball_x = 0;
    float ball_y = 0;

    volatile uint32_t GPIOD_ODR;
    volatile void *ecu_addr_ptrs[2];

    static void ecu_protocol_usart_read(QSerialPort* serial,uint8_t* data,uint16_t count) {
        serial->read(reinterpret_cast<char*>(data),count);
    }

    static void ecu_protocol_usart_write(QSerialPort* serial, uint8_t* data, uint16_t count) {
        serial->write(reinterpret_cast<char*>(data),count);
        serial->flush();
    }

    static void ecu_protocol_net_data_get(void*,simple_protocol_link_layer_t* protocol) {
        simple_protocol_data_head_t read;
        memcpy(&read,protocol->read.frame.data,SIMPLE_PROTOCOL_NET_DATA_HEAD_COUNT);
        if(read.start+read.count < (16*16*4)) {
            simple_protocol_data_read(protocol,protocol->read.frame.head.addr,read.addr,read.start+read.count,read.count);
        } else {
            qDebug() << "Count" << protocol->read_count++;
            simple_protocol_data_read(protocol,protocol->read.frame.head.addr,read.addr,0,read.count);
        }
    }

    static void ecu_protocol_net_data_write(void*,simple_protocol_link_layer_t* protocol) {
        simple_protocol_data_head_t write;
        memcpy(&write,protocol->read.frame.data,SIMPLE_PROTOCOL_NET_DATA_HEAD_COUNT);
        if(write.start+write.count < (16*16*4)) {
            simple_protocol_data_write(protocol,protocol->read.frame.head.addr,write.addr,write.start+write.count,write.count);
        } else {
            qDebug() << "Всё записано";
        }
    }

    static void ecu_protocol_link_crc_err(void*,simple_protocol_link_layer_t* protocol) {
        qDebug() << "Ошибка контрольной суммы";
        qDebug() << "Addr" << protocol->read.frame.head.addr;
        qDebug() << "ID" << protocol->read.frame.head.id;
        qDebug() << "Count" << protocol->read.frame.head.count;
        simple_protocol_service_init(protocol);
    }

    void ecu_online_read();

private slots:
    void on_pushButton_Connect_toggled(bool state);
    void on_pushButton_read_clicked();
    void on_pushButton_write_clicked();
    void serial_readyRead();
};

#endif // MAINWINDOW_H
