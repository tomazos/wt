#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsLineItem>

#include "ladybug/ladybug_client.h"
#include "ladybug/ladybug_state.pb.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

 protected:
  void timerEvent(QTimerEvent *event);

 private:
  void UpdateView();

  LadyBugClient client;
  LadyBugState state;
  Ui::MainWindow *ui;
  QGraphicsLineItem *line1;
  QGraphicsLineItem *line2;
  QGraphicsLineItem *line3;

  QGraphicsScene *scene;
  QGraphicsPixmapItem *pixmap;
  QImage image;
};

#endif  // MAINWINDOW_H
