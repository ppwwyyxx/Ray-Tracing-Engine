// File: mainwindow.h
// Date: Thu Jun 20 16:27:23 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once

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

#include "viewer.hh"
#include "renderable/plane.hh"
#include "renderable/sphere.hh"
#include "renderable/mesh.hh"
#include "view.hh"
#include "space.hh"


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
		QPushButton *open, *save, *quit, *trace, *simplify;

		QGraphicsView *pic;
		QGraphicsScene *scene;
		QPixmap *pixmap;
		QImage *image;


		QCheckBox *smooth;

		QPushButton *shift_up, *shift_down, *shift_left, *shift_right,
					*rotate_left, *rotate_right, *orbit_left, *orbit_right,
					*twist_left, *twist_right, *zoom_in, *zoom_out;

		QLineEdit *target_rate;

		bool done_load = false;
		Space space;
		View* view = nullptr;
		CVViewer* viewer = nullptr;

		string now_fname;

		public slots:
			void do_open();
			void do_quit();
			void do_save();
			void do_trace();

			void update_mesh();

			void do_shift_up();
			void do_shift_down();
			void do_shift_left();
			void do_shift_right();

			void do_orbit_left();
			void do_orbit_right();
			void do_twist_left();
			void do_twist_right();
			void do_rotate_left();
			void do_rotate_right();
			void do_zoom_in();
			void do_zoom_out();

			void update_scene();
};

