// File: main.cc

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
