#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QDebug>
#include <QSerialPort>
#include <QSerialPortInfo>

#include "ecu_protocol.h"
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
    ECU_Protocol master_protocol;

    volatile float ign_angle_mg_by_cycle[IGN_ANGLE_MG_BY_CYCLE_MG_SCALE_N*IGN_ANGLE_MG_BY_CYCLE_RPM_SCALE_N];

    volatile void *ecu_addr_ptrs[2] = {
            NULL,
            ign_angle_mg_by_cycle
    };

private slots:
    void on_pushButton_Connect_toggled(bool state);
    void on_pushButton_15_clicked();
    void on_pushButton_14_clicked();
    void on_pushButton_13_clicked();
    void on_pushButton_12_clicked();
    void serial_readyRead();
};

#endif // MAINWINDOW_H
