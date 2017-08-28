#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <cmath>

const int samples_per_chunk = 4;				// the chunk granularity at which we transmit data into LSL
const double value_scale = 1000000.0/8388607.0;	// rescaling from 24-bit signed integers to microvolts

MainWindow::MainWindow(QWidget *parent, const std::string &config_file) :
QMainWindow(parent),
ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	// parse startup config file
	load_config(config_file);

	// make GUI connections
	QObject::connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
	QObject::connect(ui->linkButton, SIGNAL(clicked()), this, SLOT(link()));
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
		ui->samplingRate->setValue(pt.get<int>("settings.samplingrate",256));
		ui->channelCount->setValue(pt.get<int>("settings.channelcount",16));
		ui->protocolVersion->setCurrentIndex(pt.get<int>("settings.protocolversion",1));
		ui->bitDepth->setCurrentIndex(pt.get<int>("settings.bitdepth",1));
		ui->ampGain->setCurrentIndex(pt.get<int>("settings.ampgain",1));
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
		pt.put("settings.protocolversion",ui->protocolVersion->currentIndex());
		pt.put("settings.bitdepth",ui->bitDepth->currentIndex());
		pt.put("settings.ampgain",ui->ampGain->currentIndex());
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


// start/stop the MINDO connection
void MainWindow::link() {
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
			int protocolVersion = ui->protocolVersion->currentIndex();
			int bitDepth = ui->bitDepth->currentIndex();
			int ampGain = ui->ampGain->currentIndex();
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
			reader_thread_.reset(new boost::thread(&MainWindow::read_thread,this,hPort,comPort,samplingRate,channelCount,protocolVersion,bitDepth,ampGain,channelLabels));
		}
		catch(std::exception &e) {
			if (hPort != INVALID_HANDLE_VALUE)
				CloseHandle(hPort);
			QMessageBox::critical(this,"Error",(std::string("Could not initialize the MINDO interface: ")+=e.what()).c_str(),QMessageBox::Ok);
			return;
		}

		// done, all successful
		ui->linkButton->setText("Unlink");
	}
}

unsigned log2(unsigned x) { return (int)(std::log10((double)x) / std::log10(2.0)); }

// background data reader thread
void MainWindow::read_thread(HANDLE hPort, int comPort, int samplingRate, int channelCount, int protocolVersion, int bitDepth, int ampGain, std::vector<std::string> channelLabels) {
	try {

		// create streaminfo
		lsl::stream_info info("MINDO:COM" + boost::lexical_cast<std::string>(comPort),"EEG",channelCount,samplingRate,lsl::cf_float32,"MINDO_C" + boost::lexical_cast<std::string>(channelCount) + boost::lexical_cast<std::string>(comPort));
		// append some meta-data
		lsl::xml_element channels = info.desc().append_child("channels");
		for (int k=0;k<channelLabels.size();k++) {
			channels.append_child("channel")
				.append_child_value("label",channelLabels[k].c_str())
				.append_child_value("type","EEG")
				.append_child_value("unit","microvolts");
		}
		info.desc().append_child("acquisition")
			.append_child_value("manufacturer","MINDO");

		// make a new outlet
		lsl::stream_outlet outlet(info,samples_per_chunk);

		// reserve memory
		std::vector<float> sample(channelCount);

		int leadOffBytes = max(2,std::ceil(channelCount/8.0)); // the number of lead-off bytes depends on the #channels

		DWORD dwBytesWritten = 0;
		if (protocolVersion >= 1) {
			// send startup command sequence
			unsigned char cmd_beginseq = 0xFE;
			unsigned char cmd_start = 0x07;
			unsigned char cmd_resolution_srate = (bitDepth << 4) | log2(samplingRate);
			unsigned char cmd_gain_chns = (ampGain << 4) | log2(channelCount);
			unsigned char cmd_endseq = 0xFF;
			WriteFile(hPort,(LPSTR)&cmd_beginseq,1,&dwBytesWritten,NULL);
			WriteFile(hPort,(LPSTR)&cmd_start,1,&dwBytesWritten,NULL);
			WriteFile(hPort,(LPSTR)&cmd_resolution_srate,1,&dwBytesWritten,NULL);
			WriteFile(hPort,(LPSTR)&cmd_gain_chns,1,&dwBytesWritten,NULL);
			WriteFile(hPort,(LPSTR)&cmd_endseq,1,&dwBytesWritten,NULL);
		}

		// enter transmission loop
		unsigned char temp;
		signed char stemp;
		int msb, lsb2, lsb1;
		unsigned long bytes_read;
		int k = 0;

		int curCounter = -1;
		while (!stop_) {
			temp = 0;
			if (protocolVersion < 2) {
				// scan for the sync byte
				while (!stop_ && (temp != 0xFF))
					ReadFile(hPort,&temp,1,&bytes_read,NULL);
				if (stop_)
					break;
				// skip the counter
				ReadFile(hPort,&temp,1,&bytes_read,NULL);
			}
			if (protocolVersion >= 2) {
				// new header handling code
				unsigned char bytesPerChannel = (bitDepth == 0) ? 3 : 2;
				unsigned char bytesPerSample = 2 + leadOffBytes + bytesPerChannel * channelCount;
				if (curCounter < 0) {
					// no synchronization, need to find header again
					std::vector<unsigned char> shift_buffer(bytesPerSample*16);	// this is a circular buffer that holds the last data words that make up a measurement
					// shift in bytes at the end until we get a complete measurement
					while (!stop_) {
						for (unsigned k=0;k<shift_buffer.size()-1;k++)
							shift_buffer[k] = shift_buffer[k+1];
						if (!ReadFile(hPort,&shift_buffer.back(),1,&bytes_read,NULL)) {
							QMessageBox::critical(this,"Error","Connection broke off.",QMessageBox::Ok);
							return;
						}
						// check if this is matching
						bool match = true;
						for (unsigned k=0;k<16;k++) {
							if (shift_buffer[k*bytesPerSample] != 240+k) {
								match = false;
								break;
							}
						}
						if (match)
							break;
					}
					curCounter = 0;
				}

				// read header & confirm that we're still synchronized
				ReadFile(hPort,&temp,1,&bytes_read,NULL);
				if (temp != curCounter+240) {
					// synch loss
					curCounter = -1;
					continue;
				} else
					curCounter = (curCounter+1) % 16;
				// skip the magic byte
				ReadFile(hPort,&temp,1,&bytes_read,NULL);
			}

			// get next sample
			if (protocolVersion == 0) {
				for(int c=0; c < channelCount; c++) {
					ReadFile(hPort,&temp,1,&bytes_read,NULL);
					lsb2 = temp;
					ReadFile(hPort,&temp,1,&bytes_read,NULL);
					lsb1 = temp;
					sample[c] = (double)((lsb2-64)*64 + (lsb1-64));
				}
			}
			if (protocolVersion >= 1) {
				if (bitDepth == 0) {
					// 24 bit
					for(int c=0; c < channelCount; c++) {
						ReadFile(hPort,&stemp,1,&bytes_read,NULL);
						msb = stemp;
						ReadFile(hPort,&temp,1,&bytes_read,NULL);
						lsb2 = temp;
						ReadFile(hPort,&temp,1,&bytes_read,NULL);
						lsb1 = temp;
						sample[c] = (2.4 * (double)(msb*65536 + lsb2*256 + lsb1)) * value_scale;
					}
				}
				if (bitDepth == 1) {
					// 16 bit
					for(int c=0; c < channelCount; c++) {
						ReadFile(hPort,&stemp,1,&bytes_read,NULL);
						msb = stemp;
						ReadFile(hPort,&temp,1,&bytes_read,NULL);
						lsb2 = temp;
						sample[c] = (2.4 * (double)(msb*65536 + lsb2*256)) * value_scale;
					}
				}

				// read the "lead-off" bytes
				for (int k=0;k<leadOffBytes;k++)
					ReadFile(hPort,&temp,1,&bytes_read,NULL);
			}

			// push into the outlet
			outlet.push_sample(sample);
		}
	} 
	catch(boost::thread_interrupted &e) {
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
