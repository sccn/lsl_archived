#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

// TODO: figure out the correct units of the various channels and ensure that we can in fact sample at the desired rate (maybe make that switchable)

MainWindow::MainWindow(QWidget *parent, const std::string &config_file) :
QMainWindow(parent),
ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	// parse startup config file
	load_config(config_file);

	// make GUI connections
	QObject::connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
	QObject::connect(ui->linkButton, SIGNAL(clicked()), this, SLOT(link_tobii()));
	QObject::connect(ui->actionLoad_Configuration, SIGNAL(triggered()), this, SLOT(load_config_dialog()));
	QObject::connect(ui->actionSave_Configuration, SIGNAL(triggered()), this, SLOT(save_config_dialog()));
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

void MainWindow::closeEvent(QCloseEvent *ev) {
	if (outlet)
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
		ui->serverAddress->setText(pt.get<std::string>("server.address","127.0.0.1").c_str());
		ui->serverPort->setValue(pt.get<int>("server.port",SERVER_PORT_NUMBER));
		ui->calibrationFile->setText(pt.get<std::string>("calibration.file","default.cd").c_str());
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
		pt.put("server.address",ui->serverAddress->text().toStdString());
		pt.put("server.port",ui->serverPort->value());
		pt.put("calibration.file",ui->calibrationFile->text().toStdString());
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

// start/stop the Tobii connection
void MainWindow::link_tobii() {
	if (outlet) {
		// === perform unlink action ===
		try {
			Tet_Stop();
			Tet_Disconnect();
			outlet.reset();
		} catch(std::exception &e) {
			QMessageBox::critical(this,"Error",(std::string("Could not stop the background processing: ")+=e.what()).c_str(),QMessageBox::Ok);
			return;
		}

		// indicate that we are now successfully unlinked
		ui->linkButton->setText("Link");
	} else {
		// === perform link action ===
		bool connected = false;
		char errbuf[16384] = {0};
		try {
			// get the UI parameters...
			int serverPort = ui->serverPort->value();
			std::string serverAddress = ui->serverAddress->text().toStdString();
			std::string calibrationFile = ui->calibrationFile->text().toStdString();

			// try to connect
			if (Tet_Connect(const_cast<char*>(serverAddress.c_str()),serverPort,TET_SYNC_LOCAL) != TET_NO_ERROR)
				throw std::runtime_error("Could not connect to Tobii eye tracker: " + std::string(Tet_GetLastErrorAsText(errbuf)));
			connected = true;

			// load calibration file
			if (Tet_CalibLoadFromFile(const_cast<char*>(calibrationFile.c_str())) != TET_NO_ERROR)
				throw std::runtime_error("Could not load calibration file:" + std::string(Tet_GetLastErrorAsText(errbuf)));

			// start tracking
			if (Tet_Start(&MainWindow::callback,this,20) != TET_NO_ERROR)
				throw std::runtime_error("Could not start tracking:" + std::string(Tet_GetLastErrorAsText(errbuf)));

			// create the streaminfo
			lsl::stream_info info("Tobii","Gaze",14,0,lsl::cf_float32,"Tobii_" + serverAddress + "_tjtyjuff");

			// append some meta-data
			lsl::xml_element channels = info.desc().append_child("channels");
			channels.append_child("channel")
				.append_child_value("label","Screen_X_left")
				.append_child_value("eye","left")
				.append_child_value("type","ScreenX");
			channels.append_child("channel")
				.append_child_value("label","Screen_Y_left")
				.append_child_value("eye","left")
				.append_child_value("type","ScreenY");
			channels.append_child("channel")
				.append_child_value("label","Pupil_X_left")
				.append_child_value("eye","left")
				.append_child_value("type","PupilX");
			channels.append_child("channel")
				.append_child_value("label","Pupil_Y_left")
				.append_child_value("eye","left")
				.append_child_value("type","PupilY");
			channels.append_child("channel")
				.append_child_value("label","Diameter_left")
				.append_child_value("eye","left")
				.append_child_value("type","Diameter");
			channels.append_child("channel")
				.append_child_value("label","Distance_left")
				.append_child_value("eye","left")
				.append_child_value("type","Distance");
			channels.append_child("channel")
				.append_child_value("label","Confidence_left")
				.append_child_value("eye","left")
				.append_child_value("type","Confidence");

			channels.append_child("channel")
				.append_child_value("label","Screen_X_right")
				.append_child_value("eye","right")
				.append_child_value("type","ScreenX");
			channels.append_child("channel")
				.append_child_value("label","Screen_Y_right")
				.append_child_value("eye","right")
				.append_child_value("type","ScreenY");
			channels.append_child("channel")
				.append_child_value("label","Pupil_X_right")
				.append_child_value("eye","right")
				.append_child_value("type","PupilX");
			channels.append_child("channel")
				.append_child_value("label","Pupil_Y_right")
				.append_child_value("eye","right")
				.append_child_value("type","PupilY");
			channels.append_child("channel")
				.append_child_value("label","Diameter_right")
				.append_child_value("eye","right")
				.append_child_value("type","Diameter");
			channels.append_child("channel")
				.append_child_value("label","Distance_right")
				.append_child_value("eye","right")
				.append_child_value("type","Distance");
			channels.append_child("channel")
				.append_child_value("label","Confidence_right")
				.append_child_value("eye","right")
				.append_child_value("type","Confidence");

			info.desc().append_child("acquisition")
				.append_child_value("manufacturer","Tobii");

			// make a new outlet
			outlet.reset(new lsl::stream_outlet(info));
		}
		catch(std::exception &e) {
			QMessageBox::critical(this,"Error",(std::string("Could not initialize the Tobii interface: ")+=e.what()).c_str(),QMessageBox::Ok);
			if (connected)
				Tet_Disconnect();
			return;
		}

		// done, all successful
		ui->linkButton->setText("Unlink");
	}
}

void MainWindow::handle_packet(ETet_CallbackReason reason, void *pData) {
	if (reason == TET_CALLBACK_GAZE_DATA && outlet) {
		_STet_GazeData *d = reinterpret_cast<_STet_GazeData*>(pData);
		// determine when the sample was collected in LSL time
		_STimeStamp now; TT_GetLocalTimeStamp(&now);
		double sample_age = (now.second + now.microsecond/1000000.0) - (d->timestamp_sec + d->timestamp_microsec/1000000.0);
		double collection_time = lsl::local_clock() - sample_age;

		// construct the sample
		float sample[14] = {d->x_gazepos_lefteye, d->y_gazepos_lefteye, d->x_camerapos_lefteye, d->y_camerapos_lefteye, 
			d->diameter_pupil_lefteye, d->distance_lefteye, d->validity_lefteye, d->x_gazepos_righteye, d->y_gazepos_righteye, 
			d->x_camerapos_righteye, d->y_camerapos_righteye, d->diameter_pupil_righteye, d->distance_righteye, d->validity_righteye};

		// push it into LSL
		outlet->push_sample(sample,collection_time);
	}
}


void MainWindow::callback(ETet_CallbackReason reason, void *pData, void *pApplicationData) {
	((MainWindow*)pApplicationData)->handle_packet(reason,pData);
}

MainWindow::~MainWindow() {
	delete ui;
}

