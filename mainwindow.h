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
		QPushButton *open_action, *save_action;
		QPushButton *quit_action;
		QPushButton *start_trace, *start_simplify;
		QGraphicsView *pic;
		QGraphicsScene *scene;
		QPixmap *pixmap;
		QImage *image;

		// for ray tracing parameter
		QCheckBox *interpolation;
		QCheckBox *kd_tree;
		QSlider *dist;
		QLineEdit *target_rate;
		QPlainTextEdit *lights;

		QDial *angle_z;
		QDial *angle_xy;

		QPushButton* fx, *fy, *fz, *sxy, *syz, *sxz;

		QPushButton * fill;
		QPushButton * antia, *mos;
		QComboBox *combo;

		bool done_load = false;
		// for ray tracing
		/*
		 *    para_tool para;
		 *    static int maxN;
		 *    static int maxM;
		 *    bool done_load;
		 *    face *f;
		 *    int m;
		 *    point *p, *norm_v;
		 *    int n;
		 *
		 *    ray_tracing *tracer;
		 */
		Space space;
		View* view = nullptr;
		CVViewer* viewer = nullptr;


		public slots:
			void open();
			void quit();
			void save();
			void trace();
			void simplify();
			void flood_fill();
			void anti_alias();
			void mosaic();
			void update_scene();
			void update_model_fx();
			void update_model_fy();
			void update_model_fz();
			void update_model_sxy();
			void update_model_sxz();
			void update_model_syz();
			void change_texture();
};

#endif // MAINWINDOW_H
