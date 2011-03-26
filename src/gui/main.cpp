#include <QtGui/QApplication>
#include "MainWindow.h"
#include "../G/GLogger.h"

GLogger logg("lastrun.log");

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}