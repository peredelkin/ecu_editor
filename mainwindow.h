#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QDebug>
#include <QSerialPort>
#include <QSerialPortInfo>

extern "C" {
    #include "simple_protocol.h"
}

#include "ign_angle_mg_by_cycle.h"

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
    Ui::MainWindow *ui;

    QSerialPort *serial;

    simple_protocol_link_layer_t ecu_master;

    volatile float ign_angle_mg_by_cycle[IGN_ANGLE_MG_BY_CYCLE_MG_SCALE_N*IGN_ANGLE_MG_BY_CYCLE_RPM_SCALE_N];

    volatile void *ecu_addr_ptrs[2] = {
            NULL,
            ign_angle_mg_by_cycle
    };

    const uint8_t ecu_addr_ptrs_width[2] = {0,0};
    const uint8_t ecu_addr_ptrs_type[2] = {0,0};
    const uint8_t ecu_addr_ptrs_count[2] = {0,0};

    static void ecu_protocol_usart_read(QSerialPort* serial,uint8_t* data,int16_t count) {
        serial->read(reinterpret_cast<char*>(data),count);
    }

    static void ecu_protocol_usart_write(QSerialPort* serial,uint8_t* data,int16_t count) {
        serial->write(reinterpret_cast<char*>(data),count);
    }

    static void ecu_protocol_data_received(void*,simple_protocol_link_layer_t* protocol) {
        simple_protocol_id_rw_t read;
        memcpy(&read,protocol->read.frame.data,SIMPLE_PROTOCOL_ID_RW_HEAD_COUNT);
        qDebug() << "Received:" << (read.start)/4;
        read.start += read.count;
        if((read.start) < (16*16*4)) {
            simple_protocol_cmd_read(protocol,4,1,read.start,4);
        } else {
            qDebug() << "All Received";
        }
    }

    static void ecu_protocol_data_transmitted(void*,simple_protocol_link_layer_t* protocol) {
        qDebug() << "Transmitted";
    }

    static void ecu_protocol_ack(void*,simple_protocol_link_layer_t* protocol) {
        qDebug() << "ACK";
    }

    static void ecu_protocol_crc_err(void*,simple_protocol_link_layer_t* protocol) {
        simple_protocol_id_rw_t read;
        memcpy(&read,protocol->read.frame.data,SIMPLE_PROTOCOL_ID_RW_HEAD_COUNT);
        qDebug() << "CRC Error: read" << protocol->service.crc_read << ",calc" << protocol->service.crc_calc;
        qDebug() << "Link Addr:" << protocol->read.frame.head.addr;
        qDebug() << "Link ID:" << protocol->read.frame.head.id;
        qDebug() << "Link Count:" << protocol->read.frame.head.count;
        qDebug() << "Addr:" << read.addr;
        qDebug() << "Start:" << read.start;
        qDebug() << "Count:" << read.count;
    }

    static void ecu_protocol_wrong_id(void*,simple_protocol_link_layer_t* protocol) {
        qDebug() << "Wrong ID";
    }

private slots:
    void on_pushButton_Connect_toggled(bool state);
    void on_pushButton_15_clicked();
    void on_pushButton_14_clicked();
    void on_pushButton_13_clicked();
    void on_pushButton_12_clicked();
    void serial_readyRead();
};

#endif // MAINWINDOW_H
