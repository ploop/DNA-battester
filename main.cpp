#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  QTranslator translator;
  translator.load(":/translations/battester_" + QLocale::system().name() );
  a.installTranslator(&translator);

  qDebug() << QLocale::system().name();

  MainWindow w;
  w.show();

  return a.exec();
}
