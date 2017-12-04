#include "mainwindow.h"

#include <boost/thread.hpp>

MainWindow::MainWindow(QWidget *parent, const std::string &config_file) :
QMainWindow(parent), ui(new Ui::MainWindow){

	ui->setupUi(this);
	QObject::connect(ui->pbNewFrame, SIGNAL(clicked()), this, SLOT(LaunchNewVideoFrame()));
}

MainWindow::~MainWindow(){
delete ui;
}

void MainWindow::LaunchNewVideoFrame() {
	VideoFrame *videoFrame = new VideoFrame(this);
	videoFrame->show();
}


