#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QGLWidget>
#include <QKeyEvent>
#include <QDebug>

#include "ladybug/camera_subsystem.h"

constexpr int direction1_top = 30;
constexpr int direction1_left = 30;
constexpr int direction2_top = 30;
constexpr int direction2_left = 230;

constexpr int direction_size = 200;
constexpr int direction_radius = direction_size / 2;

constexpr int direction1_center_x = direction1_left +  direction_radius;
constexpr int direction1_center_y = direction1_top +  direction_radius;

constexpr int direction2_center_x = direction2_left +  direction_radius;
constexpr int direction2_center_y = direction2_top +  direction_radius;

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
  pixmap->setPos(9,9);

  QPen pen;
  pen.setWidth(5);
  pen.setColor(Qt::cyan);
  scene->addEllipse(QRectF(direction1_left, direction1_top,direction_size,direction_size),pen);
  line1 = scene->addLine(direction1_center_x, direction1_center_y, direction1_center_x, direction1_top,pen);
  scene->addEllipse(QRectF(direction2_left, direction2_top,direction_size,direction_size),pen);
  line2 = scene->addLine(direction2_center_x, direction2_center_y, direction2_center_x, direction2_top,pen);
  pen.setWidth(20);
  line3 = scene->addLine(100,100,100,100,pen);

  ui->graphicsView->setScene(scene);

  grabKeyboard();
  startTimer(100);
}

void MainWindow::timerEvent(QTimerEvent* event [[gnu::unused]]) {
  LadyBugCommand command;
  if (forward_pressed) {
    command.set_left_power(0.7);
    command.set_right_power(0.7);
  } else if (left_pressed) {
    command.set_left_power(-0.7);
    command.set_right_power(0.7);
  } else if (right_pressed) {
    command.set_left_power(0.7);
    command.set_right_power(-0.7);
  } else if (backward_pressed) {
    command.set_left_power(-0.7);
    command.set_right_power(-0.7);
  } else {
    command.set_left_power(0);
    command.set_right_power(0);
  }
  state = client.SendCommand(command);
  UpdateView();
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
  switch (event->key()) {
  case Qt::Key_W: forward_pressed = true; break;
  case Qt::Key_A: left_pressed = true; break;
  case Qt::Key_D: right_pressed = true; break;
  case Qt::Key_S: backward_pressed = true; break;
  default:
    QMainWindow::keyPressEvent(event);
  }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
  switch (event->key()) {
  case Qt::Key_W: forward_pressed = false; break;
  case Qt::Key_A: left_pressed = false; break;
  case Qt::Key_D: right_pressed = false; break;
  case Qt::Key_S: backward_pressed = false; break;
  default: QMainWindow::keyReleaseEvent(event);
  }
}

void MainWindow::UpdateView() {
  line1->setLine(direction1_center_x,
                 direction1_center_y,
                 direction1_center_x+ direction_radius*cos(state.direction1()),
                 direction1_center_y+ direction_radius*sin(state.direction1()));
  line2->setLine(direction2_center_x,
                 direction2_center_y,
                 direction2_center_x+ direction_radius*cos(state.direction2()),
                 direction2_center_y+ direction_radius*sin(state.direction2()));

  line3->setLine(width()/2,height()-18, width()/2, height()-18-state.sonar()*500);

  if (state.has_image()) {
    std::memcpy(image.bits(), state.image().data(), CameraSubsystem::kBytes);
    pixmap->setPixmap(QPixmap::fromImage(image.scaled(width()-18, height()-18,
                                                      Qt::IgnoreAspectRatio,
                                                      Qt::FastTransformation)));
  }
}

MainWindow::~MainWindow()
{
  delete ui;
}

