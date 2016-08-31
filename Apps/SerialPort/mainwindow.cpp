#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

MainWindow::MainWindow(QWidget *parent, const std::string &config_file) :
QMainWindow(parent),
ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	// parse startup config file
	load_config(config_file);

	// make GUI connections
	QObject::connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
	QObject::connect(ui->linkButton, SIGNAL(clicked()), this, SLOT(on_link()));
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
	if (reader_thread_)
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
		ui->comPort->setValue(pt.get<int>("coresettings.comport",1));
		ui->baudRate->setValue(pt.get<int>("coresettings.baudrate",57600));
		ui->samplingRate->setValue(pt.get<int>("streamsettings.samplingrate",0));
		ui->chunkSize->setValue(pt.get<int>("streamsettings.chunksize",32));
		ui->streamName->setText(pt.get<std::string>("streamsettings.streamname","SerialPort").c_str());
		ui->dataBits->setCurrentIndex(pt.get<int>("miscsettings.databits",4));
		ui->parity->setCurrentIndex(pt.get<int>("miscsettings.parity",0));
		ui->stopBits->setCurrentIndex(pt.get<int>("miscsettings.stopbits",0));
		ui->readIntervalTimeout->setValue(pt.get<int>("timeoutsettings.readintervaltimeout",500));
		ui->readTotalTimeoutConstant->setValue(pt.get<int>("timeoutsettings.readtotaltimeoutconstant",50));
		ui->readTotalTimeoutMultiplier->setValue(pt.get<int>("timeoutsettings.readtotaltimeoutmultiplier",10));
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
		pt.put("coresettings.comport",ui->comPort->value());
		pt.put("coresettings.baudrate",ui->baudRate->value());
		pt.put("streamsettings.samplingrate",ui->samplingRate->value());
		pt.put("streamsettings.chunksize",ui->chunkSize->value());
		pt.put("streamsettings.streamname",ui->streamName->text().toStdString());
		pt.put("miscsettings.databits",ui->dataBits->currentIndex());
		pt.put("miscsettings.parity",ui->parity->currentIndex());
		pt.put("miscsettings.stopbits",ui->stopBits->currentIndex());
		pt.put("timeoutsettings.readintervaltimeout",ui->readIntervalTimeout->value());
		pt.put("timeoutsettings.readtotaltimeoutconstant",ui->readTotalTimeoutConstant->value());
		pt.put("timeoutsettings.readtotaltimeoutmultiplier",ui->readTotalTimeoutMultiplier->value());
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


// start/stop the cognionics connection
void MainWindow::on_link() {
	if (reader_thread_) {
		// === perform unlink action ===
		try {
			stop_ = true;
			reader_thread_->interrupt();
			reader_thread_->join();
			reader_thread_.reset();
		} catch(std::exception &e) {
			QMessageBox::critical(this,"Error",(std::string("Could not stop the background processing: ")+=e.what()).c_str(),QMessageBox::Ok);
			return;
		}

		// indicate that we are now successfully unlinked
		ui->linkButton->setText("Link");
	} else {
		// === perform link action ===
		HANDLE hPort = NULL;

		try {
			// get the UI parameters...
			int comPort = ui->comPort->value();
			int baudRate = ui->baudRate->value();
			int samplingRate = ui->samplingRate->value();
			int chunkSize = ui->chunkSize->value();
			std::string streamName = ui->streamName->text().toStdString();
			int dataBits = ui->dataBits->currentIndex()+4;
			int parity = ui->parity->currentIndex();
			int stopBits = ui->stopBits->currentIndex();
			int readIntervalTimeout = ui->readIntervalTimeout->value();
			int readTotalTimeoutConstant = ui->readTotalTimeoutConstant->value();
			int readTotalTimeoutMultiplier = ui->readTotalTimeoutMultiplier->value();

			// try to open the serial port
			std::string fname = "\\\\.\\COM" + boost::lexical_cast<std::string>(comPort);
			hPort = CreateFileA(fname.c_str(),GENERIC_READ|GENERIC_WRITE,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
			if (hPort == INVALID_HANDLE_VALUE)
				throw std::runtime_error("Could not open serial port. Please make sure that you are using the right COM port and that the device is ready.");

			// try to set up serial port parameters
			DCB dcbSerialParams = {0};
			if (!GetCommState(hPort, &dcbSerialParams))
				QMessageBox::critical(this,"Error","Could not get COM port state.",QMessageBox::Ok);
			dcbSerialParams.BaudRate=baudRate;
			dcbSerialParams.ByteSize=dataBits;
			dcbSerialParams.StopBits=stopBits;
			dcbSerialParams.Parity=parity;
			if(!SetCommState(hPort, &dcbSerialParams))
				QMessageBox::critical(this,"Error","Could not set baud rate.",QMessageBox::Ok);

			// try to set timeouts
			COMMTIMEOUTS timeouts = {0};
			if (!GetCommTimeouts(hPort,&timeouts))
				QMessageBox::critical(this,"Error","Could not get COM port timeouts.",QMessageBox::Ok);
			timeouts.ReadIntervalTimeout = readIntervalTimeout;
			timeouts.ReadTotalTimeoutConstant = readTotalTimeoutConstant;
			timeouts.ReadTotalTimeoutMultiplier = readTotalTimeoutMultiplier;
			if (!SetCommTimeouts(hPort,&timeouts))
				QMessageBox::critical(this,"Error","Could not set COM port timeouts.",QMessageBox::Ok);

			// start reading
			stop_ = false;
			reader_thread_.reset(new boost::thread(&MainWindow::read_thread,this,hPort,comPort,baudRate,samplingRate,chunkSize,streamName));
		}
		catch(std::exception &e) {
			if (hPort != INVALID_HANDLE_VALUE)
				CloseHandle(hPort);
			QMessageBox::critical(this,"Error",(std::string("Error during initialization: ")+=e.what()).c_str(),QMessageBox::Ok);
			return;
		}

		// done, all successful
		ui->linkButton->setText("Unlink");
	}
}


// background data reader thread
void MainWindow::read_thread(HANDLE hPort, int comPort, int baudRate, int samplingRate, int chunkSize, const std::string &streamName) {
	try {

		// create streaminfo
		lsl::stream_info info(streamName,"Raw",1,samplingRate,lsl::cf_int16,std::string("SerialPort_") + streamName);
		// append some meta-data
		lsl::xml_element channels = info.desc().append_child("channels");
		channels.append_child("channel")
			.append_child_value("label","Channel1")
			.append_child_value("type","Raw")
			.append_child_value("unit","integer");
		info.desc().append_child("acquisition")
			.append_child("hardware")
				.append_child_value("com_port",boost::lexical_cast<std::string>(comPort).c_str())
				.append_child_value("baud_rate",boost::lexical_cast<std::string>(baudRate).c_str());

		// make a new outlet
		lsl::stream_outlet outlet(info,chunkSize);

		// enter transmission loop
		unsigned char byte;
		short sample;
		unsigned long bytes_read;
		while (!stop_) {
			// get a sample
			ReadFile(hPort,&byte,1,&bytes_read,NULL); sample = byte;
			// transmit it
			if (bytes_read)
				outlet.push_sample(&sample);
		}
	}
	catch(boost::thread_interrupted &) {
		// thread was interrupted: no error
	}
	catch(std::exception &e) {
		// any other error
		QMessageBox::critical(this,"Error",(std::string("Error during processing: ")+=e.what()).c_str(),QMessageBox::Ok);
	}
	CloseHandle(hPort);
}

MainWindow::~MainWindow() {
	delete ui;
}
