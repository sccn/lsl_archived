#include "mainwindow.h"
#include "ui_mainwindow.h"
//#include "windows.h"
#include "stdlib.h"
//#include <Objbase.h>
#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>




bool clicked, open;



void MainWindow::listen(void){

	printf("welcome to the listen thread . . .\n");
	const QString qstr("Click");
	std::string str("Click");
	double ts;

	lsl::stream_info info("ClickStream", "Markers", 1, lsl::IRREGULAR_RATE, lsl::cf_string, "1234ClickStream5678");
	//info.desc().append_child("Acquisition")
	//	.append_child_value("click stream", "1");
	lsl::stream_outlet outlet(info);

	while(1)
	{
		if(clicked == true)
		{
		
			//push the sample
			ts = lsl::local_clock();
			outlet.push_sample(&str, ts);

			//display the sample and timestamp in the gui
			ui->le_ec->setText(qstr);		
			ui->le_ts->setText(QString("%L0").arg(ts, 0, 'f', 2));
			boost::this_thread::sleep(boost::posix_time::milliseconds(250));
			ui->le_ec->setText(QString(""));

			clicked = false;

		}
		if(open == false)break;
	}
}

void MainWindow::got_clicked(void){

	clicked = true;

}

void MainWindow::got_started(void){

	open = true;
	listen_thread_.reset(new boost::thread(&MainWindow::listen, this));
	ui->click->setEnabled(true);
	ui->stop->setEnabled(true);	
	ui->start->setEnabled(false);	

}

void MainWindow::got_stopped(void){


	ui->click->setEnabled(false);
	ui->stop->setEnabled(false);
	ui->start->setEnabled(true);
	open = false;

	try{
		if(listen_thread_)
		{
				
			listen_thread_->interrupt();
			listen_thread_->join();
		}
	}catch(std::exception &e) {
		QMessageBox::critical(this,"Error",(std::string("Could not stop the background processing: ")+=e.what()).c_str(),QMessageBox::Ok);
		return;
	}


}


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
	ui->setupUi(this);

	
	QObject::connect(ui->click, SIGNAL(clicked()), this, SLOT(got_clicked()));	
	QObject::connect(ui->start, SIGNAL(clicked()), this, SLOT(got_started()));	
	QObject::connect(ui->stop, SIGNAL(clicked()), this, SLOT(got_stopped()));	
	clicked = false;
	open = true;

}
MainWindow::~MainWindow() {
	//listen_thread_->interrupt();
	if(listen_thread_)
	{
				
		listen_thread_->interrupt();
		listen_thread_->join();
	}
	open = false;
	delete ui;
}
