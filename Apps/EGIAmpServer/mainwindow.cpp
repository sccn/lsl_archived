#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

// hard-coded data about EGI packets
const int block_header_bytes = 16;	// number of header bytes per block
const int sample_header_bytes = 32;	// number of header bytes per sample

// number of samples per chunk sent into LSL (has nothing to do with EGI)
const int samples_per_chunk = 32;

MainWindow::MainWindow(QWidget *parent, const std::string &config_file) :
QMainWindow(parent),
ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	// parse startup config file
	load_config(config_file);

	// make GUI connections
	QObject::connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
	QObject::connect(ui->linkButton, SIGNAL(clicked()), this, SLOT(link_ampserver()));
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
		ui->serverAddress->setText(pt.get<std::string>("ampserver.address","172.16.2.249").c_str());
		ui->commandPort->setValue(pt.get<int>("ampserver.commandport",9877));
		ui->notificationPort->setValue(pt.get<int>("ampserver.notificationport",9878));
		ui->dataPort->setValue(pt.get<int>("ampserver.dataport",9879));
		ui->amplifierId->setValue(pt.get<int>("settings.amplifierid",0));
		ui->samplingRate->setValue(pt.get<int>("settings.samplingrate",1000));
		ui->channelCount->setValue(pt.get<int>("settings.channelcount",280));
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
		pt.put("ampserver.address",ui->serverAddress->text().toStdString());
		pt.put("ampserver.commandport",ui->commandPort->value());
		pt.put("ampserver.notificationport",ui->notificationPort->value());
		pt.put("ampserver.dataport",ui->dataPort->value());
		pt.put("settings.amplifierid",ui->amplifierId->value());
		pt.put("settings.samplingrate",ui->samplingRate->value());
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


// start/stop the ampserver connection
void MainWindow::link_ampserver() {
	if (reader_thread_) {
		// === perform unlink action ===
		try {
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

		// get the UI parameters...
		std::string address = ui->serverAddress->text().toStdString().c_str();
		int commandPort = ui->commandPort->value();
		int notificationPort = ui->notificationPort->value();
		int dataPort = ui->dataPort->value();
		int amplifierId = ui->amplifierId->value();
		int samplingRate = ui->samplingRate->value();
		int nChannels = ui->channelCount->value();

		bool connected = false;
		char response[4096];
		try {
			// create the connections
			commandStream_.reset(new socket_stream());
			commandStream_->exceptions(std::ios::eofbit | std::ios::failbit | std::ios::badbit);
			commandStream_->expires_from_now(boost::posix_time::seconds(2));
			commandStream_->connect(ip::tcp::endpoint(ip::address::from_string(address),commandPort));

			commandStream_->rdbuf()->set_option(ip::tcp::no_delay(true));
			commandStream_->expires_from_now(boost::posix_time::minutes(60*24*365));
			notificationStream_.reset(new socket_stream(ip::tcp::endpoint(ip::address::from_string(address),notificationPort)));
			notificationStream_->rdbuf()->set_option(ip::tcp::no_delay(true));
			dataStream_.reset(new socket_stream(ip::tcp::endpoint(ip::address::from_string(address),dataPort)));
			dataStream_->rdbuf()->set_option(ip::tcp::no_delay(true));

			// turn on the amp (note: maybe this should actually address the desired amp to work as intended)
			(*commandStream_) << "(sendCommand cmd_SetPower 0 0 1)\n" << std::flush;
			commandStream_->getline(response,sizeof(response));

			// wait for 3 seconds
			boost::this_thread::sleep(boost::posix_time::seconds(3));

			// start the desired amplifier
			(*commandStream_) << "(sendCommand cmd_Start " << amplifierId << " 0 0)\n" << std::flush;
			commandStream_->getline(response,sizeof(response));
			connected = true;

			// set to default acquisition mode (note: this should almost surely come before the start call...)
			(*commandStream_) << "(sendCommand cmd_DefaultAcquisitionState 0 0 0)\n" << std::flush;
			commandStream_->getline(response,sizeof(response));

			// send the listen message
			(*dataStream_) << "(sendCommand cmd_ListenToAmp " << amplifierId << " 0 0)\n" << std::flush;

			// wait for another 3 seconds
			boost::this_thread::sleep(boost::posix_time::seconds(3));

			// start reading the data
			reader_thread_.reset(new boost::thread(&MainWindow::read_thread,this,address,amplifierId,samplingRate,nChannels));
		}
		catch(std::ios_base::failure &) {
			QMessageBox::critical(this,"Error","Could not initialize the AmpServer interface due to a connection error. Please make sure that the AmpServer is turned on, your network settings are correct, and you have an SDK license for it.",QMessageBox::Ok);
			if (connected)
				halt_ampserver(amplifierId);
			return;
		}
		catch(std::exception &e) {
			QMessageBox::critical(this,"Error",(std::string("Could not initialize the AmpServer interface: ")+=e.what()).c_str(),QMessageBox::Ok);
			if (connected)
				halt_ampserver(amplifierId);
			return;
		}

		// done, all successful
		ui->linkButton->setText("Unlink");
	}
}

// background data reader thread
void MainWindow::read_thread(const std::string &address, int amplifierId, int samplingRate, int nChannels) {
	try {
		// create streaminfo
		std::string streamname = "EGI NetAmp " + boost::lexical_cast<std::string>(amplifierId);
		lsl::stream_info info(streamname,"EEG",nChannels,samplingRate,lsl::cf_float32,streamname + "at_" + address);

		// append some meta-data
		info.desc().append_child("acquisition")
			.append_child_value("manufacturer","EGI")
			.append_child_value("model","NetAmp")
			.append_child_value("application","AmpServer")
			.append_child_value("precision","24");

		// make a new outlet (we transmit at least every samples_per_chunk samples)
		lsl::stream_outlet outlet(info,samples_per_chunk);

		// reserve memory
		std::vector<float> sample(nChannels);

		// enter transmission loop
		unsigned bytes = 0;
		while (dataStream_->good()) {
			// read the chunk header
			boost::uint32_t ampIdA,ampIdB;
			boost::uint32_t lengthA,lengthB;
			// 64-bit ampID indicator (ignored by us)
			dataStream_->read((char*)&ampIdA,4); ampIdA = ntohl(ampIdA);
			dataStream_->read((char*)&ampIdB,4); ampIdB = ntohl(ampIdB);
			// 64-bit block length value (in bytes)
			dataStream_->read((char*)&lengthA,4); lengthA = ntohl(lengthA);
			dataStream_->read((char*)&lengthB,4); lengthB = ntohl(lengthB);

			// determine length in bytes; note that the byte shuffling in the 
			// sample code is note quite right, so we're a bit more tolerant here
			if (lengthA && lengthB && (lengthA!=lengthB))
				// length is larger than 4GB... we can basically read forever
				bytes = std::numeric_limits<unsigned>::max();
			else
				// pick the one that's nonzero
				bytes = lengthA ? lengthA : lengthB;

			// read and transmit the chunk sample-by-sample
			int nSamples = bytes / (sample_header_bytes + nChannels*sizeof(float));
			for (int s=0;s<nSamples && dataStream_->good();s++) {
				// skip the sample header
				dataStream_->ignore(sample_header_bytes);
				// read the data into the sample
				dataStream_->read((char*)&sample[0],nChannels*sizeof(float));
				// do endianness conversion
				for (int c=0;c<nSamples;c++)
					*((boost::uint32_t*)&sample[c]) = ntohl(*((boost::uint32_t*)&sample[c]));
				// push it into LSL
				outlet.push_sample(sample);
			}
		}
	}
	catch(std::exception &) { }
	halt_ampserver(amplifierId);
}

void MainWindow::halt_ampserver(int id) {
	try {
		(*commandStream_) << "(sendCommand cmd_Stop " << id << " 0 0)\n" << std::flush;
	} catch(...) { }
	commandStream_.reset();
	notificationStream_.reset();
	dataStream_.reset();
}

MainWindow::~MainWindow() {
	delete ui;
}
