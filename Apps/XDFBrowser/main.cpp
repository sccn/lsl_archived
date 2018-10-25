/******************************************************************************************
Application to allow the viewing and editing of XDF files. Currently, only text chunks 
can be viewed and edited. The specifications for xdf files are available at 
https://code.google.com/p/xdf/wiki/Specifications.

Written by Matthew Grivich, SCCN/UCSD, May 2014.

*******************************************************************************************/

#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	MainWindow w(nullptr, argc > 1 ? argv[1] : nullptr);
    w.show();
    
    return a.exec();
}
