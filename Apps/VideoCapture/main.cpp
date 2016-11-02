#include "mainwindow.h"
#include <QtWidgets\QApplication>

MainWindow *theWindow = NULL;

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
	MainWindow w;
    w.show();
   
    return a.exec();
}
