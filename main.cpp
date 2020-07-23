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
