#include "mainwindow.h"
#include "ui_mainwindow.h"
//#include "windows.h"
#include "stdlib.h"
//#include <Objbase.h>
#include <string>
#include <vector>
#include <QMainWindow>
#include <QCloseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>
#include <boost/asio.hpp>
#include <zmq_client.h>
//#include <tcp_client.h>





//QString port = "5000";
//const int pupilpro_sr = 24; // fps of world camera
// TODO: make these variable 

void MainWindow::parse(std::string str, std::vector<double>& sample, std::vector<std::string>& markers, int& marker_cnt)
 {

	int pos1, pos2;
	std::string tmp_str;
	std::string match;
	

	std::cout << str;

	// first initialize the sample buffer
	for(std::vector<double>::iterator it = sample.begin(); it!=sample.end(); ++it)
		*it = 0.0;
		
	match = "confidence:";
	pos1 = str.find(match);
	if(pos1!=std::string::npos)
	{
		pos1+= match.size();
		pos2 = 0;
		while(str[pos1+pos2] !='\n')
			pos2++;
		tmp_str = str.substr(pos1,pos2);
		sample[0] = boost::lexical_cast<double>(tmp_str);
		//std::cout << "this is our confidence:\n\t" + tmp_str+"\n";
		//std::cout << "this is our confidence:\n\t" << sample[0];
		//std::cout << "\n";
	}

	match = "norm_gaze:";
	pos1 = str.find(match);
	if(pos1!=std::string::npos)
	{
		pos1+= match.size();
		if(str.substr(pos1, pos1+4).find("None")!=std::string::npos)
			;//std::cout << "None\n";
		else
		{
			pos1+=1; // account for openeing parenthesis
			pos2 = 0;
			while(str[pos1+pos2] !=',')
				pos2++;
			tmp_str = str.substr(pos1,pos2);
			sample[1] = boost::lexical_cast<double>(tmp_str);
			pos1+=pos2+2; // skip the comma and the space

			//std::cout << "this is our norm_gazeX:\n\t" + tmp_str+"\n";
			//std::cout << "this is our norm_gazeX:\n\t" << sample[1];
			//std::cout << "\n";

			pos2 = 0;
			while(str[pos1+pos2] !=')')
				pos2++;
			tmp_str = str.substr(pos1,pos2);

			//std::cout << "this is our norm_gazeY:\n\t" + tmp_str+"\n";
			sample[2] = boost::lexical_cast<double>(tmp_str);
			//std::cout << "this is our norm_gazeY:\n\t" << sample[2];
			//std::cout << "\n";
		}
	}

	match = "apparent_pupil_size:";
	pos1 = str.find(match);
	if(pos1!=std::string::npos)
	{
		pos1+= match.size();
		pos2 = 0;
		while(str[pos1+pos2] !='\n')
			pos2++;
		tmp_str = str.substr(pos1,pos2);
		//std::cout << "this is our confidence:\n\t" + tmp_str+"\n";
		sample[3] = boost::lexical_cast<double>(tmp_str);
		//std::cout << "this is our confidence:\n\t" << sample[0];
		//std::cout << "\n";
	}

	match = "norm_pupil:";
	pos1 = str.find(match);
	if(pos1!=std::string::npos)
	{
		pos1+= match.size();
		
		if(str.substr(pos1, pos1+4).find("None")!=std::string::npos)
			;//std::cout << "None\n";
		else
		{
			pos1++;
		
			pos2 = 0;
			while(str[pos1+pos2] !=',')
				pos2++;
			tmp_str = str.substr(pos1,pos2);
			sample[4] = boost::lexical_cast<double>(tmp_str);
			pos1+=pos2+2; // skip the comma and the space
	
			pos2 = 0;
			while(str[pos1+pos2] !=')')
				pos2++;
			tmp_str = str.substr(pos1,pos2);


			//std::cout << "this is our confidence:\n\t" + tmp_str+"\n";
			sample[5] = boost::lexical_cast<double>(tmp_str);
			//std::cout << "this is our confidence:\n\t" << sample[0];
			//std::cout << "\n";
		}
	}

	match = "timestamp:";
	pos1 = str.find(match);
	if(pos1!=std::string::npos)
	{
		pos1+= match.size();
		pos2 = 0;
		while(str[pos1+pos2] !='\n')
			pos2++;
		tmp_str = str.substr(pos1,pos2);
		sample[6] = boost::lexical_cast<double>(tmp_str);
		//std::cout << "this is our confidence:\n\t" + tmp_str+"\n";
		//std::cout << "this is our confidence:\n\t" << sample[0];
		//std::cout << "\n";
	}

	for(std::vector<double>::iterator it = sample.begin(); it!=sample.end(); ++it)
	{		
		std::cout << *it;
		std::cout << std::endl;
	}


}

void MainWindow::listen(QString serverIP, QString port, int fps){

	QString status; 
	
	const int pp_channs = 7;
	std::vector<double> sample(pp_channs);
	double smp;
	std::vector<std::string> markers; 
	int marker_cnt;

	//create data streaminfo
	std::string lsl_id = QString("PupilPro %1:%2").arg(serverIP, port).toStdString();
	lsl::stream_info lsl_info("PupilPro", "EyeTracker",
		pp_channs, fps, lsl::cf_double64, "fooid");//lsl_id);
	// append some meta-data
	lsl::xml_element channels = lsl_info.desc().append_child("channels");
	
	channels.append_child("channel")
		.append_child_value("label", "confidence");
	channels.append_child("channel")
		.append_child_value("label", "norm_gazeX");
	channels.append_child("channel")
		.append_child_value("label", "norm_gazeY");
	channels.append_child("channel")
		.append_child_value("label", "apparent_pupil_size");
	channels.append_child("channel")
		.append_child_value("label", "norm_pupilX");
	channels.append_child("channel")
		.append_child_value("label", "norm_pupilY");
	//channels.append_child("channel")
	//	.append_child_value("label", "roi_centerX");
	//channels.append_child("channel")
	//	.append_child_value("label", "roi_centerY");
	channels.append_child("channel")
		.append_child_value("label", "timestamp");
	
	lsl_info.desc().append_child("eye tracking")
		.append_child_value("manufacturer","Pupil Pro");
	// make a new data outlet
	lsl::stream_outlet lsl_outlet(lsl_info);

	// create event stream info
	lsl::stream_info lsl_event_info("PupilPro_Markers", "Markers",
		1, lsl::IRREGULAR_RATE, lsl::cf_string, lsl_id + " Marker");
	// make a new event outlet
	//lsl::stream_outlet lsl_event_outlet(lsl_event_info,1);

	try {
		
		


		// create the zmq client
		ZMQ_Client zmq_client("tcp://"+serverIP.toStdString()+":"+port.toStdString());
		while(!stop_)
		{
			
			//smp = 1.0;
			zmq_client.socket_pull();
			MainWindow::parse(zmq_client.msg_string(), sample, markers, marker_cnt);
			lsl_outlet.push_sample(sample);
		}
		
    }
	catch(boost::thread_interrupted &) {
		// thread was interrupted: no error
	}
	catch(std::exception &e) {
		// any other error
		//QMessageBox::critical(this,"Error",(std::string("Error during processing: ")+=e.what()).c_str(),QMessageBox::Ok);
		status = QString("error during processing: %1").arg(e.what());
		emit sendMessage(status);
	}


}

void MainWindow::load_config_dialog() {
	QString sel = QFileDialog::getOpenFileName(this,"Load Configuration File","","Configuration Files (*.cfg)");
	if (!sel.isEmpty())
		load_config(sel.toStdString());
}

void MainWindow::save_config_dialog() {
	QString sel = QFileDialog::getSaveFileName(this,"Save Configuration File","","Configuration Files (*.cfg)");
	if (!sel.isEmpty())
		save_config(sel.toStdString());
}

void MainWindow::statusMessage(QString msg) {
	ui->connectionStatus->setText(msg);
}

void MainWindow::closeEvent(QCloseEvent *ev) {
	if (listen_thread_)
		ev->ignore();
}

void MainWindow::load_config(const std::string &filename) {
	using boost::property_tree::ptree;
	ptree pt;
	// parse file
	try {
		read_xml(filename, pt);
	} catch(std::exception &e) {
		QMessageBox::information(this,"Error",(std::string("Cannot read config file: ")+= e.what()).c_str(),QMessageBox::Ok);
		return;
	}
	// get config values
	try {
		ui->serverIP->setText(pt.get<std::string>("settings.serverip","127.0.0.1").c_str());
		ui->port->setText(pt.get<std::string>("settings.port","5000").c_str());
		ui->fps->setText(pt.get<std::string>("settings.fps","30").c_str());
	
	} catch(std::exception &) {
		QMessageBox::information(this,"Error in Config File","Could not read out config parameters.",QMessageBox::Ok);
		return;
	}
}

void MainWindow::save_config(const std::string &filename) {
	using boost::property_tree::ptree;
	ptree pt;
	// transfer UI content into property tree
	try {
		pt.put("settings.serverip",ui->serverIP->text().toStdString());
		pt.put("settings.port",ui->port->text().toStdString());
		pt.put("settings.fps",ui->fps->text().toStdString());
	} catch(std::exception &e) {
		QMessageBox::critical(this,"Error",(std::string("Could not prepare settings for saving: ")+=e.what()).c_str(),QMessageBox::Ok);
	}
	// write to disk
	try {
		write_xml(filename, pt);
	} catch(std::exception &e) {
		QMessageBox::critical(this,"Error",(std::string("Could not write to config file: ")+=e.what()).c_str(),QMessageBox::Ok);
	}
}


// start/stop the pupil pro connection
void MainWindow::link_pupilpro() {
	if (listen_thread_) {
		// === perform unlink action ===
		try {
			setCursor(Qt::WaitCursor);
			stop_ = true;
			listen_thread_->join();
			listen_thread_->interrupt();
			listen_thread_.reset();
			setCursor(Qt::ArrowCursor);
		} catch(std::exception &e) {
			QMessageBox::critical(this,"Error",(std::string("Could not stop the background processing: ")+=e.what()).c_str(),QMessageBox::Ok);
			return;
		}
		// indicate that we are now successfully unlinked
		emit sendMessage("not connected");
		ui->linkButton->setText("Link");
	} else {
		// === perform link action ===
		try {
			// get the UI parameters...
			QString serverIP = ui->serverIP->text();
			QString port = ui->port->text();
			int fps = ui->fps->text().toInt();
			// start reading
			stop_ = false;
			listen_thread_.reset(new boost::thread(&MainWindow::listen,this,serverIP, port, fps));
		}
		catch(std::exception &e) {
			QMessageBox::critical(this,"Error",(std::string("Could not interface with Pupil Pro ")+=e.what()).c_str(),QMessageBox::Ok);
			return;
		}

		// done, all successful
		ui->linkButton->setText("Unlink");
	}
}

MainWindow::MainWindow(QWidget *parent, const std::string &config_file) : QMainWindow(parent), ui(new Ui::MainWindow) {


	ui->setupUi(this);

	// parse startup config file
	load_config(config_file);
	ui->connectionStatus->setText("not connected");

	// make GUI connections
	QObject::connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
	QObject::connect(ui->linkButton, SIGNAL(clicked()), this, SLOT(link_pupilpro()));
	QObject::connect(this, SIGNAL(sendMessage(QString)), this, SLOT(statusMessage(QString)));
	QObject::connect(ui->actionLoad_Configuration, SIGNAL(triggered()), this, SLOT(load_config_dialog()));
	QObject::connect(ui->actionSave_Configuration, SIGNAL(triggered()), this, SLOT(save_config_dialog()));
	

}
MainWindow::~MainWindow() {
	//listen_thread_->interrupt();
	if(listen_thread_)
	{
				
		listen_thread_->interrupt();
		listen_thread_->join();
	}

	delete ui;
}
