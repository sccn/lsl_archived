#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <fstream> 
#include "wrapper.cc"

using namespace eemagine::sdk;

MainWindow::MainWindow(QWidget *parent, const std::string &config_file, const bool linkOnStart) : QMainWindow(parent)
{
	reader = NULL;
	ui.setupUi(this);

	// make GUI connections
	QObject::connect(ui.actionLoad_Configuration, SIGNAL(triggered()), this, SLOT(load_config_dialog()));
	QObject::connect(ui.actionSave_Configuration, SIGNAL(triggered()), this, SLOT(save_config_dialog()));
	QObject::connect(ui.actionQuit, SIGNAL(triggered()), this, SLOT(close()));
	QObject::connect(ui.linkButton, SIGNAL(clicked()), this, SLOT(link()));
	if(linkOnStart) {
		link();
	}
}

void MainWindow::load_config_dialog() {
	QString sel = QFileDialog::getOpenFileName(this, "Load Configuration File", "", "Configuration Files (*.cfg)");
	if (!sel.isEmpty())
		load_config(sel.toStdString());
}

void MainWindow::save_config_dialog() {
	QString sel = QFileDialog::getSaveFileName(this, "Save Configuration File", "", "Configuration Files (*.cfg)");
	if (!sel.isEmpty())
		save_config(sel.toStdString());
}

void MainWindow::closeEvent(QCloseEvent *ev) {
	if (reader != NULL) {
		ev->ignore();
	}
}

void MainWindow::load_config(const std::string &filename) {
	using boost::property_tree::ptree;
	ptree pt;

	// parse file
	try {
		read_xml(filename, pt);
	}
	catch (std::exception &e) {
		QMessageBox::information(this, "Error", (std::string("Cannot read config file: ") += e.what()).c_str(), QMessageBox::Ok);
		return;
	}

	// get config values
	try {
		ui.samplingRate->setCurrentIndex(pt.get<int>("settings.samplingrate", 3));
	}
	catch (std::exception &) {
		QMessageBox::information(this, "Error in Config File", "Could not read out config parameters.", QMessageBox::Ok);
		return;
	}
}

void MainWindow::save_config(const std::string &filename) {
	using boost::property_tree::ptree;
	ptree pt;

	// transfer UI content into property tree
	try {
		pt.put("settings.samplingrate", ui.samplingRate->currentIndex());
	}
	catch (std::exception &e) {
		QMessageBox::critical(this, "Error", (std::string("Could not prepare settings for saving: ") += e.what()).c_str(), QMessageBox::Ok);
	}

	// write to disk
	try {
		write_xml(filename, pt);
	}
	catch (std::exception &e) {
		QMessageBox::critical(this, "Error", (std::string("Could not write to config file: ") += e.what()).c_str(), QMessageBox::Ok);
	}
}


void MainWindow::link() {
	if (reader != NULL) {
		reader->setStop(true);
		ui.linkButton->setText("Link");
	}
	else {
		// === perform link action ===
		try {
			// get the UI parameters...
			QString sr = ui.samplingRate->currentText();
			int samplingRate = sr.toInt();// boost::lexical_cast<int>(sr);
			
			thread = new QThread;
			reader = new Reader();

			reader->setParams(samplingRate);

			reader->moveToThread(thread);
			connect(thread, SIGNAL(started()), reader, SLOT(read()));
			connect(reader, SIGNAL(finished()), thread, SLOT(quit()));
			connect(thread, SIGNAL(finished()), this, SLOT(threadFinished()));
			connect(reader, SIGNAL(finished()), reader, SLOT(deleteLater()));
			connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
			connect(reader, SIGNAL(timeout()), this, SLOT(threadTimeout()));
			connect(reader, SIGNAL(connectionLost()), this, SLOT(connectionLost()));
			connect(reader, SIGNAL(ampNotFound()), this, SLOT(ampNotFound()));
			thread->start();
		}
		catch (std::exception &e) {
			// try to decode the error message
			std::string msg = "Could not query driver message because the device is not open";
			QMessageBox::critical(this, "Error", ("Could not initialize the eego Sport interface: " + (e.what() + (" (driver message: " + msg + ")"))).c_str(), QMessageBox::Ok);
			return;
		}

		// done, all successful
		ui.linkButton->setText("Unlink");
	}
}

void MainWindow::threadFinished() {
	reader = NULL;
	delete thread;
	thread = NULL;
	ui.linkButton->setText("Link");
}

void MainWindow::threadTimeout() {
	QMessageBox::critical(this, "Error", (std::string("Error: eego Sport Conncetion timed out")).c_str(), QMessageBox::Ok);
}

void MainWindow::ampNotFound() {
	QMessageBox::critical(this, "Error", (std::string("Error: Amp not found or license file not present.Please connect the amplifier and make sure that the amp is turned on and a license file is present in 'My Documents/eego/")).c_str(), QMessageBox::Ok);
}

void MainWindow::connectionLost() {
	QMessageBox::critical(this, "Error", (std::string("Error: Amp connection lost")).c_str(), QMessageBox::Ok);
}

// --- CONSTRUCTOR ---
Reader::Reader() {
	stop = false;
	// you could copy data from constructor arguments to internal variables here.
}

// --- DECONSTRUCTOR ---
Reader::~Reader() {
	// free resources
}

void Reader::setParams(int samplingRate) {
	this->samplingRate = samplingRate;
}

// --- PROCESS ---
// Start processing data.
void Reader::read() {
	bool ampFound = true;
	try {
		factory fact;
		amp = fact.getAmplifier();
		eegStream = amp->OpenEegStream(samplingRate);

		std::vector<channel> channelList = eegStream->getChannelList();

		// create data streaminfo and append some meta-data
		lsl::stream_info data_info("eegoSports " + amp->getSerialNumber(), "EEG", channelList.size() - 2, samplingRate, lsl::cf_float32, "eegoSports_" + amp->getSerialNumber());
		lsl::xml_element channels = data_info.desc().append_child("channels");

		for (int k = 0; k < channelList.size() - 2; k++) {
			channels.append_child("channel")
				.append_child_value("label", "Ch" + k)
				.append_child_value("type", "EEG")
				.append_child_value("unit", "microvolts");
		}
		data_info.desc().append_child("acquisition")
			.append_child_value("manufacturer", "antneuro")
			.append_child_value("serial_number", boost::lexical_cast<std::string>(amp->getSerialNumber()).c_str());

		// make a data outlet
		lsl::stream_outlet data_outlet(data_info);

		// create marker streaminfo and outlet
		lsl::stream_info marker_info("eegoSports-" + amp->getSerialNumber() + "_markers" + "Markers", "Markers", 1, 0, lsl::cf_string, "eegoSports_" + boost::lexical_cast<std::string>(amp->getSerialNumber()) + "_markers");
		lsl::stream_outlet marker_outlet(marker_info);

		std::vector<channel> eegChannelList = eegStream->getChannelList();

		int timeout_count = 0;
		while (!stop) {
			buffer buffer = eegStream->getData();
			int channelCount = buffer.getChannelCount();
			int sampleCount = buffer.size() / channelCount;
			std::vector<std::vector<float>> send_buffer(sampleCount, std::vector<float>(channelCount - 2));
			for (int s = 0; s < sampleCount; s++) {
				for (int c = 0; c < channelCount - 2; c++) {
					send_buffer[s][c] = buffer.getSample(c, s);
				}
			}
			double now = lsl::local_clock();
			data_outlet.push_chunk(send_buffer, now);

			int last_mrk = 0;
			for (int s = 0; s < sampleCount; s++) {
				//if (int mrk = src_buffer[channelCount + s*(channelCount + 1)]) {
				int mrk = buffer.getSample(channelCount - 2, s);
				if (mrk != last_mrk) {
					std::string mrk_string = boost::lexical_cast<std::string>(mrk);
					marker_outlet.push_sample(&mrk_string, now + (s + 1 - sampleCount) / samplingRate);
					last_mrk = mrk;
				}
			}
		}
	}
	catch (exceptions::notFound) {
		ampFound = false;
		emit ampNotFound();
	}
	catch (exceptions::notConnected) {
		emit connectionLost();
	}
	catch (std::exception &e) {
		int i = 0;
	}
	if (ampFound) {
		delete eegStream;
		delete amp;
	}
	emit finished();
}

MainWindow::~MainWindow() {
}