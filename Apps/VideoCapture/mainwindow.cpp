#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <Objbase.h>
#include <string>
#include <vector>
#include <iostream>



MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {

	ui->setupUi(this);
	QObject::connect(ui->newVideoButton, SIGNAL(clicked()), this, SLOT(newVideo()));
	//theWindow = this;
	
#ifdef _WIN32

#else
	cameraInfos = QCameraInfo::availableCameras();
#endif
	//cameraInfos = QCameraInfo::availableCameras();
	//for (QList<QCameraInfo>::Iterator it = cameraInfos.begin();
	//	it != cameraInfos.end(); ++it){
	//	std::cout << it->description().toStdString() << std::endl;
	//	ui->comboBox->addItem(it->description());
	//}
	//std::cout << ui->comboBox->currentText().toStdString() << std::endl;

}

void MainWindow::newVideo(void){

	boost::shared_ptr<VideoPanel> vp(new VideoPanel());
	//VideoPanel *vp = new VideoPanel;
	videoPanels.push_back(vp);
	vp->show();
	

}

void MainWindow::closeEvent(QCloseEvent *ev) {
//	if (outletButtons_ || outletPosition_)
//		ev->ignore();

	// close the open video panels
	videoPanels.clear();	
}



MainWindow::~MainWindow() {


	delete ui;
}
