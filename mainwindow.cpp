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


    interpolation = findChild<QCheckBox*>("interpolation");
    kd_tree = findChild<QCheckBox*>("kd_tree");
    dist = findChild<QSlider*>("dist");
    angle_z = findChild<QDial*>("angle_z");
    angle_xy = findChild<QDial*>("angle_xy");
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
    connect(start_simplify, SIGNAL(clicked()), this, SLOT(simplify()));

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
	space.add_light(Light(Vec(9, 2, 50), Color::WHITE, 2.0));
	space.add_light(Light(Vec(-9, 2, 50), Color::WHITE, 2.0));
	shared_ptr<Texture> tpic(new ImgTexture("res/texture.jpg", 80, 1));
	space.add_obj(new Plane(InfPlane::XYPLANE, tpic));
	space.finish();
	view = new View(space, Vec(0, 0, 12), Vec(0, 0, 2), 15, Geometry(pixmap->width(), pixmap->height()));
	viewer = new CVViewer(*view);
	trace();
	done_load = false;
}

MainWindow::~MainWindow() {
	delete ui;
	delete view;
	delete viewer;
}

void MainWindow::open() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "",
               tr("Obj Files (*.obj);;"));
    if(fileName != "") {
        cout << "file name is " << fileName.toStdString() << endl;
        QFile file(fileName);
        if(!file.open(QIODevice::ReadOnly)) {
            QMessageBox::critical(this, tr("Error"), tr("Could not open file"));
            return;
        }
		Mesh mesh(fileName.toStdString().c_str(), Vec(0, 0, 2), 5);
		mesh.smooth = true;
		shared_ptr<Texture> tred = make_shared<HomoTexture>(Surface::RED);
		mesh.set_texture(tred);
		mesh.finish();
		space.clear();
		space.add_obj(shared_ptr<RenderAble>(new Mesh(mesh)));
		space.finish();
    }
	done_load = true;
    cout << "opened ." << endl;
}

void MainWindow::save() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "",
               tr("Bmp Files (*.bmp);;"));
    if(fileName != "") {
        //bmp->output_pic(fileName.toStdString());
    }
}

void MainWindow::quit() {
    QMessageBox messageBox;
    messageBox.setWindowTitle(tr("MainWindow"));
    messageBox.setText(tr("Do you really want to quit?"));
    messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    messageBox.setDefaultButton(QMessageBox::No);
    if(messageBox.exec() == QMessageBox::Yes) {
        qApp->quit();
    }
}

void MainWindow::trace() {
    if(done_load == false) {
        QMessageBox::critical(this, tr("Error"), tr("Haven't open any obj file"));
        return;
    }


	viewer->render_all();
	int w = pixmap->width(), h = pixmap->height();
    for(int i = 0 ; i < w; i++) {
        for(int j = 0; j < h; j++) {
            Color col = viewer->r.get(i, j);
            image->setPixel(i, j, qRgb((int)(col.x * 255), (int)(col.y * 255), (int)(col.z * 255)));
        }
    }

    update_scene();
}

void MainWindow::simplify() {
    if(done_load == false) {
        QMessageBox::critical(this, tr("Error"), tr("Haven't open any obj file"));
        return;
    }

    // modify para_tool

    // simplify meshed
    //mesh_simplify *simplifier = new mesh_simplify(p, n, f, m);
    int last = clock();

    // set rate
//    simplifier->set_target((int)(m * atof(para.get_para("target_rate").c_str())) / 100.0);

    // simplifier
	/*
     *pair<pair<point*,face*>,pair<int,int> > ret = simplifier->simplify();
     *p = ret.first.first;
     *f = ret.first.second;
     *n = ret.second.first;
     *m = ret.second.second;
     *obj_reader *reader = new obj_reader();
     *reader->prepare_model(p, norm_v, n, f, m);
     *delete tracer;
     *delete reader;
     *tracer = new ray_tracing(para, p, norm_v, n, f, m);
	 */
    update_scene();

    int cur = clock();
    cout << endl;
    cout << "mesh simplify time cost = " << (cur - last) * 1.0 / CLOCKS_PER_SEC << endl;

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

void MainWindow::update_model_fx() {
    if(done_load == false) {
        QMessageBox::critical(this, tr("Error"), tr("Haven't open any obj file"));
        return;
    }

}

void MainWindow::update_model_fy() {
    if(done_load == false) {
        QMessageBox::critical(this, tr("Error"), tr("Haven't open any obj file"));
        return;
    }
}

void MainWindow::update_model_fz() {
    if(done_load == false) {
        QMessageBox::critical(this, tr("Error"), tr("Haven't open any obj file"));
        return;
    }

}

void MainWindow::update_model_sxy() {
    if(done_load == false) {
        QMessageBox::critical(this, tr("Error"), tr("Haven't open any obj file"));
        return;
    }

}

void MainWindow::update_model_sxz() {
/*
 *    if(done_load == false) {
 *        QMessageBox::critical(this, tr("Error"), tr("Haven't open any obj file"));
 *        return;
 *    }
 *
 *    for(int i = 0; i < n; i++) {
 *        swap(p[i].x, p[i].z);
 *    }
 *    obj_reader *reader = new obj_reader();
 *    reader->prepare_model(p, norm_v, n, f, m);
 *    delete tracer;
 *    delete reader;
 *    tracer = new ray_tracing(para, p, norm_v, n, f, m);
 */
}

void MainWindow::update_model_syz() {
/*
 *    if(done_load == false) {
 *        QMessageBox::critical(this, tr("Error"), tr("Haven't open any obj file"));
 *        return;
 *    }
 *
 *    for(int i = 0; i < n; i++) {
 *        swap(p[i].z, p[i].y);
 *    }
 *    obj_reader *reader = new obj_reader();
 *    reader->prepare_model(p, norm_v, n, f, m);
 *    delete tracer;
 *    delete reader;
 *    tracer = new ray_tracing(para, p, norm_v, n, f, m);
 */
}


void MainWindow::flood_fill() {
    QRgb cr = image->pixel(0, 0);
    int r = cr >> 16;
    int g = (cr >> 8) & 255;
    int b = (cr) & 255;
    QRgb ncr = ((255 - r) << 16) + ((255 - g) << 8) + (255 - b);
    queue<pair<int,int> > q;
    q.push(make_pair(0, 0));
    while(!q.empty()) {
        pair<int,int> cur = q.front();
        q.pop();
        int x = cur.first;
        int y = cur.second;
        if(x > 0 && image->pixel(x - 1, y) == cr) {
            image->setPixel(x - 1, y, ncr);
            q.push(make_pair(x - 1, y));
        }
        if(y > 0 && image->pixel(x, y - 1) == cr) {
            image->setPixel(x, y - 1, ncr);
            q.push(make_pair(x, y - 1));
        }
        if(x + 1 < image->width() && image->pixel(x + 1, y) == cr) {
            image->setPixel(x + 1, y, ncr);
            q.push(make_pair(x + 1, y));
        }
        if(y + 1 < image->height() && image->pixel(x, y + 1) == cr) {
            image->setPixel(x, y + 1, ncr);
            q.push(make_pair(x, y + 1));
        }
    }
    update_scene();
}

void MainWindow::anti_alias() {
	/*
     *QImage *new_image = new QImage(image->width(), image->height(), QImage::Format_RGB32);
     *for(int x = 0; x < image->width(); x++) {
     *    for(int y = 0; y < image->height(); y++) {
     *        int r = 0;
     *        int b = 0;
     *        int g = 0;
     *        int counter = 0;
     *        for(int dx = -1; dx < 1; dx++) {
     *            for(int dy = -1; dy < 1; dy++) {
     *                x += dx;
     *                y += dy;
     *                if(x >= 0 && x < image->width() && y >= 0 && y < image->height()) {
     *                    r += image->pixel(x, y) >> 16;
     *                    g += (image->pixel(x, y) >> 8) & 255;
     *                    b += (image->pixel(x, y)) & 255;
     *                    counter ++;
     *                }
     *                x -= dx;
     *                y -= dy;
     *            }
     *        }
     *        r /= counter;
     *        b /= counter;
     *        g /= counter;
     *        new_image->setPixel(x, y, (r << 16) + (g << 8) + b);
     *    }
     *}
     *delete image;
     *image = new_image;
     *update_scene();
	 */
}

void MainWindow::mosaic() {
    QImage *new_image = new QImage(image->width(), image->height(), QImage::Format_RGB32);
    const int S = 10;
    for(int x = 0; x < image->width(); x+= S) {
        for(int y = 0; y < image->height(); y+= S) {
            for(int dx = 0; dx < S && x + dx < image->width(); dx++) {
                for(int dy = 0; dy < S && y + dy < image->height(); dy++) {
                    new_image->setPixel(x + dx, y + dy, image->pixel(x, y));
                }
            }
        }
    }
    delete image;
    image = new_image;
    update_scene();
}

void MainWindow::change_texture() {
	/*
     *for(int i = 0; i < m; i++) {
     *    if(combo->currentIndex() == 0) {
     *        f[i].text = &ceramics;
     *    }
     *    else if(combo->currentIndex() == 1){
     *        f[i].text = &silver;
     *    }
     *    else if(combo->currentIndex() == 2){
     *        f[i].text = &mirror;
     *    }
     *    else {
     *        f[i].text = &glass;
     *    }
     *}
	 */
}
