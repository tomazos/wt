#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QGLWidget>

#include "ladybug/camera_subsystem.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  client("192.168.0.15"),
  ui(new Ui::MainWindow),
  image(CameraSubsystem::kWidth, CameraSubsystem::kHeight, QImage::Format_RGB888)
{
  ui->setupUi(this);
  ui->graphicsView->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
  scene = new QGraphicsScene(this);

  pixmap = scene->addPixmap(QPixmap::fromImage(image));
  pixmap->setPos(210,0);

  scene->addEllipse(QRectF(0,0,100,100));
  line1 = scene->addLine(50, 50, 50, 100);
  scene->addEllipse(QRectF(100,0,100,100));
  line2 = scene->addLine(150,50,150,50);
  line3 = scene->addLine(0,0,0,0);
  QPen pen;
  pen.setWidth(5);
  line3->setPen(pen);

  ui->graphicsView->setScene(scene);

  startTimer(50);
}

void MainWindow::timerEvent(QTimerEvent* event) {
  state = client.GetState();
  UpdateView();
}

void MainWindow::UpdateView() {
  line1->setLine(50,50,50+50*cos(state.direction1()),50+50*sin(state.direction1()));
  line2->setLine(150,50,150+50*cos(state.direction2()),50+50*sin(state.direction2()));
  line3->setLine(0,300,state.sonar()*300,300);

  if (state.has_image()) {
    std::memcpy(image.bits(), state.image().data(), CameraSubsystem::kBytes);
    pixmap->setPixmap(QPixmap::fromImage(image));
  }
}

MainWindow::~MainWindow()
{
  delete ui;
}

