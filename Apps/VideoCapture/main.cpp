#include "mainwindow.h"
#include <QApplication>
#include <QComboBox>
#include <QCameraInfo>
#include <iostream>

int main(int argc, char *argv[])
{


	QApplication app( argc, argv );
	//QComboBox *combo = new QComboBox;
	//int i;
	//QList<QCameraInfo> info = QCameraInfo::availableCameras();
	//foreach(QCameraInfo i, info)
	//	combo->addItem(i.description());
	////for (i = 0; i < 2; i++)
	////	combo->addItem(QString("lala"));
	//combo->show();
	//std::cout << combo->currentText().toStdString() << std::endl;
	//return app.exec();
 //  delete combo;
	//QApplication a(argc, argv);
	MainWindow w;
    w.show();
	    
    return app.exec();
}
