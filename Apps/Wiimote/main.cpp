#include <QApplication>
#include "mainwindow.h"
#include <string>
//#include <boost/algorithm/string.hpp>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w(0);
    w.show();

    return a.exec();
}
