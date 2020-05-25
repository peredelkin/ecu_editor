#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QDebug>
#include <QSerialPort>
#include <QSerialPortInfo>

#include "ecu_protocol.h"

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

    uint16_t ecu_read_count = 0;
    uint16_t ecu_read_count_end = 0;
    ecu_frame_t ecu_read;

    uint16_t ecu_write_count = 0;
    uint16_t ecu_write_count_end = 0;
    ecu_frame_t ecu_write;

private slots:
    void on_pushButton_Connect_toggled(bool state);
    void on_pushButton_15_clicked();
    void on_pushButton_14_clicked();
    void on_pushButton_13_clicked();
    void on_pushButton_12_clicked();
    void serial_readyRead();
};

#endif // MAINWINDOW_H
