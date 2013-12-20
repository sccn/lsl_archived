#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

const int usb_packet_size = 1024;

MainWindow::MainWindow(QWidget *parent, const std::string &config_file): QMainWindow(parent),ui(new Ui::MainWindow),hDevice(NULL),hEvent(NULL),pOverlapped(NULL)
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
		ui->deviceNumber->setText(pt.get<std::string>("settings.devicenumber","(search)").c_str());
		ui->channelCount->setValue(pt.get<int>("settings.channelcount",16));
		ui->chunkSize->setValue(pt.get<int>("settings.chunksize",16));
		ui->samplingRate->setCurrentIndex(pt.get<int>("settings.samplingrate",3));
		ui->isSlave->setCheckState(pt.get<bool>("settings.isslave",false) ? Qt::Checked : Qt::Unchecked);
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
		pt.put("settings.devicenumber",ui->deviceNumber->text().toStdString());
		pt.put("settings.channelcount",ui->channelCount->value());
		pt.put("settings.chunksize",ui->chunkSize->value());
		pt.put("settings.samplingrate",ui->samplingRate->currentIndex());
		pt.put("settings.isslave",ui->isSlave->checkState()==Qt::Checked);
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


// start/stop the gHIamp connection
void MainWindow::link() {
	if (reader_thread_) {
		// === perform unlink action ===
		try {
			stop_ = true;
			reader_thread_->interrupt();
			reader_thread_->join();
			reader_thread_.reset();
			if (hDevice>0) {
				GT_Stop(hDevice);
				GT_ResetTransfer(hDevice);
				GT_CloseDevice(&hDevice);
			}
			if (hEvent) {
				CloseHandle(hEvent);
				hEvent = NULL;
			}
			if (pOverlapped) {
				delete pOverlapped;
				pOverlapped = NULL;
			}
		} catch(std::exception &e) {
			QMessageBox::critical(this,"Error",(std::string("Could not stop the background processing: ")+=e.what()).c_str(),QMessageBox::Ok);
			return;
		}

		// indicate that we are now successfully unlinked
		ui->linkButton->setText("Link");
	} else {
		// === perform link action ===

		try {
			// get the UI parameters...
			std::string deviceNumber = ui->deviceNumber->text().toStdString();
			int channelCount = ui->channelCount->value();
			int chunkSize = ui->chunkSize->value();
			int samplingRate = boost::lexical_cast<int>(ui->samplingRate->currentText().toStdString());
			bool isSlave = ui->isSlave->checkState()==Qt::Checked;
			std::vector<std::string> channelLabels;
			boost::algorithm::split(channelLabels,ui->channelLabels->toPlainText().toStdString(),boost::algorithm::is_any_of("\n"));
			if (channelLabels.size() != channelCount)
				throw std::runtime_error("The number of channels labels does not match the channel count device setting.");

			// get the driver version
			float driver_version = GT_GetDriverVersion();
			std::string driver_version_str = boost::lexical_cast<std::string>(driver_version);

			// try to open the device
			if (deviceNumber == "(search)") {
				for (int k=0;k<128;k++)
					if (hDevice = GT_OpenDevice(k)) {
						deviceNumber = boost::lexical_cast<std::string>(k);
						break;
					}
				if (!hDevice)
					throw std::runtime_error("Found no device that could be opened. Please make sure that the device is plugged in, turned on, the driver is installed correctly, and that the version of your driver DLL (gHIamp.dll) (currently " + driver_version_str + ") matches that of your amplifier.");
			} else {
				if (deviceNumber.size()<=2) {
					hDevice = GT_OpenDevice(boost::lexical_cast<int>(deviceNumber));
					if (!hDevice)
						throw std::runtime_error("A device with that number could not be opened. Please make sure that the device is plugged in, turned on, and the driver is installed correctly, and that the version of your driver DLL (gHIamp.dll) (currently " + driver_version_str + ") matches that of your amplifier. You can also pass in the serial number of your amplifier instead of a device number.");
				} else {
					hDevice = GT_OpenDeviceEx((LPSTR)deviceNumber.c_str());
					if (!hDevice)
					throw std::runtime_error("Could not open device. Please make sure that the device is plugged in, turned on, the driver is installed correctly, and that the version of your driver DLL (gHIamp.dll) (currently " + driver_version_str + ") matches that of your amplifier. Also check that the serial number is correct. You can also try to pass in 0 to search over all ports.");
				}
			}

			// initialize structures for async IO
			hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
			pOverlapped = new OVERLAPPED;
			memset(pOverlapped, 0, sizeof(OVERLAPPED));
			pOverlapped->hEvent = hEvent;

			// set amplifier configuration
			GT_HIAMP_CONFIGURATION conf = {0};
			conf.SampleRate = samplingRate;
			conf.BufferSize = chunkSize;
			conf.TriggerLineEnabled = true;
			conf.HoldEnabled = false;
			conf.IsSlave = isSlave;
			conf.CounterEnabled = false;
			conf.InternalSignalGenerator.Enabled = false;
			for (int k=0;k<channelCount;k++) {
				conf.Channels[k].Acquire = true;
				conf.Channels[k].ChannelNumber = k+1;
				conf.Channels[k].BandpassFilterIndex = -1;
				conf.Channels[k].NotchFilterIndex = -1;
			}
			if (!GT_SetConfiguration(hDevice,conf))
				throw std::runtime_error("Could not set amplifier configuration.");

			// try to get the serial number
			char buffer[1024]; 
			std::string serialNumber = GT_GetSerial(hDevice,buffer,sizeof(buffer)) ? buffer : "unknown";

			// start acquisition
			GT_Start(hDevice);

			// start reader thread
			stop_ = false;
			reader_thread_.reset(new boost::thread(&MainWindow::read_thread,this,deviceNumber,chunkSize,samplingRate,isSlave,serialNumber,channelCount,channelLabels));
		}
		catch(std::exception &e) {
			// try to decode the error message
			std::string msg = "Could not query driver message because the device is not open";
			if (hDevice>0) {
				WORD ec; char buffer[1024];
				if (GT_GetLastError(&ec,buffer))
					msg.assign(buffer,strlen(buffer));
				else
					msg = "Could not retrieve error message";
				GT_CloseDevice(&hDevice);
				hDevice = NULL;
			}
			if (hEvent) {
				CloseHandle(hEvent);
				hEvent = NULL;
			}
			if (pOverlapped) {
				delete pOverlapped;
				pOverlapped = NULL;
			}
			QMessageBox::critical(this,"Error",("Could not initialize the gHIamp interface: "+(e.what()+(" (driver message: "+msg+")"))).c_str(),QMessageBox::Ok);
			return;
		}

		// done, all successful
		ui->linkButton->setText("Unlink");
	}
}

// background data reader thread
void MainWindow::read_thread(std::string deviceNumber, int chunkSize, int samplingRate, bool isSlave, std::string serialNumber, int channelCount, std::vector<std::string> channelLabels) {
	// reserve buffers to receive and send data
	int chunk_bytes = ceil((chunkSize*(channelCount+1)*sizeof(float))/(double)MAX_USB_PACKET_SIZE)*MAX_USB_PACKET_SIZE;
	char *recv_buffer = new char[chunk_bytes];
	float *src_buffer = reinterpret_cast<float*>(recv_buffer);
	std::vector<std::vector<float> > send_buffer(chunkSize,std::vector<float>(channelCount));
	try {
		// create data streaminfo and append some meta-data
		lsl::stream_info data_info("g.USBamp-"+deviceNumber,"EEG",channelCount,samplingRate,lsl::cf_float32,"gHIamp_" + deviceNumber + "_" + boost::lexical_cast<std::string>(serialNumber));
		lsl::xml_element channels = data_info.desc().append_child("channels");
		for (int k=0;k<channelLabels.size();k++)
			channels.append_child("channel")
				.append_child_value("label",channelLabels[k].c_str())
				.append_child_value("type","EEG")
				.append_child_value("unit","microvolts");
		data_info.desc().append_child("amplifier").append_child("settings")
			.append_child_value("is_slave",isSlave?"true":"false");
		data_info.desc().append_child("acquisition")
			.append_child_value("manufacturer","g.Tec")
			.append_child_value("serial_number",boost::lexical_cast<std::string>(serialNumber).c_str());
		// make a data outlet
		lsl::stream_outlet data_outlet(data_info);

		// create marker streaminfo and outlet
		lsl::stream_info marker_info("gHIamp-"+deviceNumber+"Markers","Markers",1,0,lsl::cf_string,"gHIamp_" + boost::lexical_cast<std::string>(deviceNumber) + "_" + boost::lexical_cast<std::string>(serialNumber) + "_markers");
		lsl::stream_outlet marker_outlet(marker_info);
			
		// enter transmission loop
		DWORD bytes_read;
		while (!stop_) {
			// read chunk into recv_buffer
			if (GT_GetData(hDevice,(BYTE*)recv_buffer,chunk_bytes,pOverlapped)) {
				if (WaitForSingleObject(pOverlapped->hEvent,1000)==WAIT_OBJECT_0) {
					if (GT_GetOverlappedResult(hDevice, pOverlapped, &bytes_read, FALSE) && bytes_read) {
						// reformat into send_buffer
						for (int s=0;s<chunkSize;s++)
							for (int c=0;c<channelCount;c++)
								send_buffer[s][c] = src_buffer[c + s*(channelCount+1)];
						double now = lsl::local_clock();
						// push data chunk into the outlet
						data_outlet.push_chunk(send_buffer,now);
						// push markers into outlet
						int last_mrk = 0;
						for (int s=0;s<chunkSize;s++)
							if (int mrk=src_buffer[channelCount + s*(channelCount+1)]) {
								if (mrk != last_mrk) {
									std::string mrk_string = boost::lexical_cast<std::string>(mrk);
									marker_outlet.push_sample(&mrk_string,now + (s + 1 - chunkSize)/samplingRate);
									last_mrk = mrk;
								}
							}
					}
				}
			}
		}
	}
	catch(boost::thread_interrupted &) {
		// thread was interrupted: no error
	}
	catch(std::exception &e) {
		// any other error
		QMessageBox::critical(this,"Error",(std::string("Error during processing: ")+=e.what()).c_str(),QMessageBox::Ok);
	}
	delete recv_buffer;
}

MainWindow::~MainWindow() {
	delete ui;
}
