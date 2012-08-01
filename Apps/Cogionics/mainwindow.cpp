#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>


const int samples_per_chunk = 32;					// the chunk granularity at which we transmit data into LSL
const double value_scale = 1000000.0/4294967296.0;	// rescaling from 32-bit integers to microvolts

MainWindow::MainWindow(QWidget *parent, const std::string &config_file) :
QMainWindow(parent),
ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	// parse startup config file
	load_config(config_file);

	// make GUI connections
	QObject::connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
	QObject::connect(ui->linkButton, SIGNAL(clicked()), this, SLOT(link_cogionics()));
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
		ui->comPort->setValue(pt.get<int>("settings.comport",1));
		ui->samplingRate->setValue(pt.get<int>("settings.samplingrate",250));
		ui->channelCount->setValue(pt.get<int>("settings.channelcount",24));
		ui->channelLabels->clear();
		BOOST_FOREACH(ptree::value_type &v, pt.get_child("channels.labels"))
			ui->channelLabels->appendPlainText(v.second.data().c_str());
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
		pt.put("settings.comport",ui->comPort->value());
		pt.put("settings.samplingrate",ui->samplingRate->value());
		pt.put("settings.channelcount",ui->channelCount->value());
		std::vector<std::string> channelLabels;
		boost::algorithm::split(channelLabels,ui->channelLabels->toPlainText().toStdString(),boost::algorithm::is_any_of("\n"));
		BOOST_FOREACH(std::string &v, channelLabels)
			pt.add("channels.labels.label", v);
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


// start/stop the cogionics connection
void MainWindow::link_cogionics() {
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
			int samplingRate = ui->samplingRate->value();
			int channelCount = ui->channelCount->value();
			std::vector<std::string> channelLabels;
			boost::algorithm::split(channelLabels,ui->channelLabels->toPlainText().toStdString(),boost::algorithm::is_any_of("\n"));
			if (channelLabels.size() != channelCount)
				throw std::runtime_error("The number of channels labels does not match the channel count device setting.");

			// try to open the serial port
			std::string fname = "\\\\.\\COM" + boost::lexical_cast<std::string>(comPort);
			hPort = CreateFileA(fname.c_str(),GENERIC_READ|GENERIC_WRITE,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
			if (hPort == INVALID_HANDLE_VALUE)
				throw std::runtime_error("Could not open serial port. Please make sure that the device is paired (pairing code 0000) and that you have the right serial port number.");

			// try to set timeouts
			COMMTIMEOUTS timeouts = {0};
			timeouts.ReadIntervalTimeout = 500;
			timeouts.ReadTotalTimeoutConstant = 50;
			timeouts.ReadTotalTimeoutMultiplier = 10;
			timeouts.WriteTotalTimeoutConstant = 50;
			timeouts.WriteTotalTimeoutMultiplier = 10;
			SetCommTimeouts(hPort,&timeouts);

			// start reading
			stop_ = false;
			reader_thread_.reset(new boost::thread(&MainWindow::read_thread,this,hPort,comPort,samplingRate,channelCount,channelLabels));
		}
		catch(std::exception &e) {
			if (hPort != INVALID_HANDLE_VALUE)
				CloseHandle(hPort);
			QMessageBox::critical(this,"Error",(std::string("Could not initialize the Cogionics interface: ")+=e.what()).c_str(),QMessageBox::Ok);
			return;
		}

		// done, all successful
		ui->linkButton->setText("Unlink");
	}
}


// background data reader thread
void MainWindow::read_thread(HANDLE hPort, int comPort, int samplingRate, int channelCount, std::vector<std::string> channelLabels) {

	// create streaminfo
	lsl::stream_info info("Cogionics","EEG",channelCount,samplingRate,lsl::cf_float32,"Cogionics_C" + boost::lexical_cast<std::string>(channelCount));
	// append some meta-data
	lsl::xml_element channels = info.desc().append_child("channels");
	for (int k=0;k<channelLabels.size();k++) {
		channels.append_child("channel")
			.append_child_value("label",channelLabels[k].c_str())
			.append_child_value("type","EEG")
			.append_child_value("unit","microvolts");
	}
	info.desc().append_child("acquisition")
		.append_child_value("manufacturer","Cogionics");

	// make a new outlet
	lsl::stream_outlet outlet(info,samples_per_chunk);

	// reserve memory
	std::vector<float> sample(channelCount);

	// enter transmission loop
	unsigned char temp;
	int msb, lsb2, lsb1;
	unsigned long bytes_read;
	while (!stop_) {
		temp = 0;
		// scan for the sync byte
		while (temp != 0xFF)
			ReadFile(hPort,&temp,1,&bytes_read,NULL);

		// skip the counter
		ReadFile(hPort,&temp,1,&bytes_read,NULL);
		
		// get next sample
		for(int c=0; c < channelCount; c++) {
			ReadFile(hPort,&temp,1,&bytes_read,NULL);
			msb = temp;
			ReadFile(hPort,&temp,1,&bytes_read,NULL);
			lsb2 = temp;
			ReadFile(hPort,&temp,1,&bytes_read,NULL);
			lsb1 = temp;
			sample[c] = (double)((msb<<24) | (lsb2<<17) | (lsb1<<10)) * value_scale;
		}

		// push into the outlet
		outlet.push_sample(sample);
	}
	CloseHandle(hPort);
}

MainWindow::~MainWindow() {
	delete ui;
}
