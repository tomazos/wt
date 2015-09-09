#include "mainwindow.h"
#include <QApplication>
#include <QWindow>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  MainWindow w;
  w.show();
  w.windowHandle()->setScreen(QGuiApplication::primaryScreen());
  w.showFullScreen();

  return a.exec();
}
