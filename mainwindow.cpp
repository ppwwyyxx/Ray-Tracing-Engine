#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "space.hh"
#include "renderable/plane.hh"
#include "renderable/sphere.hh"
#include "view.hh"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(this->width(),this->height());

    cb_open = findChild<QPushButton*>("open");
    cb_quit = findChild<QPushButton*>("quit_it");
    cb_trace = findChild<QPushButton*>("begin_tracing");

    dist = findChild<QSlider*>("dist");
    twist = findChild<QDial*>("angle_z");
    rotate = findChild<QDial*>("angle_xy");

    pic = findChild<QGraphicsView*>("pic");
    scene = new QGraphicsScene();
    pixmap = new QPixmap(pic->width() - 10, pic->height() - 10);
    image = new QImage(pic->width() - 10, pic->height() - 10, QImage::Format_RGB32);
    scene->addPixmap(*pixmap);
    pic->setScene(scene);


    connect(cb_trace, SIGNAL(clicked()), this, SLOT(trace()));

	update_value();
}

MainWindow::~MainWindow() {
    delete ui;
}
void MainWindow::quit() {
    qApp->quit();
}

void MainWindow::trace() {
	int w, h;
	w = h = 500;
	Space s;
	Light l(Vec(5, -2, 8), Color(0.9, 1, 1), 1);
	Light lall(Vec(0, 0, 50), Color(1, 1, 1), 0.2);
	l.size = EPS;
	s.add_light(l);s.add_light(lall);

	shared_ptr<Texture> t1(new GridTexture(GridTexture::BLACK_WHITE));
	Plane plane1(InfPlane::XYPLANE, t1);
	s.add_obj(shared_ptr<RenderAble>(new Plane(plane1)));
	/*
	 *Plane plane2 = Plane::YZPLANE;
	 *s.add_obj(shared_ptr<RenderAble>(new Plane(plane2)));
	 */
	t1 = shared_ptr<Texture>(new HomoTexture(HomoTexture::BLUE));
	Sphere sphere(PureSphere::TestSphere, t1);
	s.add_obj(shared_ptr<RenderAble>(new Sphere(sphere)));

	View v(make_shared<Space>(s), Vec(0, -10, 5), Vec(0, 0, 5), 30, Geometry(w, h));
	for (int i = 0; i < h; i ++)
		for (int j = 0; j < w; j ++) {
			Color col = v.render(i, j);
			col = col * 255;
            image->setPixel(j, i, qRgb((int)col.x, (int)col.y, (int)col.z));
		}

    update_scene();
}

void MainWindow::update_scene() {
    QPixmap pixmap = QPixmap::fromImage(*image);
    scene->clear();
    scene->addPixmap(pixmap);
    scene->update();

    pic->setScene(scene);
    pic->viewport()->update();
    QApplication::processEvents();
    cout << "updated" << endl;
}

void MainWindow::update_value() {
	double twist_v = twist->value();

}

