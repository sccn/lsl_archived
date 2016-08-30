#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

// some constant device properties
const char *const name_by_type[] = {"None","X10","X24","X4"};
int channels_by_type[] = {0,10,24,4};
const int nSamples = 8;
const int nRate = 256;


MainWindow::MainWindow(QWidget *parent, const std::string &config_file) :
QMainWindow(parent),
ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	// parse startup config file
	load_config(config_file);

	// make GUI connections
	QObject::connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
	QObject::connect(ui->linkButton, SIGNAL(clicked()), this, SLOT(link_balert()));
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
		ui->useFilter->setCheckState(pt.get<bool>("settings.usefilter",true) ? Qt::Checked : Qt::Unchecked);
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
		pt.put("settings.usefilter",ui->useFilter->checkState()==Qt::Checked);
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


// start/stop the BAlert connection
void MainWindow::link_balert() {
	if (reader_thread_) {

		// === perform unlink action ===
		try {
			stop_ = true;
			reader_thread_->interrupt();
			reader_thread_->join();
			reader_thread_.reset();
			BAlertStop();
		} catch(std::exception &e) {
			QMessageBox::critical(this,"Error",(std::string("Could not stop the background processing: ")+=e.what()).c_str(),QMessageBox::Ok);
			return;
		}

		// indicate that we are now successfully unlinked
		ui->linkButton->setText("Link");
	} else {
		// === perform link action ===
		bool connected = false;
		try {

			// get the UI parameters...
			bool usefilter = ui->useFilter->checkState()==Qt::Checked;

			// query the device type
			int devicetype = BAlertGetDeviceType();
			if (!devicetype)
				throw std::runtime_error("No device found. Is it plugged in?");
			if (devicetype > 3)
				throw std::runtime_error("Unsupported device found -- please check if there is a newer version of this program available.");

			// try to connect
			if (!BAlertStart(usefilter))
				throw std::runtime_error("Could not start data acquisition.");
			connected = true;

			// start reading
			stop_ = false;
			reader_thread_.reset(new boost::thread(&MainWindow::read_thread,this,usefilter,devicetype));		
		}
		catch(std::exception &e) {
			if (connected)
				BAlertStop();
			QMessageBox::critical(this,"Error",(std::string("Could not initialize the BAlert interface: ")+=e.what()).c_str(),QMessageBox::Ok);
			return;
		}

		// done, all successful
		ui->linkButton->setText("Unlink");
	}
}

// background data reader thread
void MainWindow::read_thread(bool usefilter, int devicetype) {
	int nChannels = channels_by_type[devicetype];

	// create streaminfo
	std::string modelname = std::string("BAlert ") +  name_by_type[devicetype];
	lsl::stream_info info(modelname,"EEG",nChannels,nRate,lsl::cf_float32,modelname + "_sdetsff");
	// append some meta-data
	info.desc().append_child("amplifier").append_child("settings")
		.append_child_value("filtering", usefilter ? "Yes" : "No");
	info.desc().append_child("acquisition")
		.append_child_value("manufacturer","Advanced Brain Monitoring")
		.append_child_value("model",modelname.c_str());

	// make a new outlet
	lsl::stream_outlet outlet(info);

	// reserve memory
	float fBuffer[16384];
	std::vector<std::vector<float> > chunk(nSamples,std::vector<float>(nChannels));

	// enter transmission loop
	while (!stop_) {
		if (BAlertWaitForData(fBuffer,nSamples)) {
			int i=0;
			for(int s=0;s<nSamples;s++)
				for(int c=0;c<nChannels;c++)
					chunk[s][c] = fBuffer[i++];
			outlet.push_chunk(chunk);
		} else {
			QMessageBox::critical(this,"Error","Error during B-Alert transmission. Please re-start.",QMessageBox::Ok);
			return;
		}
	}
}

MainWindow::~MainWindow() {
	delete ui;
}
