#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

const int nSamples = 25;

MainWindow::MainWindow(QWidget *parent, const std::string &config_file) :
QMainWindow(parent),
ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	// parse startup config file
	load_config(config_file);

	// make GUI connections
	QObject::connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
	QObject::connect(ui->linkButton, SIGNAL(clicked()), this, SLOT(link_enobio()));
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
		ui->channelCount->setValue(pt.get<int>("settings.channelcount",4));
		ui->samplingRate->setValue(pt.get<int>("settings.samplingrate",250));
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
		pt.put("settings.channelcount",ui->channelCount->value());
		pt.put("settings.samplingrate",ui->samplingRate->value());
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


// start/stop the Enobio connection
void MainWindow::link_enobio() {
	if (reader_thread_) {

		// === perform unlink action ===
		try {
			stop_ = true;
			reader_thread_->interrupt();
			reader_thread_->join();
			reader_thread_.reset();
			CEnobioCaptureStop(amp);
			CEnobioClose(amp);
		} catch(std::exception &e) {
			QMessageBox::critical(this,"Error",(std::string("Could not stop the background processing: ")+=e.what()).c_str(),QMessageBox::Ok);
			return;
		}

		// indicate that we are now successfully unlinked
		ui->linkButton->setText("Link");
	} else {
		// === perform link action ===
		bool initialized = false;
		try {

			// get the UI parameters...
			int channelCount = ui->channelCount->value();
			int samplingRate = ui->samplingRate->value();
			if (!channelCount)
				throw std::runtime_error("You must set the correct number of channels that is provided by your device.");

			// instantiate
			amp = CEnobioCreate();
			if (!amp)
				throw std::runtime_error("Could not instantiate Amp connection. Is it plugged in?");
			
			// initialize
			if (!CEnobioInitialize(amp,nSamples))
				throw std::runtime_error("Could not initialize Amp setup.");
			initialized = true;

			// prepare and start acquisition
			CEnobioResetBuffer(amp);
			if (!CEnobioCaptureStart(amp))
				throw std::runtime_error("Could not start acquisition. Is your box set up correctly?");

			// start reading
			stop_ = false;
			reader_thread_.reset(new boost::thread(&MainWindow::read_thread,this,channelCount,samplingRate));		
		}
		catch(std::exception &e) {
			// try to get more detailed error info
			string errstr("no error info");
			if (amp) {
				try {
					int errcode = CEnobioGetLastError(amp);
					errstr = errcode ? CEnobioStrError(errcode,amp) : "no error";
				} catch(...) { 
					errstr = "could not query error";
				}
			}
			// try to shut down again
			if (initialized) {
				try {
					CEnobioClose(amp);
				} catch(...) { }
			}
			// show error
			QMessageBox::critical(this,"Error",(std::string("Error initializing the Enobio interface: ") + e.what() + std::string("  Enobio: ") + errstr).c_str(),QMessageBox::Ok);
			return;
		}

		// done, all successful
		ui->linkButton->setText("Unlink");
	}
}

// background data reader thread
void MainWindow::read_thread(int nChannels, int nRate) {
	// create streaminfo
	lsl::stream_info info("Enobio","EEG",nChannels,nRate,lsl::cf_float32,"Enobio_5y45ys");
	// append some meta-data
	info.desc().append_child("acquisition")
		.append_child_value("manufacturer","Neuroelectrics / Starlab")
		.append_child_value("model","Enobio");

	// make a new outlet
	lsl::stream_outlet outlet(info);

	// reserve memory
	std::vector<std::vector<float> > chunk(nSamples,std::vector<float>(nChannels));

	// enter transmission loop
	int *data;
	while (!stop_) {
		if (!CEnobioWaitForData(amp,&data,nSamples)) {
			for(int c=0; c<nChannels;c++)
				for(int s=0; s<nSamples;s++)
					chunk[s][c] = data[c+ nChannels*s];
			CEnobioResetBuffer(amp);
			outlet.push_chunk(chunk);
		} else {
			QMessageBox::critical(this,"Error","Error during Enobio transmission. Please re-start.",QMessageBox::Ok);
			return;
		}
	}
}

MainWindow::~MainWindow() {
	delete ui;
}
