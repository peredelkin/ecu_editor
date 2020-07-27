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
    Ui::MainWindow *ui;

    QSerialPort *serial;

    simple_protocol_link_layer_t ecu_master;

    volatile uint32_t GPIOD_ODR;

    volatile void *ecu_addr_ptrs[2];

    const uint8_t ecu_addr_ptrs_width[2] = {0,0};
    const uint8_t ecu_addr_ptrs_type[2] = {0,0};
    const uint8_t ecu_addr_ptrs_count[2] = {0,0};

    static void ecu_protocol_usart_read(QSerialPort* serial,uint8_t* data,uint16_t count) {
        serial->read(reinterpret_cast<char*>(data),count);
    }

    static void ecu_protocol_usart_write(QSerialPort* serial, uint8_t* data, uint16_t count) {
        serial->write(reinterpret_cast<char*>(data),count);
        serial->flush();
    }

    static void ecu_protocol_net_data_written(void*,simple_protocol_link_layer_t* protocol) {
        qDebug() << "Data Written";
    }

    static void ecu_protocol_link_crc_err(void*,simple_protocol_link_layer_t* protocol) {
        qDebug() << "CRC Error";
    }

private slots:
    void on_pushButton_Connect_toggled(bool state);
    void on_pushButton_read_clicked();
    void on_pushButton_write_clicked();
    void serial_readyRead();
};

#endif // MAINWINDOW_H
