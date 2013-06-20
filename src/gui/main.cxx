// File: main.cc
// Date: Thu Jun 20 16:40:33 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#include "window.hh"
#include <QApplication>
#include <iostream>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
