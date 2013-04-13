#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <sstream>
#include <queue>
#include <set>
#include <QMainWindow>
#include <QAction>
#include <QPushButton>
#include <QGraphicsView>
#include <QPixmap>
#include <QPoint>
#include <QColor>
#include <QCheckBox>
#include <QTimer>
#include <QLineEdit>
#include <QMessageBox>
#include <QFile>
#include <QDial>
#include <QFileDialog>
#include <QTextStream>
#include <QThread>
#include <QComboBox>
#include <QSlider>
#include <QPlainTextEdit>


using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QPushButton *cb_open, *cb_quit;
    QPushButton *cb_trace;
    QGraphicsView *pic;
    QGraphicsScene *scene;
    QPixmap *pixmap;
    QImage *image;

    // for ray tracing parameter
    QSlider *dist;
    QPlainTextEdit *lights;
    QDial *twist, *rotate;
    QPushButton* fx, *fy, *fz, *sxy, *syz, *sxz;


public slots:
    void quit();
    void trace();
	void update_value();
	void update_scene();
};

#endif // MAINWINDOW_H
