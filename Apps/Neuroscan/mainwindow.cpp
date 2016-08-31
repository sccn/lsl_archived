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
	QObject::connect(ui->linkButton, SIGNAL(clicked()), this, SLOT(link_neuroscan()));
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
		ui->ampType->setCurrentIndex(pt.get<int>("amplifier.type",1)-1);
		ui->wirelessAddress->setText(pt.get<std::string>("amplifier.address","").c_str());
		ui->samplingRate->setCurrentIndex(pt.get<int>("settings.samplingrate",0));
		ui->channelCount->setValue(pt.get<int>("settings.channelcount",64));
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
		pt.put("amplifier.type",ui->ampType->currentIndex()+1);
		pt.put("amplifier.address",ui->wirelessAddress->text().toStdString());
		pt.put("settings.samplingrate",ui->samplingRate->currentIndex());
		pt.put("settings.channelcount",ui->channelCount->value());
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


// start/stop the Neuroscan connection
void MainWindow::link_neuroscan() {
	if (reader_thread_) {
		// === perform unlink action ===
		try {
			reader_thread_->interrupt();
			reader_thread_->join();
			reader_thread_.reset();

			NSDDisconnect();
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
			int ampType = ui->ampType->currentIndex()+1;
			std::string address = ui->wirelessAddress->text().toStdString().c_str();
			int nRate = boost::lexical_cast<int>(ui->samplingRate->itemText(ui->samplingRate->currentIndex()).toStdString());
			int nChannels = ui->channelCount->value();
			std::string ampCaps;

			// connect
			if (!NSDConnect(ampType,address.c_str())) {
				connected = true;
				throw std::runtime_error("Could not connect to amplifier. Is it turned on and does it have a valid license?");
			}
			
			// query the amplifier's capabilities
			try {
				char buf[65536];
				int len = NSDGetCapabilities(buf);
				if (!len)
					throw std::runtime_error("Could not query amplifier capabilities.");
				ampCaps.assign(buf,buf+len);
			}
			catch(std::exception &e) {
				QMessageBox::information(this,"Error",(std::string("Could not query the amplifier's capabiliies: ")+=e.what()).c_str(),QMessageBox::Ok);
			}

			// initialize amplifier
			if (!NSDCreateConfiguration(nChannels,nRate))
				throw std::runtime_error("Could not query initialize amplifier configuration.");

			// query block size
			int nBlocks = NSDGetBlocksPerSecond();
			if (!nBlocks)
				throw std::runtime_error("Could not query initialize amplifier configuration.");

			// start reading
			reader_thread_.reset(new boost::thread(&MainWindow::read_thread,this,ampType,address,nRate,nChannels,nBlocks));
		
		}
		catch(std::exception &e) {
			if (connected)
				NSDDisconnect();
			QMessageBox::critical(this,"Error",(std::string("Could not initialize the Neuroscan interface: ")+=e.what()).c_str(),QMessageBox::Ok);
			return;
		}

		// done, all successful
		ui->linkButton->setText("Unlink");
	}
}


// background data reader thread
void MainWindow::read_thread(int ampType, std::string address, int nRate, int nChannels, int nBlocks) {
	// calculate block parameters
	int nSamplesPerBlock = nRate/nBlocks;
	int nValuesPerBlock = nChannels * nSamplesPerBlock + 1;	// includes 1 dummy element at the beginning

	// create streaminfo
	lsl::stream_info info("Neuroscan","EEG",nChannels,nRate,lsl::cf_float32,"Neuroscan_sdetsff");
	// append some meta-data
	info.desc().append_child("acquisition")
		.append_child_value("manufacturer","Neuroscan")
		.append_child_value("model",ampType==1 ? "Synamp II" : "Synamp Wireless");

	// make a new outlet
	lsl::stream_outlet outlet(info);

	// reserve memory
	std::vector<float> data(nValuesPerBlock);
	std::vector<std::vector<float> > chunk(nSamplesPerBlock,std::vector<float>(nChannels));

	// enter transmission loop
	while (true) {
		if (NSDGetData(&data[0])) {
			for (int c=0;c<nChannels;c++)
				for (int s=0;s<nSamplesPerBlock;s++)
					chunk[s][c] = data[1 + s + c*nSamplesPerBlock];
			outlet.push_chunk(chunk);
		} else
			boost::this_thread::yield();
	}
}

MainWindow::~MainWindow() {
	delete ui;
}
