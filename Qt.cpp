// File: Qt.cpp
// Date: Thu Jun 20 16:31:32 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#include "mainwindow.h"
#include <QApplication>
#include <iostream>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
