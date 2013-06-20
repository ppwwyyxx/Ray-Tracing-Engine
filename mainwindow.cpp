
#include <sstream>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "lib/Timer.hh"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    this->setFixedSize(this->width(),this->height());

    open_action = findChild<QPushButton*>("open");
    save_action = findChild<QPushButton*>("save");
    quit_action = findChild<QPushButton*>("quit_it");
    start_trace = findChild<QPushButton*>("begin_tracing");
    start_simplify = findChild<QPushButton*>("begin_simplify");
    pic = findChild<QGraphicsView*>("pic");
    target_rate = findChild<QLineEdit*>("target_rate");
    scene = new QGraphicsScene();
    pixmap = new QPixmap(pic->width() - 10, pic->height() - 10);
    image = new QImage(pic->width() - 10, pic->height() - 10, QImage::Format_RGB32);
    scene->addPixmap(*pixmap);
    pic->setScene(scene);


    smooth = findChild<QCheckBox*>("smooth");

    dist = findChild<QSlider*>("dist");
    orbit = findChild<QDial*>("orbit");
    rotate = findChild<QDial*>("rotate");

    lights = findChild<QPlainTextEdit*>("lights");
    fx = findChild<QPushButton*>("fx");
    fy = findChild<QPushButton*>("fy");
    fz = findChild<QPushButton*>("fz");
    sxy = findChild<QPushButton*>("sxy");
    sxz = findChild<QPushButton*>("sxz");
    syz = findChild<QPushButton*>("syz");
    fill = findChild<QPushButton*>("fill");
    antia = findChild<QPushButton*>("antia");
    mos = findChild<QPushButton*>("mosaic");

    combo = findChild<QComboBox*>("texture");

    connect(open_action, SIGNAL(clicked()), this, SLOT(open()));
    connect(save_action, SIGNAL(clicked()), this, SLOT(save()));
    connect(quit_action, SIGNAL(clicked()), this, SLOT(quit()));
    connect(start_trace, SIGNAL(clicked()), this, SLOT(trace()));
    connect(start_simplify, SIGNAL(clicked()), this, SLOT(update_mesh()));

	connect(smooth, SIGNAL(toggled(bool)), this, SLOT(update_mesh()));

    connect(fx, SIGNAL(clicked()), this, SLOT(update_model_fx()));
    connect(fy, SIGNAL(clicked()), this, SLOT(update_model_fy()));
    connect(fz, SIGNAL(clicked()), this, SLOT(update_model_fz()));
    connect(sxy, SIGNAL(clicked()), this, SLOT(update_model_sxy()));
    connect(syz, SIGNAL(clicked()), this, SLOT(update_model_syz()));
    connect(sxz, SIGNAL(clicked()), this, SLOT(update_model_sxz()));

    connect(fill, SIGNAL(clicked()), this, SLOT(flood_fill()));
    connect(antia, SIGNAL(clicked()), this, SLOT(anti_alias()));
    connect(mos, SIGNAL(clicked()), this, SLOT(mosaic()));

    connect(combo, SIGNAL(currentTextChanged(QString)),this, SLOT(change_texture()));



	done_load = true;
	space.add_light(Light(Vec(0, -10, 12), Color::WHITE, 2.0));
	space.add_light(Light(Vec(10, 10, 12), Color::WHITE, 2.0));
	space.add_light(Light(Vec(10, -10, 12), Color::WHITE, 2.0));
	space.add_light(Light(Vec(9, 2, 30), Color::WHITE, 2.0));
	space.add_light(Light(Vec(-9, 2, 30), Color::WHITE, 2.0));
	shared_ptr<Texture> tpic(new ImgTexture("res/texture.jpg", 80, 1));
	space.add_obj(new Plane(InfPlane::XYPLANE, tpic));
	space.finish();
	view = new View(space, Vec(0, 0, 12), Vec(0, 0, 2), 15, Geometry(pixmap->width(), pixmap->height()));
	viewer = new CVViewer(*view);
	trace();
	done_load = false;
}

void MainWindow::trace() {
    if(done_load == false) {
        QMessageBox::critical(this, tr("Error"), tr("Please open a obj file"));
        return;
    }
	int orbit_ang = orbit->value();
	view->orbit(orbit_ang, true);
	cout << "angle: " << orbit_ang << endl;
	viewer->render_all();
	int w = pixmap->width(), h = pixmap->height();
	REP(i, w) REP(j, h) {
		Color col = viewer->r.get(i, j);
		image->setPixel(i, j, qRgb((int)(col.x * 255), (int)(col.y * 255), (int)(col.z * 255)));
	}
	update_scene();
}

MainWindow::~MainWindow() { delete ui; delete view; delete viewer; }
void MainWindow::quit() { qApp->quit(); }

void MainWindow::open() {
	QString fname = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Obj Files (*.obj);;"));
	if(fname != "") {
		QFile file(fname);
		if(!file.open(QIODevice::ReadOnly)) {
			QMessageBox::critical(this, tr("Error"), tr("Could not open file"));
			return;
		}

		now_fname = fname.toStdString();
		Mesh mesh(fname.toStdString().c_str(), Vec(0, 0, 2), 5);
		mesh.smooth = smooth->isChecked();
		shared_ptr<Texture> tred = make_shared<HomoTexture>(Surface::RED);
		mesh.set_texture(tred);
		mesh.finish();
		space.clear();
		space.add_obj(make_shared<Mesh>(mesh));
		space.finish();
	}
	done_load = true;
}

void MainWindow::save() {
	QString fname = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("Png Files (*.png);;"));
	if(fname != "") viewer->r.save(fname.toStdString().c_str());
}

void MainWindow::update_mesh() {
	if(done_load == false) {
		QMessageBox::critical(this, tr("Error"), tr("Please open a obj file."));
		return;
	}

	Mesh mesh(now_fname.c_str(), Vec(0, 0, 2), 5);
	mesh.smooth = smooth->isChecked();
	shared_ptr<Texture> tred = make_shared<HomoTexture>(Surface::RED);
	mesh.set_texture(tred);

	stringstream text(target_rate->text().toStdString());
	double rate;
	text >> rate;
	mesh.simplify(rate / 100);
	mesh.finish();

	space.clear();
	space.add_obj(make_shared<Mesh>(mesh));
	space.finish();
	trace();
}

void MainWindow::update_scene() {
	QPixmap pixmap = QPixmap::fromImage(*image);
	scene->clear();
	scene->addPixmap(pixmap);
	scene->update();

	pic->setScene(scene);
	pic->viewport()->update();
	QApplication::processEvents();
}

void MainWindow::update_model_fx() {

}

void MainWindow::update_model_fy() {
}

void MainWindow::update_model_fz() {
}

void MainWindow::update_model_sxy() {
}

void MainWindow::update_model_sxz() {
}

void MainWindow::update_model_syz() {
}


void MainWindow::flood_fill() {
}

void MainWindow::anti_alias() {
}

void MainWindow::mosaic() {
}

void MainWindow::change_texture() {
}
